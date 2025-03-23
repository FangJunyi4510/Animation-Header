#include "Packet.h"
#include <cassert>

namespace my_ffmpeg{

Packet::Packet():m_data(av_packet_alloc()){}
Packet::~Packet()noexcept{
	av_packet_free(&m_data);
}
AVPacket* Packet::data()const{
	return m_data;
}
void Packet::unref()const{
	av_packet_unref(data());
}

}