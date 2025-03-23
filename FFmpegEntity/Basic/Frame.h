#pragma once

#include "Formats.h"
#include <deque>

extern "C"{
#include <libavformat/avformat.h>
}

namespace my_ffmpeg{

using std::deque;

class Frame{
	AVFrame* m_data=nullptr;
public:
	Frame();
	~Frame()noexcept;
	SWAP(Frame){
		std::swap(m_data,o.m_data);
	}
	COPY(Frame):m_data(av_frame_clone(o.m_data)){
		if(!m_data){
			m_data=av_frame_alloc();
		}
	}
	CLONE(clone,Frame,Frame*)
	void unref()const;
	AVFrame* data()const {return m_data;}
};

class VideoFrame{
	using RGBALine=std::vector<Color>;
	using RGBABuffer=std::vector<RGBALine>;
	RGBABuffer data;
public:
	VideoFrame();
	VideoFrame(int width,int height,Color color=Color());
	VideoFrame(Frame frame);
	void clear();
	int width()const {return data.empty()?0:data[0].size();}
	int height()const {return data.size();}
	bool empty()const {return !height() && !width();}
	void setWidth(int w);
	void setHeight(int h);
	RGBALine& operator[](int h) {return data[h];}
	const RGBALine& operator[](int h)const {return data[h];}
	VideoFormat format()const;
	Frame toFrame()const;
	Frame toFrame(VideoFormat resFormat)const;
};

class AudioBuffer{
	AudioFormat m_format;
	vector<deque<vector<uint8_t>>> data;
	AudioFormat curFormat;
	SwResample converter;
	void flushConverter();
public:
	AudioBuffer(AudioFormat fmt);
	void push(const vector<Frame>& frames);
	int size()const;
	AudioFormat format()const;
	int sampleBytes()const;
	Frame pop(int frameSize);
	vector<Frame> pop(int frameSize,int count);
	vector<Frame> flush(int frameSize);
};

}
