#include "Color.h"
#include <iostream>

std::ostream& std::operator<<(std::ostream& out,const my_ffmpeg::Color& c){
	return out<<"RGBA64("<<c.red<<","<<c.green<<","<<c.blue<<","<<c.alpha<<")";
}

namespace my_ffmpeg{

Color operator+(const Color& a,const Color& b){
	Color t=b;
	t.alpha=std::max(t.alpha,ushort(-a.alpha-1));
	return Color(avg(a.red,t.red,a.alpha,t.alpha),avg(a.green,t.green,a.alpha,t.alpha),avg(a.blue,t.blue,a.alpha,t.alpha),a.alpha+t.alpha);
}
Color operator-(const Color& a,const Color& b){
	Color t=b;
	t.alpha=std::max(t.alpha,a.alpha);
	return Color(avg(a.red,t.red,int(a.alpha),-t.alpha),avg(a.green,t.green,int(a.alpha),-t.alpha),avg(a.blue,t.blue,int(a.alpha),-t.alpha),a.alpha-t.alpha);
}


Color::Color(ushort r,ushort g,ushort b,ushort a):red(r),green(g),blue(b),alpha(a){}

Color& Color::operator+=(const Color& o){
	return *this=*this+o;
}
Color& Color::operator-=(const Color& o){
	return *this=*this-o;
}

}