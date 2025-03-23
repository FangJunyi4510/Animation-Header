#pragma once

#include "AVFormat.h"
#include "Basic.h"
#include <set>
#include <cassert>
#include <iostream> 

namespace anim{ 

using namespace my_ffmpeg;

class AbstractAnimation{
	double m_time=0;
	mutable bool isLoaded=false;
public:
	AbstractAnimation(){}
	SWAP(AbstractAnimation){
		std::swap(isLoaded,o.isLoaded);
		std::swap(m_time,o.m_time);
	}
	COPY(AbstractAnimation):m_time(o.m_time),isLoaded(false){};

	Color operator[](const Point& pos)const{
		load();
		return getPixelV(pos);
	}
	double time()const{
		return m_time;
	}
	void setTime(double newTime){
		unload();
		m_time=newTime;
	}
	pAnim clone()const{
		auto ret=cloneV();
		assert(typeid(*ret)==typeid(*this) && "子类未重写 cloneV()");
		return ret;
	}
	void unload()const{
		if(!isLoaded){
			return;
		}
		unloadV();
		isLoaded=false;
	}

	virtual ~AbstractAnimation()noexcept{}

protected:
	void load()const{
		if(isLoaded){
			return;
		}
		loadV();
		isLoaded=true;
	}
	virtual pAnim cloneV()const=0;
	virtual Color getPixelV(const Point& pos)const=0;
	virtual void loadV()const=0;
	virtual void unloadV()const=0;
};

class MappedAnimation:virtual public AbstractAnimation{
	pAnim m_source;
public:
	MappedAnimation(const pAnim& source=nullptr):m_source(source){}
	SWAP(MappedAnimation){
		AbstractAnimation::swap(o);
		std::swap(m_source,o.m_source);
	}
	COPY(MappedAnimation):AbstractAnimation(o),m_source(o.m_source->clone()){}

	const pAnim& source()const{
		return m_source;
	}
	void setSource(const pAnim& newSource){
		m_source=newSource;
	}
protected:
	void loadV()const override{
		if(!source()){
			return;
		}
		source()->setTime(time());
	}
	void unloadV()const override{
		if(!source()){
			return;
		}
		source()->unload();
	}
};

class MovedAnimation:public MappedAnimation{
	Vector m_offset;
public:
	MovedAnimation(const pAnim& source, const Vector& offset=Vector()):MappedAnimation(source), m_offset(offset){}
	SWAP(MovedAnimation,nullptr){
		MappedAnimation::swap(o);
		std::swap(m_offset,o.m_offset);
	}
	COPY(MovedAnimation)=default;
	CLONE(cloneV,MovedAnimation,pAnim)

	const Vector& offset()const{
		return m_offset;
	}
	void setOffset(const Vector& newOffset){
		m_offset=newOffset;
	}
protected:
	Color getPixelV(const Point& p)const override{
		return (*source())[p-offset()];
	}
};

class RotatedAnimation: public MappedAnimation{
	Vector m_rotate;
public:
	RotatedAnimation(const pAnim& source,const Vector& rotate=Vector(1)):MappedAnimation(source),m_rotate(rotate){}
	SWAP(RotatedAnimation,nullptr){
		MappedAnimation::swap(o);
		std::swap(m_rotate,o.m_rotate);
	}
	COPY(RotatedAnimation)=default;
	CLONE(cloneV,RotatedAnimation,pAnim)

	const Vector& rotate()const{
		return m_rotate;
	}
	void setRotate(const Vector& newRotate){
		m_rotate=newRotate;
	}
protected:
	Color getPixelV(const Point& p)const override{
		return (*source())[p/rotate()];
	}
};

class GroupAnimation: public AbstractAnimation,public std::set<pAnim>{
public:
	GroupAnimation(){}
	GroupAnimation(const std::set<pAnim>& srcs):std::set<pAnim>(srcs){}
	SWAP(GroupAnimation){
		AbstractAnimation::swap(o);
		std::set<pAnim>::swap(o);
	}
	COPY(GroupAnimation):AbstractAnimation(),std::set<pAnim>(){
		for(const auto& each:o){
			insert(each->clone());
		}
	}
	CLONE(cloneV,GroupAnimation,pAnim)

protected:
	Color getPixelV(const Point& p)const override{
		Color ret;
		for(auto each:*this){
			if(ret.alpha()==Color::max){
				break;
			}
			ret+=(*each)[p];
		}
		return ret;
	}
	void loadV()const override{
		for(auto& each:*this){
			each->setTime(time());
		}
	}
	void unloadV()const override{
		for(const auto& each:*this){
			each->unload();
		}
	}
};

class BufferImage:virtual public AbstractAnimation{
protected:
	mutable VideoFrame buf;
public:
	BufferImage(){}
	BufferImage(const Frame& data):buf(data){}
	SWAP(BufferImage){
		AbstractAnimation::swap(o);
		std::swap(buf,o.buf);
	}
	COPY(BufferImage):AbstractAnimation(),buf(0,0){}
	
	int width()const{
		load();
		return buf.width();
	}
	int height()const{
		load();
		return buf.height();
	}
	VideoFrame toFrame()const{
		load();
		// std::cerr<<"bufferImage toFrame() ==> loaded"<<std::endl;
		// std::cerr<<"toFrame return\n";
		return buf;
	}
protected:
	Color getPixelV(const Point& p)const override{
		double px=height()-p.imag(),py=p.real();
		if(px<0 || px>height() || py<0 || py>width()){
			return Color();
		}
		px=lim(px,0.0,height()-1.0);
		py=lim(py,0.0,width()-1.0);
		int rx=floor(px),ry=floor(py);
		double wx=px-rx,wy=py-ry;
		int rxbottom=lim(rx+1,0,height()-1),ryright=lim(ry+1,0,width()-1);

		/*
		 -y-------
		| t1    t2
		x    p
		|
		| t3    t4
		
		*/
		Color t1=buf[rx][ry];
		Color t2=buf[rx][ryright];
		Color t3=buf[rxbottom][ry];
		Color t4=buf[rxbottom][ryright];
		return avg(avg(t1,t2,wy),avg(t3,t4,wy),wx);
	}
	void unloadV()const override{
		// std::cerr<<"bufferImage unload"<<std::endl;
		buf.clear();
	}
};

class FrameImage:public BufferImage{
	Frame m_data;
public:
	FrameImage():m_data(){}
	FrameImage(const Frame& data):m_data(data){}
	SWAP(FrameImage){
		BufferImage::swap(o);
		m_data.swap(o.m_data);
	}
	COPY(FrameImage)=default;
	CLONE(cloneV,FrameImage,pAnim)

protected:
	void loadV()const override{
		buf=m_data;
	}
};

class RectangleAnimation:public BufferImage,public MappedAnimation{
	int width=0;
	int height=0;
	Point begin,end;
public:
	RectangleAnimation(const pAnim& source,int w,int h,const Point& begin_,const Point& end_):MappedAnimation(source),width(w),height(h),begin(begin_),end(end_){}
	SWAP(RectangleAnimation,nullptr,0,0,Point(),Point()){
		BufferImage::swap(o);
		MappedAnimation::swap(o);
		std::swap(width,o.width);
		std::swap(height,o.height);
		std::swap(begin,o.begin);
		std::swap(end,o.end);
	}
	COPY(RectangleAnimation)=default;
	CLONE(cloneV,RectangleAnimation,pAnim)

protected:
	void loadV()const override{
		// std::cerr<<"MappedAnimation::loadV() be\n";
		MappedAnimation::loadV();
		// std::cerr<<"MappedAnimation::loadV()\n";

		if(buf.empty()){
			buf={width,height};
		}
		Vector step=end-begin;
		step={step.real()/width,step.imag()/height};
		// std::cerr<<"loading source="<<source()<<"\n";
		for(int i=0;i<height;++i){
			for(int j=0;j<width;++j){
				buf[i][j]=(*source())[begin+Vector(j*step.real(),i*step.imag())];
			}
		}
		// std::cerr<<"loading source return\n";
	}
	void unloadV()const override{
		BufferImage::unloadV();
		MappedAnimation::unloadV();
	}
};

// class TextImage:public {

// };

}