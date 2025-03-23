#pragma once

#include "Frame.h"
#include "Packet.h"
#include <map>

extern "C"{
#include <libavcodec/avcodec.h>
}

namespace my_ffmpeg{

double operator*(int64_t tick,AVRational timeBase);
double operator*(AVRational timeBase,int64_t tick);
int64_t operator/(double time,AVRational timeBase);

void set(AVDictionary*& options,const std::map<string,string>& opts);


/**
 * Codec:
 *   send():
 *     pretreat() 进入子类缓冲区 ==> send_() 进入AVCodec内部缓冲区 & receive_() ==> 进入缓冲区
 *   receive():
 *     取出缓冲区 ==> aftertreat()
 *   end():
 *     flushBuffer() 清空子类缓冲区 ==> send_(nullptr) 清空AVCodec内部缓冲区 ==> receive_() 进入缓冲区
 *     调用后不能再调用 send()
 * 
 */
template<class fromT,class toT,int(*send_)(AVCodecContext*,const typename std::decay<decltype(*fromT().data())>::type*),int(*receive_)(AVCodecContext*,decltype(toT().data()))>
class Codec{
	deque<toT> outputBuffer;
protected:
	const AVCodec* codec=nullptr;
	AVCodecContext* context=nullptr;
	AVDictionary* options=nullptr;
	AVStream* stream=nullptr;

	void open();
	void readStream();
	void writeStream()const;
	
	virtual vector<fromT> pretreat(const vector<fromT>& source);
	virtual vector<toT> aftertreat(const vector<toT>& products)const;
	virtual vector<fromT> flushBuffer();
public:
	Codec(){}
	virtual ~Codec()noexcept;
	SWAP(Codec){
		std::swap(codec,o.codec);
		std::swap(context,o.context);
	}
	COPY(Codec)=delete;

	void setStream(AVStream* s);
	
	void send(const vector<fromT>& source);
	vector<toT> receive(int maxCount=-1);
	void end();
};

using BasicEncoder=Codec<Frame,Packet,avcodec_send_frame,avcodec_receive_packet>;
using BasicDecoder=Codec<Packet,Frame,avcodec_send_packet,avcodec_receive_frame>;

}