#include "Formats.h"
#include "Frame.h"

namespace my_ffmpeg{

Swscale::Swscale(VideoFormat src,VideoFormat dst):
    context(sws_getContext(src.width,src.height,src.pix_fmt,dst.width,dst.height,dst.pix_fmt,SWS_BILINEAR,nullptr,nullptr,nullptr)){}

Frame Swscale::scale(const Frame& src)const{
	Frame ret;
	sws_scale_frame(context,ret.data(),src.data());
	return ret;
}
Swscale::~Swscale()noexcept{
	sws_freeContext(context);
}

VideoFormat::VideoFormat(const Frame& frame):VideoFormat(frame.data()->width,frame.data()->height,AVPixelFormat(frame.data()->format)){}
bool operator==(const VideoFormat& a,const VideoFormat& b){
	return a.width==b.width && a.height==b.height && a.pix_fmt==b.pix_fmt;
}
bool operator!=(const VideoFormat& a,const VideoFormat& b){
	return !(a==b);
}

bool operator==(const AudioFormat& a,const AudioFormat& b){
	return !av_channel_layout_compare(&a.channelLayout,&b.channelLayout) && a.sampleFormat==b.sampleFormat && a.sampleRate==b.sampleRate;
}
bool operator!=(const AudioFormat& a,const AudioFormat& b){
	return !(a==b);
}

AudioFormat::AudioFormat(const Frame& frame):AudioFormat(frame.data()->ch_layout,AVSampleFormat(frame.data()->format),frame.data()->sample_rate){}
AudioFormat::AudioFormat(AVChannelLayout ch_layout,AVSampleFormat sampleFormat,int sampleRate_):
	channelLayout(ch_layout),sampleFormat(sampleFormat),sampleRate(AVPixelFormat(sampleRate_)){}

SwResample::SwResample(AudioFormat src,AudioFormat dst){
	swr_alloc_set_opts2(&context,&dst.channelLayout,dst.sampleFormat,dst.sampleRate,&src.channelLayout,src.sampleFormat,src.sampleRate,0,nullptr);
	swr_init(context);
}
void SwResample::send(const uint8_t*const* in,int count)const{
	swr_convert(context,nullptr,0,in,count);
}
int SwResample::receive(uint8_t*const* out,int count)const{
	return swr_convert(context,out,count,nullptr,0);
}
int SwResample::samplesCount()const{
	return swr_get_out_samples(context,0);
}
SwResample::~SwResample(){
	swr_free(&context);
}

}