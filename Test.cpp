#include "Image.h"
#include "../../include/Char033.h"
#include <bits/stdc++.h>
using namespace anim;
using namespace std;
using namespace chrono;

class ProductAnimation:public GroupAnimation{

};

double getTick(double realTime){
	return (realTime-4.05)/1.558551;
}
pair<vector<Frame>,AVRational> getAudio(){
	FormatInput src("src/起风了.mpga");
	return {src.read(AVMEDIA_TYPE_AUDIO),src.getTimeBase(AVMEDIA_TYPE_AUDIO)};
}

int main(){
	double realTime=0;
	ProductAnimation* product=new ProductAnimation();
	RectangleAnimation video(pAnim(product),1920,1080,{0,1920},{1080,0});

	auto [audio,timeBase]=getAudio();
	FormatOutput out("out/out.mp4",{
		new VideoEncoder({1920,1080,AV_PIX_FMT_YUV420P},AV_CODEC_ID_H264,30,0,{{"preset","ultrafast"}}),
		new AudioEncoder(AudioFormat(AV_CHANNEL_LAYOUT_STEREO,AV_SAMPLE_FMT_FLTP,44100),AV_CODEC_ID_AAC)
	});
	int cnt=0;
	for(auto audioIter=audio.begin();audioIter!=audio.end() && realTime<=10;realTime+=1.0/30){
		double tick=getTick(realTime);
		cout<<"Frame Count = "<<++cnt<<setw(6)<<" curRealTime = "<<setw(6)<<realTime<<" curTick = "<< tick<<endl;

		video.setTime(tick);
		// std::cerr<<"video.setTime(tick)\n";
		out.encode(AVMEDIA_TYPE_VIDEO,{video.toFrame().toFrame()});
		for(;audioIter!=audio.end() && audioIter->data()->pts*timeBase<=realTime;++audioIter){
			out.encode(AVMEDIA_TYPE_AUDIO,{*audioIter});
			// cerr<<"iter time="<<audioIter->data()->pts*timeBase<<"\n";
		}
		out.flush();
	}
	return 0;
}
