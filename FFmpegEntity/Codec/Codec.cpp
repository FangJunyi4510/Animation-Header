#include "Codec.h"

namespace my_ffmpeg{

void set(AVDictionary*& options,const std::map<string,string>& opts){
	for(const auto& [key,value]:opts){
		av_dict_set(&options,key.c_str(),value.c_str(),0);
	}
}

double operator*(int64_t tick,AVRational timeBase){
	return tick*1.0*timeBase.num/timeBase.den;
}
double operator*(AVRational timeBase,int64_t tick){
	return tick*1.0*timeBase.num/timeBase.den;
}
int64_t operator/(double time,AVRational timeBase){
    return floor(time*timeBase.den/timeBase.num+0.5);
}

}