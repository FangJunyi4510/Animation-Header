#pragma once

#include "Decoder.h"
#include "AVFormat.h"
#include <array>

namespace my_ffmpeg{

class AVInput:public AVFormat{
	struct Stream{
		Decoder decoder;
		AVRational timeBase;
		deque<Packet> packets;
		deque<Frame> buffer;
		Stream(){}
		SWAP(Stream){
			decoder.swap(o.decoder);
			std::swap(timeBase,o.timeBase);
			packets.swap(o.packets);
			buffer.swap(o.buffer);
		}
		void pushBuffer(const vector<Frame>& frames);
		bool isEmptyBuffer();
		bool decodeOne();
		vector<Frame> popBuffer(int maxCount=-1);
	};
	std::array<Stream,AVMEDIA_TYPE_NB> streams;
public:
	AVRational getTimeBase(AVMediaType type)const;
	vector<Frame> read(AVMediaType type,int maxCount=-1);
	AVInput(string url="");
	SWAP(AVInput){
		AVFormat::swap(o);
		for(uint i=0;i<streams.size();++i){
			streams[i].swap(o.streams[i]);
		}
	}
};

}