#include "Color.h"
#include <iostream>

std::ostream& std::operator<<(std::ostream& out,const my_ffmpeg::Color& c){
	return out<<"RGBA64("<<c.red()<<","<<c.green()<<","<<c.blue()<<","<<c.alpha()<<")";
}

namespace my_ffmpeg{

Color operator+(const Color& a,const Color& b){
	uint aalpha=a.alpha();
	uint balpha=b.alpha()*ushort(0xffff-aalpha)/0xffff;
	if(aalpha+balpha==0){
		return Color();
	}
	return Color(avg(a.red(),b.red(),aalpha,balpha),avg(a.green(),b.green(),aalpha,balpha),avg(a.blue(),b.blue(),aalpha,balpha),aalpha+balpha);
}

}
