#include "VideoEncoder.h"

namespace my_ffmpeg{

double VideoEncoder::step()const{
	return 1.0/fps;
}
vector<Frame> VideoEncoder::convertFormat(const vector<Frame>& source){
	vector<Frame> ret;
	if(source.empty()){
		return ret;
	}
	const VideoFormat dstFormat{context->width,context->height,context->pix_fmt};
	VideoFormat curFormat(source[0]);
	Swscale converter(curFormat,dstFormat);
	for(const auto& frame:source){
		auto res=frame;
		if(res!=dstFormat){
			if(frame!=curFormat){
				converter.swap(Swscale(frame,dstFormat));
			}
			res=converter.scale(frame);
		}
		ret.push_back(res);
	}
	return ret;
}
VideoEncoder::VideoEncoder(VideoFormat format,AVCodecID id,int fps_,int64_t bit_rate,const std::map<string,string>& opts):Encoder(id,bit_rate,opts),fps(fps_){
	context->pix_fmt = format.pix_fmt;
	context->width = format.width;
	context->height = format.height;

	context->gop_size = fps;   // I帧间隔
	context->max_b_frames = context->gop_size-1;  // B帧 ( I BBB P BBB P BBB P BBB P BBB I )

	context->time_base={1,fps};
	context->framerate={fps,1};
}

}