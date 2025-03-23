#pragma once

#include "Encoder.h"

namespace my_ffmpeg{

class VideoEncoder:public Encoder{
	class Handler;
	Handler* handler;
protected:
	int fps;
	double step()const override;
	vector<Frame> convertFormat(const vector<Frame>& source)override;
	vector<Frame> flushBuffer()override;
public:
	VideoEncoder(){}
	VideoEncoder(VideoFormat format,AVCodecID id=AV_CODEC_ID_H264,int fps_=30,int64_t bit_rate=0,const std::map<string,string>& opts={});
	~VideoEncoder()noexcept;
	SWAP(VideoEncoder){
		Encoder::swap(o);
		std::swap(fps,o.fps);
		std::swap(handler,o.handler);
	}
};

}