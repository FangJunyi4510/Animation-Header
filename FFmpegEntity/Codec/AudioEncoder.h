#pragma once

#include "Encoder.h"

namespace my_ffmpeg{

class AudioEncoder:public Encoder{
protected:
	AudioBuffer buffer;
	double step()const override;
	vector<Frame> convertFormat(const vector<Frame>& source)override;
	vector<Frame> flushBuffer()override;
public:
	AudioEncoder();
	AudioEncoder(AudioFormat format,AVCodecID id,int64_t bit_rate=0,const std::map<string,string>& opts={});
	~AudioEncoder()noexcept{}
	SWAP(AudioEncoder){
		Encoder::swap(o);
	}
};

}