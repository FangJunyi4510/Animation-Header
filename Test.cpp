#include "Animation.h"
#include "VideoEncoder.h"
#include "AudioEncoder.h"
#include "AVInput.h"
#include "AVOutput.h"
#include "Char033.h"
#include <bits/stdc++.h>
using namespace anim;
using namespace std;
using namespace chrono;

class ProductAnimation:public GroupAnimation{
	Color getPixelV(const Point&)const override{
		return Color(rand()&0xffff,rand()&0xffff,11451);
	}
};

double getTick(double realTime){
	return (realTime-4.05)/1.558551;
}
pair<vector<Frame>,AVRational> getAudio(){
	AVInput src("resource/起风了.mpga");
	return {src.read(AVMEDIA_TYPE_AUDIO),src.getTimeBase(AVMEDIA_TYPE_AUDIO)};
}

int main(){
	srand(time(0));

	double realTime=0;
	ProductAnimation* product=new ProductAnimation();	
	RectangleAnimation video(pAnim(product),1920,1080,{0,1920},{1080,0});

	auto [audio,timeBase]=getAudio();
	AVOutput out("output/out.mp4",{
		new VideoEncoder({1920,1080,AV_PIX_FMT_YUV444P},AV_CODEC_ID_H264,30,0,{{"preset","ultrafast"}}),
		new AudioEncoder(AudioFormat(AV_CHANNEL_LAYOUT_STEREO,AV_SAMPLE_FMT_FLTP,44100),AV_CODEC_ID_AAC)
	});
	AVOutput vid("output/out-%05d.png",{
		new VideoEncoder({1920,1080,AV_PIX_FMT_RGBA},AV_CODEC_ID_PNG,30,0,{{"preset","ultrafast"}})
	});
	cout<<video[0]<<endl;
	int cnt=0;
	for(auto audioIter=audio.begin();audioIter!=audio.end() && realTime<=10;realTime+=1.0/30){
		double tick=getTick(realTime);
		cerr<<"Frame Count = "<<setw(6)<<++cnt<<" curRealTime = "<<setw(6)<<realTime<<" curTick = "<<setw(6)<< tick<<endl;

		video.setTime(tick);
		out.encode(AVMEDIA_TYPE_VIDEO,{video.toFrame().toFrame()});
		vid.encode(AVMEDIA_TYPE_VIDEO,{video.toFrame().toFrame()});
		vid.flush();
		for(;audioIter!=audio.end() && audioIter->data()->pts*timeBase<=realTime;++audioIter){
			out.encode(AVMEDIA_TYPE_AUDIO,{*audioIter});
		}
		out.flush();
	}
	return 0;
}
