#pragma once

#include "Basic.h"

namespace my_ffmpeg{

class Color;
Color operator+(const Color& a,const Color& b);

class Color{
public:
	static const ushort max=0xffff;
	ull rgba=0x000000000000ffff;
	Color(ull rgba_=0x000000000000ffff):rgba(rgba_){}
	Color(ushort r,ushort g,ushort b,ushort a=max):rgba((ull(r)<<48)|(ull(g)<<32)|(ull(b)<<16)|a){}
	Color& operator+=(const Color& o) {return *this=*this+o;}
	ushort red()const	{return (rgba>>48)&0xffff;}
	ushort green()const	{return (rgba>>32)&0xffff;}
	ushort blue()const	{return (rgba>>16)&0xffff;}
	ushort alpha()const	{return rgba&0xffff;}
};

template<class W>
Color avg(Color a,Color b,W wa){
	return Color(avg(a.rgba,b.rgba,wa));
}

}

namespace std{
std::ostream& operator<<(std::ostream& out,const my_ffmpeg::Color& c);
}
