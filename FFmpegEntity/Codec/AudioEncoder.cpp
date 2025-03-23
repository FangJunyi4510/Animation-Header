#include "AudioEncoder.h"

namespace my_ffmpeg{

double AudioEncoder::step()const{
	return context->frame_size*1.0/context->sample_rate;
}
vector<Frame> AudioEncoder::convertFormat(const vector<Frame>& source){
	buffer.push(source);
	if(context->frame_size==0){
		context->frame_size=1024;
	}
	return buffer.pop(context->frame_size,buffer.size()/context->frame_size);
}
vector<Frame> AudioEncoder::flushBuffer(){
	return buffer.flush(context->frame_size);
} 
AudioEncoder::AudioEncoder():buffer({AV_CHANNEL_LAYOUT_STEREO,AV_SAMPLE_FMT_DBLP,44100}){}
AudioEncoder::AudioEncoder(AudioFormat format,AVCodecID id,int64_t bit_rate,const std::map<string,string>& opts):Encoder(id,bit_rate,opts),buffer(format){
	context->ch_layout=format.channelLayout;
	context->sample_fmt=format.sampleFormat;
	context->sample_rate=format.sampleRate;

	context->time_base={1,format.sampleFormat};
}

}