#pragma once

#include "Codec.h"
#include <array>
#include "../../../include/Char033.h"

namespace my_ffmpeg{

using std::string;

class Format{
protected:
	AVFormatContext* context=nullptr;
public:
	Format(){}
	virtual ~Format()noexcept{
		avformat_free_context(context);
	}
	SWAP(Format){
		std::swap(context,o.context);
	}
	COPY(Format)=delete;
};

class FormatInput:public Format{
	struct Stream{
		Decoder decoder;
		AVRational timeBase;
		deque<Packet> packets;
		deque<Frame> buffer;
		Stream(){}
		SWAP(Stream){
			decoder.swap(o.decoder);
			std::swap(timeBase,o.timeBase);
			packets.swap(o.packets);
			buffer.swap(o.buffer);
		}
		void pushBuffer(const vector<Frame>& frames){
			buffer.insert(buffer.end(),frames.begin(),frames.end());
		}
		bool isEmptyBuffer(){
			if(!buffer.empty() || !packets.empty()){
				return false;
			}
			decoder.flush();
			pushBuffer(decoder.receive());
			return buffer.empty();
		}
		bool decodeOne(){
			while(buffer.empty()){
				if(isEmptyBuffer()){
					return false;
				}
				if(!buffer.empty()){
					return true;
				}
				decoder.send({packets.front()});
				packets.pop_front();
				pushBuffer(decoder.receive());
			}
			return true;
		}
		vector<Frame> popBuffer(int maxCount=-1){
			vector<Frame> ret;
			while(decodeOne() && maxCount--){
				ret.push_back(buffer.front());
				buffer.pop_front();
			}
			return ret;
		}
	};
	std::array<Stream,AVMEDIA_TYPE_NB> streams;
public:
	AVRational getTimeBase(AVMediaType type)const{
		return streams[type].timeBase;
	}
	vector<Frame> read(AVMediaType type,int maxCount=-1){
		return streams[type].popBuffer(maxCount);
	}
	FormatInput(string url=""){
		if(avformat_open_input(&context,url.c_str(),nullptr,nullptr)){
			assert(!"open failed");
			return;
		}
		if(avformat_find_stream_info(context,nullptr)){
			assert(!"finding stream info failed");
			return;
		}
		for(u_int i=0;i<context->nb_streams;++i){
			auto type=context->streams[i]->codecpar->codec_type;
			streams[type].decoder.swap(Decoder(context->streams[i]));
			streams[type].timeBase=context->streams[i]->time_base;
		}
		Packet packet;
		while(!av_read_frame(context, packet.data())) {
			streams[context->streams[packet.data()->stream_index]->codecpar->codec_type].packets.push_back(packet);
			packet.unref();
		}
		std::clog<<c033::pGreen;
		for(uint i=0;i<context->nb_streams;++i){
			av_dump_format(context,i,url.c_str(),0);// debug
		}
		std::clog<<c033::pNone;
		avformat_close_input(&context);
	}
	SWAP(FormatInput){
		Format::swap(o);
		for(uint i=0;i<streams.size();++i){
			streams[i].swap(o.streams[i]);
		}
	}
};

class FormatOutput:public Format{
	std::array<Encoder*,AVMEDIA_TYPE_NB> encoders={};
public:
	FormatOutput(string filename,const vector<Encoder*>& arg_encoders){
		for(auto& each:arg_encoders){
			Encoder*& encoder=encoders[each->type()];
			if(encoder){
				delete encoder;
			}
			encoder=each;
		}
		// 初始化输出码流的AVFormatContext
		if(avformat_alloc_output_context2(&context,nullptr,nullptr,filename.c_str())<0){
			assert(!"init failed");
			return;
		}
		// 打开输出文件
		if(avio_open(&context->pb,filename.c_str(),AVIO_FLAG_WRITE)<0){
			assert(!"open failed");
			return;
		}
		for(auto& each:encoders){
			if(!each){
				continue;
			}
			each->setStream(avformat_new_stream(context,nullptr));
		}

		if(avformat_write_header(context,nullptr)<0){
			assert(!"write header failed");
			return;
		}
		for(auto& each:encoders){
			if(!each){
				continue;
			}
			each->configure();
		}
		std::clog<<c033::pGreen;
		for(uint i=0;i<context->nb_streams;++i){
			av_dump_format(context,i,filename.c_str(),1);// debug
		}
		std::clog<<c033::pNone;
	}
	void encode(AVMediaType type,const vector<Frame>& frames){
		if(!encoders[type]){
			return;
		}
		encoders[type]->send(frames);
	}
	void flush(){
		vector<Packet> buffer;
		for(const auto& each:encoders){
			if(!each){
				continue;
			}
			auto tmp=each->receive();
			buffer.insert(buffer.end(),tmp.begin(),tmp.end());
		}
		sort(buffer.begin(),buffer.end(),[=](const Packet& a,const Packet& b){
			return a.data()->dts<b.data()->dts;
		});
		for(auto each:buffer){
			av_write_frame(context,each.data());
		}
	}
	void close(){
		for(const auto& each:encoders){
			if(!each){
				continue;
			}
			each->flush();
		}
		flush();
		av_write_trailer(context);
		avio_close(context->pb);
		for(auto& each:encoders){
			delete each;
			each=nullptr;
		}
	}
	~FormatOutput(){
		close();
	}
	SWAP(FormatOutput,"",{}){
		Format::swap(o);
		encoders.swap(o.encoders);
	}
};

}