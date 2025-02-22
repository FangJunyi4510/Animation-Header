#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

namespace anim{

template<class T>
T lim(const T& a,const T& min,const T& max){
	if(max<a){
		return max;
	}
	if(a<min){
		return min;
	}
	return a;
}
template<class T,class W>
T avg(const T& a,const T& b,const W& wa,const W& wb){
	return (a*wa+b*wb)/(wa+wb);
}
template<class T,class W>
T avg(const T& a,const T& b,const W& wa){
	return avg(a,b,wa,1-wa);
}
using std::max;

using ull=unsigned long long;
using ushort=unsigned short;
class Color{
public:
	ushort red=0,green=0,blue=0,alpha=0;
	Color(){}
	Color(ushort r,ushort g,ushort b,ushort a=0xffff):
		red(r),green(g),blue(b),alpha(a){}
	Color operator+(Color o)const{
		o.alpha=max(o.alpha,ushort(-alpha-1));
		return Color(avg(red,o.red,alpha,o.alpha),avg(green,o.green,alpha,o.alpha),avg(blue,o.blue,alpha,o.alpha),alpha+o.alpha);
	}
	Color operator-(Color o)const{
		o.alpha=max(o.alpha,alpha);
		return Color(avg(red,o.red,alpha,ushort(-o.alpha)),avg(green,o.green,alpha,ushort(-o.alpha)),avg(blue,o.blue,alpha,ushort(-o.alpha)),alpha-o.alpha);
	}
	Color& operator+=(const Color& o){
		return *this=*this+o;
	}
	Color& operator-=(const Color& o){
		return *this=*this-o;
	}
};

template<class W>
Color avg(Color a,Color b,W wa){
	return Color(avg(a.red,b.red,wa),avg(a.green,b.green,wa),avg(a.blue,b.blue,wa),avg(a.alpha,b.alpha,wa));
}

}

namespace std{
std::ostream& operator<<(std::ostream& out,const anim::Color& c);
}

using std::string;
using std::vector;

#define CLONE(clone,T,pT) \
	pT clone()const{return static_cast<pT>(new T(*this));} \
	T& operator=(T o)noexcept{swap(o);return *this;}
#define COPY(T) \
	T(const T& o)
#define SWAP(T,...) \
	T(T&& o)noexcept:T(__VA_ARGS__){swap(o);} \
	void swap(T&& o)noexcept{swap(o);}\
	void swap(T& o)noexcept
