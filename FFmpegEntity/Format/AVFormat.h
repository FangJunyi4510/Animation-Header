#pragma once

#include "Frame.h"

namespace my_ffmpeg{

using std::string;

class AVFormat{
protected:
	AVFormatContext* context=nullptr;
public:
	AVFormat(){}
	virtual ~AVFormat()noexcept{
		avformat_free_context(context);
	}
	SWAP(AVFormat){
		std::swap(context,o.context);
	}
	COPY(AVFormat)=delete;
};

}