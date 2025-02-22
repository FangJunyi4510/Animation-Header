#pragma once

#include "Frame.h"
#include <thread>

extern "C"{
#include <libavcodec/avcodec.h>
}
#include <map>



namespace my_ffmpeg{

double operator*(int64_t tick,AVRational timeBase);
double operator*(AVRational timeBase,int64_t tick);
int64_t operator/(double time,AVRational timeBase);

void set(AVDictionary*& options,const std::map<string,string>& opts);

class Codec{
protected:
	const AVCodec* codec=nullptr;
	AVCodecContext* context=nullptr;
	AVDictionary* options=nullptr;
	AVStream* stream=nullptr;
	void open(){
		context->thread_count=std::thread::hardware_concurrency();
		assert(!avcodec_open2(context,codec,&options));
	}
	void writeStream()const{
		if(!context){
			return;
		}
		avcodec_parameters_from_context(stream->codecpar,context);
	}
	void readStream(){
		if(!stream){
			return;
		}
		avcodec_parameters_to_context(context,stream->codecpar);
		context->pkt_timebase=stream->time_base;
	}
public:
	Codec(){}
	virtual ~Codec()noexcept{
		avcodec_free_context(&context);
		av_dict_free(&options);
	}
	SWAP(Codec){
		std::swap(codec,o.codec);
		std::swap(context,o.context);
	}
	COPY(Codec)=delete;
	void setStream(AVStream* s){
		stream=s;
		if(!stream){
			return;
		}
		writeStream();
	}
};

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
class Codec_convert:public Codec{
	deque<toT> outputBuffer;
protected:
	virtual vector<fromT> pretreat(const vector<fromT>& source){
		return source;
	}
	virtual vector<fromT> flushBuffer(){
		return {};
	}
	virtual vector<toT> aftertreat(const vector<toT>& products)const{
		return products;
	}
public:
	void send(const vector<fromT>& source){
		auto res=pretreat(source);
		toT tmp;
		for(const auto& each:res){
			while(send_(context,each.data())==AVERROR(EAGAIN)){
				while(!receive_(context, tmp.data())) {
					outputBuffer.push_back(tmp);
				}
			}
		}
		while(!receive_(context, tmp.data())) {
			outputBuffer.push_back(tmp);
		}
	}
	vector<toT> receive(int maxCount=-1){
		vector<toT> ret;
		while(!outputBuffer.empty() && maxCount--){
			ret.push_back(outputBuffer.front());
			outputBuffer.pop_front();
		}
		return aftertreat(ret);
	}
	void flush(){
		send(flushBuffer());
		send_(context,nullptr);
		send({});
	}
};

class Decoder:public Codec_convert<Packet,Frame,avcodec_send_packet,avcodec_receive_frame>{
public:
	Decoder(AVStream* s=nullptr){
		if(!s){
			return;
		}
		setStream(s);
		codec=avcodec_find_decoder(stream->codecpar->codec_id);
		context=avcodec_alloc_context3(codec);
		readStream();
		open();
	}
	~Decoder()noexcept{}
	SWAP(Decoder){
		Codec::swap(o);
	}
	COPY(Decoder)=delete;
};

class Encoder:public Codec_convert<Frame,Packet,avcodec_send_frame,avcodec_receive_packet>{
	double curTime=0;
	void setpts(vector<Frame>& source){
		for(auto& each:source){
			each.data()->pts=curTime/stream->time_base;
			curTime+=step();
		}
	}
protected:
	virtual double step()const=0;
	virtual vector<Frame> convertFormat(const vector<Frame>& source)=0;
	vector<Frame> pretreat(const vector<Frame>& source)override{
		auto ret=convertFormat(source);
		setpts(ret);
		return ret;
	}
	vector<Packet> aftertreat(const vector<Packet>& products)const override{
		auto ret=products;
		for(auto& each:ret){
			each.data()->stream_index=stream->index;
		}
		return ret;
	}
public:
	Encoder(AVCodecID id=AV_CODEC_ID_NONE,int64_t bit_rate=0,const std::map<string,string>& opts={}){
		codec=avcodec_find_encoder(id);
		if(!codec){
			return;
		}
		context=avcodec_alloc_context3(codec);
		context->codec_id = id;
		context->codec_type = avcodec_get_type(id);
		context->bit_rate = bit_rate;
		set(options,opts);
	}
	~Encoder()noexcept{}
	SWAP(Encoder){
		Codec::swap(o);
	}
	COPY(Encoder)=delete;
	AVMediaType type()const{
		return context->codec_type;
	}
	void configure(){
		readStream();
		open();
	}
};

class VideoEncoder:public Encoder{
	static const string presetMap[10];
protected:
	int fps;
	double step()const override{
		return 1.0/fps;
	}
	vector<Frame> convertFormat(const vector<Frame>& source)override{
		vector<Frame> ret;
		if(source.empty()){
			return ret;
		}
		const VideoFormat dstFormat{context->width,context->height,context->pix_fmt};
		VideoFormat curFormat(source[0]);
		Swscale converter(curFormat,dstFormat);
		for(const auto& frame:source){
			auto res=frame;
			if(res!=dstFormat){
				if(frame!=curFormat){
					converter.swap(Swscale(frame,dstFormat));
				}
				res=converter.scale(frame);
			}
			ret.push_back(res);
		}
		return ret;
	}
public:
	VideoEncoder(){}
	VideoEncoder(VideoFormat format,AVCodecID id=AV_CODEC_ID_H264,int fps_=30,int64_t bit_rate=0,const std::map<string,string>& opts={}):Encoder(id,bit_rate,opts),fps(fps_){
		context->pix_fmt = format.pix_fmt;
		context->width = format.width;
		context->height = format.height;

		context->gop_size = fps;   // I帧间隔
		context->max_b_frames = context->gop_size-1;  // B帧 ( I BBB P BBB P BBB P BBB P BBB I )
	
		context->time_base={1,fps};
		context->framerate={fps,1};
	}
	~VideoEncoder()noexcept{}
	SWAP(VideoEncoder){
		Encoder::swap(o);
		std::swap(fps,o.fps);
	}
};

class AudioEncoder:public Encoder{
protected:
	anim::AudioBuffer buffer;
	double step()const override{
		return context->frame_size*1.0/context->sample_rate;
	}
	vector<Frame> convertFormat(const vector<Frame>& source)override{
		buffer.push(source);
		return buffer.pop(context->frame_size,buffer.size()/context->frame_size);
	}
	vector<Frame> flushBuffer()override{
		return buffer.flush(context->frame_size);
	} 
public:
	AudioEncoder():buffer({AV_CHANNEL_LAYOUT_STEREO,AV_SAMPLE_FMT_DBLP,44100}){}
	AudioEncoder(AudioFormat format,AVCodecID id,int profile=AV_PROFILE_UNKNOWN,int64_t bit_rate=0,const std::map<string,string>& opts={}):Encoder(id,bit_rate,opts),buffer(format){
		context->ch_layout=format.channelLayout;
		context->sample_fmt=format.sampleFormat;
		context->sample_rate=format.sampleRate;

		context->profile = profile;
		context->time_base={1,format.sampleFormat};
	}
	~AudioEncoder()noexcept{}
	SWAP(AudioEncoder){
		Encoder::swap(o);
	}
};

}