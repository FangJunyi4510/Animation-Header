#include "AVOutput.h"
#include <array>
#include <cassert>

namespace my_ffmpeg{

AVOutput::AVOutput(string filename,const vector<Encoder*>& arg_encoders){
	for(auto& each:arg_encoders){
		Encoder*& encoder=encoders[each->type()];
		if(encoder){
			delete encoder;
		}
		encoder=each;
	}
	if(avformat_alloc_output_context2(&context,nullptr,nullptr,filename.c_str())<0){
		throw FFmpegError("init failed");
	}
	if(avio_open(&context->pb,filename.c_str(),AVIO_FLAG_WRITE)<0){
		throw FileError("Cannot open file "+filename);
	}
	for(auto& each:encoders){
		if(!each){
			continue;
		}
		each->setStream(avformat_new_stream(context,nullptr));
	}

	if(avformat_write_header(context,nullptr)<0){
		throw FileError("Writing header failed");
	}
	for(auto& each:encoders){
		if(!each){
			continue;
		}
		each->configure();
	}
}
void AVOutput::encode(AVMediaType type,const vector<Frame>& frames){
	if(!encoders[type]){
		return;
	}
	encoders[type]->send(frames);
}
void AVOutput::flush(){
	vector<Packet> buffer;
	for(const auto& each:encoders){
		if(!each){
			continue;
		}
		auto tmp=each->receive();
		buffer.insert(buffer.end(),tmp.begin(),tmp.end());
	}
	sort(buffer.begin(),buffer.end(),[=](const Packet& a,const Packet& b){
		return a.data()->dts<b.data()->dts;
	});
	for(auto each:buffer){
		av_interleaved_write_frame(context,each.data());
	}
}
void AVOutput::close(){
	for(const auto& each:encoders){
		if(!each){
			continue;
		}
		each->end();
	}
	flush();
	av_write_trailer(context);
	avio_close(context->pb);
	for(auto& each:encoders){
		delete each;
		each=nullptr;
	}
}
AVOutput::~AVOutput(){
	close();
}

}