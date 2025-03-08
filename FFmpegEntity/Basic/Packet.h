#pragma once

#include "Color.h"

extern "C"{
#include <libavformat/avformat.h>
}

namespace my_ffmpeg{

class Packet{
	AVPacket* m_data=nullptr;
public:
	Packet();
	~Packet()noexcept;
	SWAP(Packet){
		std::swap(m_data,o.m_data);
	}
	COPY(Packet):m_data(av_packet_clone(o.m_data)){
		if(!m_data){
			m_data=av_packet_alloc();
		}
	}
	CLONE(clone,Packet,Packet*)
	AVPacket* data()const;
	void unref()const;
};

}
