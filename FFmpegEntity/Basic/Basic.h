#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <stdexcept>

namespace my_ffmpeg{

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
using std::string;

using ull=unsigned long long;
using ushort=unsigned short;


class FFmpegError:public std::runtime_error{
public:
	FFmpegError(const string& error):std::runtime_error(error){}
};

class FileError:public FFmpegError{
public:
	FileError(const string& error):FFmpegError(error){}
};

class CodecError:public FFmpegError{
public:
	CodecError(const string& error):FFmpegError(error){}
};

}

using std::string;
using std::vector;

#define CLONE_(clone,T,pT) \
	pT clone()const{return static_cast<pT>(new T(*this));}
#define CLONE(clone,T,pT) \
	CLONE_(clone,T,pT) \
	T& operator=(const T& o){T t(o);swap(t);return *this;} 
#define COPY(T) \
	T([[maybe_unused]]const T& o)
#define SWAP(T,...) \
	T(T&& o)noexcept:T(__VA_ARGS__){swap(o);} \
	void swap(T&& o)noexcept{swap(o);}\
	T& operator=(T&& o)noexcept{swap(o);return *this;} \
	void swap(T& o)noexcept
