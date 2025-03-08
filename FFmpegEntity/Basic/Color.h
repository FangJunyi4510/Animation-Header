#pragma once

#include "Basic.h"

namespace my_ffmpeg{

class Color;
Color operator+(const Color& a,const Color& b);
Color operator-(const Color& a,const Color& b);

class Color{
public:
	static const ushort max=0xffff;
	ushort red=0,green=0,blue=0,alpha=0;
	Color(){}
	Color(ushort r,ushort g,ushort b,ushort a=max);
	Color& operator+=(const Color& o);
	Color& operator-=(const Color& o);
};

template<class W>
Color avg(Color a,Color b,W wa){
	return Color(avg(a.red,b.red,wa),avg(a.green,b.green,wa),avg(a.blue,b.blue,wa),avg(a.alpha,b.alpha,wa));
}

}

namespace std{
std::ostream& operator<<(std::ostream& out,const my_ffmpeg::Color& c);
}
