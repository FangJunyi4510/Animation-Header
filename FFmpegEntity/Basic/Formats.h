#pragma once

#include "Color.h"

extern "C"{
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

namespace my_ffmpeg{

class Frame;

struct VideoFormat{
	int width=0,height=0;
	AVPixelFormat pix_fmt=AV_PIX_FMT_NONE;
	VideoFormat(){};
	VideoFormat(const Frame&);
	VideoFormat(int w,int h,AVPixelFormat pix_fmt_):width(w),height(h),pix_fmt(pix_fmt_){}
};
bool operator==(const VideoFormat&,const VideoFormat&);
bool operator!=(const VideoFormat&,const VideoFormat&);

class Swscale{
	SwsContext* context=nullptr;
	VideoFormat m_src,m_dst;
public:
	Swscale(){}
	Swscale(const VideoFormat& src,const VideoFormat& dst);
	~Swscale()noexcept;
	SWAP(Swscale){
		std::swap(context,o.context);
	}
	COPY(Swscale)=delete;
	Frame scale(const Frame& src)const;
};

struct AudioFormat{
	AVChannelLayout channelLayout;
	AVSampleFormat sampleFormat;
	int sampleRate;
	AudioFormat(const Frame& frame);
	AudioFormat(AVChannelLayout ch_layout,AVSampleFormat sampleFormat,int sampleRate_);
	SWAP(AudioFormat,AV_CHANNEL_LAYOUT_MONO,AV_SAMPLE_FMT_NONE,0){
		std::swap(channelLayout,o.channelLayout);
		std::swap(sampleFormat,o.sampleFormat);
		std::swap(sampleRate,o.sampleRate);
	}
	COPY(AudioFormat):sampleFormat(o.sampleFormat),sampleRate(o.sampleRate){
		av_channel_layout_copy(&channelLayout,&o.channelLayout);
	}
	CLONE(clone,AudioFormat,AudioFormat*)
};
bool operator==(const AudioFormat&,const AudioFormat&);
bool operator!=(const AudioFormat&,const AudioFormat&);

class SwResample{
	SwrContext* context=nullptr;
public:
	SwResample(){}
	SwResample(AudioFormat src,AudioFormat dst);
	~SwResample();
	SWAP(SwResample){
		std::swap(context,o.context);
	}
	COPY(SwResample)=delete;
	void send(const uint8_t*const* in,int count)const;
	int receive(uint8_t*const* out,int count)const;
	int samplesCount()const;
};

}
