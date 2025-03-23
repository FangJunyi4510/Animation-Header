#include "Frame.h"
#include <cassert>

namespace my_ffmpeg{

Frame::Frame():m_data(av_frame_alloc()){}
Frame::~Frame()noexcept{
	av_frame_free(&m_data);
}
void Frame::unref()const{
	av_frame_unref(m_data);
}

VideoFrame::VideoFrame():VideoFrame(0,0){}
VideoFrame::VideoFrame(int width,int height,Color color):data(height,RGBALine(width,color)){}
VideoFrame::VideoFrame(my_ffmpeg::Frame frame):VideoFrame(frame.data()->width,frame.data()->height){
	frame=my_ffmpeg::Swscale(frame,{width(),height(),AV_PIX_FMT_RGBA64}).scale(frame);
	for(int i=0;i<height();++i){
		for(int j=0;j<width();++j){
			const uint16_t* pixel=reinterpret_cast<const uint16_t*>(frame.data()->data[0]+(frame.data()->linesize[0]*i+4*sizeof(uint16_t)*j));
			data[i][j]=Color(pixel[0],pixel[1],pixel[2],pixel[3]);
		}
	}
}
void VideoFrame::clear(){
	*this=VideoFrame();
}
void VideoFrame::setWidth(int w){
	for(auto& line:data){
		line.resize(w);
	}
}
void VideoFrame::setHeight(int h){
	data.resize(h);
}
my_ffmpeg::VideoFormat VideoFrame::format()const{
	return {width(),height(),AV_PIX_FMT_RGBA64};
}
my_ffmpeg::Frame VideoFrame::toFrame()const{
	my_ffmpeg::Frame ret;
	ret.data()->width=width();
	ret.data()->height=height();
	ret.data()->format=AV_PIX_FMT_RGBA64;
	av_frame_get_buffer(ret.data(),0);
	for(int i=0;i<height();++i){
		memcpy(ret.data()->data[0]+ret.data()->linesize[0]*i,data[i].data(),width()*sizeof(Color));
	}
	return ret;
}
my_ffmpeg::Frame VideoFrame::toFrame(my_ffmpeg::VideoFormat resFormat)const{
	return my_ffmpeg::Swscale(format(),resFormat).scale(toFrame());
}

void AudioBuffer::flushConverter(){
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
AudioBuffer::AudioBuffer(my_ffmpeg::AudioFormat fmt):
	m_format(fmt),data(m_format.channelLayout.nb_channels),curFormat(m_format),converter(curFormat,m_format){}
void AudioBuffer::push(const vector<my_ffmpeg::Frame>& frames){
	for(const auto& frame:frames){
		if(AudioFormat(frame).sampleFormat==-1){
			continue;
		}
		if(frame!=curFormat){
			flushConverter();
			converter.swap(my_ffmpeg::SwResample(frame,m_format));
		}
		converter.send(frame.data()->data,frame.data()->nb_samples);
	}
}
int AudioBuffer::size()const{
	if(data.empty()){
		return 0;
	}
	return data[0].size()+converter.samplesCount();
}
my_ffmpeg::AudioFormat AudioBuffer::format()const{
	return m_format;
}
int AudioBuffer::sampleBytes()const{
	return av_get_bytes_per_sample(m_format.sampleFormat);
}
my_ffmpeg::Frame AudioBuffer::pop(int frameSize){
	frameSize=std::min(size(),frameSize);
	if(frameSize==0){
		return Frame();
	}
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
vector<my_ffmpeg::Frame> AudioBuffer::pop(int frameSize,int count){
	vector<my_ffmpeg::Frame> ret;
	for(int i=0;i<count;++i){
		ret.push_back(pop(frameSize));
	}
	return ret;
}
vector<my_ffmpeg::Frame> AudioBuffer::flush(int frameSize){
	if(frameSize==0){
		frameSize=1024;
	}
	vector<my_ffmpeg::Frame> ret;
	while(size()>=frameSize){
		ret.push_back(pop(frameSize));
	}
	ret.push_back(pop(size()));
	return ret;
}

}
