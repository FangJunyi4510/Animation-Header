#include "AVInput.h"
#include <cassert>
#include <iostream>

namespace my_ffmpeg{

AVRational AVInput::getTimeBase(AVMediaType type)const{
	return streams[type].timeBase;
}
vector<Frame> AVInput::read(AVMediaType type,int maxCount){
	return streams[type].popBuffer(maxCount);
}
AVInput::AVInput(string url){
	if(avformat_open_input(&context,url.c_str(),nullptr,nullptr)){
		throw FileError("Cannot open file "+url);
	}
	if(avformat_find_stream_info(context,nullptr)){
		throw FileError("invalid file "+url);
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
	avformat_close_input(&context);
}

void AVInput::Stream::pushBuffer(const vector<Frame>& frames){
	buffer.insert(buffer.end(),frames.begin(),frames.end());
}
bool AVInput::Stream::isEmptyBuffer(){
	if(!buffer.empty() || !packets.empty()){
		return false;
	}
	decoder.end();
	pushBuffer(decoder.receive());
	return buffer.empty();
}


bool AVInput::Stream::decodeOne(){
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
vector<Frame> AVInput::Stream::popBuffer(int maxCount){
	vector<Frame> ret;
	while(decodeOne() && maxCount--){
		ret.push_back(buffer.front());
		buffer.pop_front();
	}
	return ret;
}

}