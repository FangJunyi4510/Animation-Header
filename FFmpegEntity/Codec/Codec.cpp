#include "Codec.h"
#include <string>
#include <thread>
#include <cassert>
#include <iostream>

namespace my_ffmpeg{

void set(AVDictionary*& options,const std::map<std::string,std::string>& opts){
	for(const auto& [key,value]:opts){
		av_dict_set(&options,key.c_str(),value.c_str(),0);
	}
}

double operator*(int64_t tick,AVRational timeBase){
	return tick*1.0*timeBase.num/timeBase.den;
}
double operator*(AVRational timeBase,int64_t tick){
	return tick*1.0*timeBase.num/timeBase.den;
}
int64_t operator/(double time,AVRational timeBase){
    return floor(time*timeBase.den/timeBase.num+0.5);
}

template class Codec<Frame,Packet,avcodec_send_frame,avcodec_receive_packet>;
template class Codec<Packet,Frame,avcodec_send_packet,avcodec_receive_frame>;

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
void Codec<fromT,toT,send_,receive_>::open(){
	context->thread_count=std::thread::hardware_concurrency();
	if(avcodec_open2(context,codec,&options)<0){
		throw CodecError("Cannot open codec");
	}
}

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
void Codec<fromT,toT,send_,receive_>::writeStream()const{
	if(!context){
		return;
	}
	if(avcodec_parameters_from_context(stream->codecpar,context)<0){
		throw CodecError("avcodec_parameters_from_context failed");
	}
}

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
void Codec<fromT,toT,send_,receive_>::readStream(){
	if(!stream){
		return;
	}
	if(avcodec_parameters_to_context(context,stream->codecpar)<0){
		throw CodecError("avcodec_parameters_to_context failed");
	}
	context->pkt_timebase=stream->time_base;
}

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
vector<fromT> Codec<fromT,toT,send_,receive_>::pretreat(const vector<fromT>& source){
	return source;
}

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
vector<fromT> Codec<fromT,toT,send_,receive_>::flushBuffer(){
	return {};
}

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
vector<toT> Codec<fromT,toT,send_,receive_>::aftertreat(const vector<toT>& products)const{
	return products;
}

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
Codec<fromT,toT,send_,receive_>::~Codec()noexcept{
	avcodec_free_context(&context);
	av_dict_free(&options);
}

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
void Codec<fromT,toT,send_,receive_>::setStream(AVStream* s){
	stream=s;
	if(!stream){
		return;
	}
	writeStream();
}

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
void Codec<fromT,toT,send_,receive_>::send(const vector<fromT>& source){
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

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
vector<toT> Codec<fromT,toT,send_,receive_>::receive(int maxCount){
	vector<toT> ret;
	while(!outputBuffer.empty() && maxCount--){
		ret.push_back(outputBuffer.front());
		outputBuffer.pop_front();
	}
	return aftertreat(ret);
}

template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
void Codec<fromT,toT,send_,receive_>::end(){
	send(flushBuffer());
	send_(context,nullptr);
	send({});
}

}