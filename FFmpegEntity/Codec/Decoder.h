#pragma once

#include "Codec.h"

namespace my_ffmpeg{

class Decoder:public BasicDecoder{
public:
	Decoder(AVStream* s=nullptr);
	SWAP(Decoder){
		Codec::swap(o);
	}
};

}