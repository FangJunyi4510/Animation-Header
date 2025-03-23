#pragma once

#include "Encoder.h"
#include "AVFormat.h"
#include <array>

namespace my_ffmpeg{

class AVOutput:public AVFormat{
	std::array<Encoder*,AVMEDIA_TYPE_NB> encoders={nullptr};
public:
	AVOutput(string filename,const vector<Encoder*>& arg_encoders);
	SWAP(AVOutput,"",{}){
		AVFormat::swap(o);
		encoders.swap(o.encoders);
	}
	void encode(AVMediaType type,const vector<Frame>& frames);
	void flush();
	void close();
	~AVOutput();
};

}