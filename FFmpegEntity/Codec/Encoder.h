#pragma once

#include "Codec.h"

namespace my_ffmpeg{

class Encoder:public BasicEncoder{
	double curTime=0;
	void setpts(vector<Frame>& source);
protected:
	virtual double step()const=0;
	virtual vector<Frame> convertFormat(const vector<Frame>& source)=0;
	vector<Frame> pretreat(const vector<Frame>& source)override;
	vector<Packet> aftertreat(const vector<Packet>& products)const override;
public:
	Encoder(AVCodecID id=AV_CODEC_ID_NONE,int64_t bit_rate=0,const std::map<string,string>& opts={});
	~Encoder()noexcept{}
	SWAP(Encoder){
		Codec::swap(o);
	}
	COPY(Encoder)=delete;
	AVMediaType type()const;
	void configure();
};

}