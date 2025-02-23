#pragma once

#include "Color.h"
#include <exception>

extern "C"{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <cassert>
}
#include <sstream>
#include <deque>

namespace my_ffmpeg{

class Packet{
	AVPacket* m_data=nullptr;
public:
	Packet():m_data(av_packet_alloc()){
	}
	~Packet()noexcept{
		av_packet_free(&m_data);
	}
	SWAP(Packet){
		std::swap(m_data,o.m_data);
	}
	COPY(Packet):m_data(av_packet_clone(o.m_data)){
		if(!m_data){
			m_data=av_packet_alloc();
		}
	}
	CLONE(clone,Packet,Packet*)
	AVPacket* data()const{
		assert(m_data && "Packet data NULL! ");
		return m_data;
	}
	void unref()const{
		av_packet_unref(data());
	}
};

class Frame;


struct VideoFormat{
	int width,height;
	AVPixelFormat pix_fmt;
	VideoFormat(const Frame&);
	VideoFormat(int w,int h,AVPixelFormat pix_fmt_):width(w),height(h),pix_fmt(pix_fmt_){}
};
bool operator==(const VideoFormat&,const VideoFormat&);
bool operator!=(const VideoFormat&,const VideoFormat&);

class Swscale{
	SwsContext* context=nullptr;
public:
	Swscale(){}
	Swscale(VideoFormat src,VideoFormat dst);
	~Swscale()noexcept{
		sws_freeContext(context);
	}
	SWAP(Swscale){
		std::swap(context,o.context);
	}
	COPY(Swscale)=delete;
	Frame scale(const Frame& src)const;
};

class Frame{
	AVFrame* m_data=nullptr;
public:
	Frame():m_data(av_frame_alloc()){}
	~Frame()noexcept{
		av_frame_free(&m_data);
	}
	SWAP(Frame){
		std::swap(m_data,o.m_data);
	}
	COPY(Frame):m_data(av_frame_clone(o.m_data)){
		if(!m_data){
			m_data=av_frame_alloc();
		}
	}
	CLONE(clone,Frame,Frame*)
	void unref()const{
		av_frame_unref(m_data);
	}
	AVFrame* data()const{
		assert(m_data && "Frame data NULL! ");
		return m_data;
	}
};

struct AudioFormat{
	AVChannelLayout channelLayout;
	AVSampleFormat sampleFormat;
	int sampleRate;
	AudioFormat(const Frame& frame):AudioFormat(frame.data()->ch_layout,AVSampleFormat(frame.data()->format),frame.data()->sample_rate){}
	AudioFormat(AVChannelLayout ch_layout,AVSampleFormat sampleFormat,int sampleRate_):
		channelLayout(ch_layout),sampleFormat(sampleFormat),sampleRate(AVPixelFormat(sampleRate_)){}
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
	SwResample(AudioFormat src,AudioFormat dst){
		swr_alloc_set_opts2(&context,&dst.channelLayout,dst.sampleFormat,dst.sampleRate,&src.channelLayout,src.sampleFormat,src.sampleRate,0,nullptr);
		swr_init(context);
	}
	void send(const uint8_t*const* in,int count)const{
		swr_convert(context,nullptr,0,in,count);
	}
	int receive(uint8_t*const* out,int count)const{
		return swr_convert(context,out,count,nullptr,0);
	}
	int samplesCount()const{
		return swr_get_out_samples(context,0);
	}
	~SwResample(){
		swr_free(&context);
	}
	SWAP(SwResample){
		std::swap(context,o.context);
	}
	COPY(SwResample)=delete;
};

}
using std::deque;

namespace anim{

class VideoFrame{
	using RGBALine=std::vector<Color>;
	using RGBABuffer=std::vector<RGBALine>;
	RGBABuffer data;
public:
	VideoFrame():VideoFrame(0,0){}
	VideoFrame(int width,int height,Color color=Color()):data(height,RGBALine(width,color)){}
	VideoFrame(my_ffmpeg::Frame frame):VideoFrame(frame.data()->width,frame.data()->height){
		frame=my_ffmpeg::Swscale(frame,{width(),height(),AV_PIX_FMT_RGBA64}).scale(frame);
		for(int i=0;i<height();++i){
			for(int j=0;j<width();++j){
				const uint16_t* pixel=reinterpret_cast<const uint16_t*>(frame.data()->data[0]+(frame.data()->linesize[0]*i+4*sizeof(uint16_t)*j));
				data[i][j]=Color(pixel[0],pixel[1],pixel[2],pixel[3]);
			}
		}
	}
	void clear(){
		*this=VideoFrame();
	}
	int width()const{
		if(data.empty()){
			return 0;
		}
		return data[0].size();
	}
	int height()const{
		return data.size();
	}
	void setWidth(int w){
		for(auto& line:data){
			line.resize(w);
		}
	}
	void setHeight(int h){
		data.resize(h);
	}
	RGBALine& operator[](int h){
		return data[h];
	}
	const RGBALine& operator[](int h)const{
		return data[h];
	}
	my_ffmpeg::VideoFormat format()const{
		return {width(),height(),AV_PIX_FMT_RGBA64};
	}
	my_ffmpeg::Frame toFrame()const{
		my_ffmpeg::Frame ret;
		ret.data()->width=width();
		ret.data()->height=height();
		ret.data()->format=AV_PIX_FMT_RGBA64;
		av_frame_get_buffer(ret.data(),0);
		for(int i=0;i<height();++i){
			for(int j=0;j<width();++j){
				uint16_t* pixel=reinterpret_cast<uint16_t*>(ret.data()->data[0]+(ret.data()->linesize[0]*i+4*sizeof(uint16_t)*j));
				pixel[0]=data[i][j].red;
				pixel[1]=data[i][j].green;
				pixel[2]=data[i][j].blue;
				pixel[3]=data[i][j].alpha;
			}
		}
		return ret;
	}
	my_ffmpeg::Frame toFrame(my_ffmpeg::VideoFormat resFormat)const{
		return my_ffmpeg::Swscale(format(),resFormat).scale(toFrame());
	}
};

class AudioBuffer{
	my_ffmpeg::AudioFormat m_format;
	vector<deque<vector<uint8_t>>> data;
	my_ffmpeg::AudioFormat curFormat;
	my_ffmpeg::SwResample converter;
	void flushConverter(){
		uint8_t** buf=new uint8_t*[m_format.channelLayout.nb_channels];
		int len=converter.samplesCount();
		for(int i=0;i<m_format.channelLayout.nb_channels;++i){
			buf[i]=new uint8_t[len*sampleBytes()];
		}
		converter.receive(buf,len);
		for(int i=0;i<m_format.channelLayout.nb_channels;++i){
			for(int j=0;j<len;++j){
				data[i].push_back(vector<uint8_t>(buf[i]+j*sampleBytes(),buf[i]+(j+1)*sampleBytes()));
			}
			delete[] buf[i];
		}
		delete[] buf;
	}
public:
	AudioBuffer(my_ffmpeg::AudioFormat fmt):
		m_format(fmt),data(m_format.channelLayout.nb_channels),curFormat(m_format),converter(curFormat,m_format){}
	void push(const vector<my_ffmpeg::Frame>& frames){
		for(const auto& frame:frames){
			if(frame!=curFormat){
				flushConverter();
				converter.swap(my_ffmpeg::SwResample(frame,m_format));
			}
			converter.send(frame.data()->data,frame.data()->nb_samples);
		}
	}
	int size()const{
		if(data.empty()){
			return 0;
		}
		return data[0].size()+converter.samplesCount();
	}
	my_ffmpeg::AudioFormat format()const{
		return m_format;
	}
	int sampleBytes()const{
		return av_get_bytes_per_sample(m_format.sampleFormat);
	}
	my_ffmpeg::Frame pop(int frameSize){
		assert(size()>=frameSize && "pop frameSize > size! ");
		if(int(data[0].size())<frameSize){
			flushConverter();
		}
		my_ffmpeg::Frame ret;
		ret.data()->ch_layout=m_format.channelLayout;
		ret.data()->sample_rate=m_format.sampleRate;
		ret.data()->format=m_format.sampleFormat;
		ret.data()->nb_samples=frameSize;
		av_frame_get_buffer(ret.data(),0);
		for(int i=0;i<m_format.channelLayout.nb_channels;++i){
			for(int j=0;j<frameSize;++j){
				for(int k=0;k<sampleBytes();++k){
					ret.data()->data[i][j*sampleBytes()+k]=data[i].front()[k];
				}
				data[i].pop_front();
			}
		}
		return ret;
	}
	vector<my_ffmpeg::Frame> pop(int frameSize,int count){
		assert(size()>=frameSize*count && "pop frameSize*count > size! ");
		vector<my_ffmpeg::Frame> ret;
		for(int i=0;i<count;++i){
			ret.push_back(pop(frameSize));
		}
		return ret;
	}
	vector<my_ffmpeg::Frame> flush(int frameSize){
		vector<my_ffmpeg::Frame> ret;
		while(size()>=frameSize){
			ret.push_back(pop(frameSize));
		}
		ret.push_back(pop(size()));
		return ret;
	}
};

}