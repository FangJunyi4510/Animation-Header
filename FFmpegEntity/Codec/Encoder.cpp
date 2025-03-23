#include "Encoder.h"

namespace my_ffmpeg{

void Encoder::setpts(vector<Frame>& source){
	for(auto& each:source){
		each.data()->pts=curTime/stream->time_base;
		curTime+=step();
	}
}
vector<Frame> Encoder::pretreat(const vector<Frame>& source){
	auto ret=convertFormat(source);
	setpts(ret);
	return ret;
}
vector<Packet> Encoder::aftertreat(const vector<Packet>& products)const{
	auto ret=products;
	for(auto& each:ret){
		each.data()->stream_index=stream->index;
	}
	return ret;
}
Encoder::Encoder(AVCodecID id,int64_t bit_rate,const std::map<std::string,std::string>& opts){
	codec=avcodec_find_encoder(id);
	if(!codec){
		throw CodecError("No available encoder");
	}
	context=avcodec_alloc_context3(codec);
	context->codec_id = id;
	context->codec_type = avcodec_get_type(id);
	context->bit_rate = bit_rate;
	set(options,opts);
}
AVMediaType Encoder::type()const{
	return context->codec_type;
}
void Encoder::configure(){
	readStream();
	open();
}

}