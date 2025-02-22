#pragma once

#include "ffmpegEntity/Format.h"
#include "animBasic.h"
#include <set>
#include <cassert>
 
namespace anim{ 

using namespace my_ffmpeg;

class AbstractImage{
	mutable bool isLoaded=false;
public:
	AbstractImage(){}
	SWAP(AbstractImage){
		std::swap(isLoaded,o.isLoaded);
	}
	COPY(AbstractImage):isLoaded(false){};

	Color operator[](Point pos)const{
		load();
		return getPixelV(pos);
	}
	pImage clone()const{
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

	virtual ~AbstractImage()noexcept{}

protected:
	void load()const{
		if(isLoaded){
			return;
		}
		loadV();
		isLoaded=true;
	}
	virtual pImage cloneV()const=0;
	virtual Color getPixelV(Point pos)const=0;
	virtual void loadV()const=0;
	virtual void unloadV()const=0;
};

class MappedImage:public AbstractImage{
	pImage m_source;
public:
	MappedImage(pImage source=nullptr):m_source(source){}
	SWAP(MappedImage){
		AbstractImage::swap(o);
		std::swap(m_source,o.m_source);
	}
	COPY(MappedImage):AbstractImage(o),m_source(o.m_source->clone()){}

	pImage source()const{
		return m_source;
	}
	void setSource(const pImage& newSource){
		m_source=newSource;
	}
protected:
	void loadV()const override{}
	void unloadV()const override{
		if(!m_source){
			return;
		}
		m_source->unload();
	}
};

class MovedImage:public MappedImage{
	Vector m_offset;
public:
	MovedImage(pImage source=nullptr, Vector offset=Vector()):MappedImage(source), m_offset(offset){}
	SWAP(MovedImage){
		MappedImage::swap(o);
		std::swap(m_offset,o.m_offset);
	}
	COPY(MovedImage)=default;
	CLONE(cloneV,MovedImage,pImage)

	Vector offset()const{
		return m_offset;
	}
	void setOffset(const Vector& newOffset){
		m_offset=newOffset;
	}
protected:
	Color getPixelV(Point p)const override{
		return (*source())[p-offset()];
	}
};

class RotatedImage: public MappedImage{
	Vector m_rotate;
public:
	RotatedImage(pImage source=nullptr,Vector rotate=Vector(1)):MappedImage(source),m_rotate(rotate){}
	SWAP(RotatedImage){
		MappedImage::swap(o);
		std::swap(m_rotate,o.m_rotate);
	}
	COPY(RotatedImage)=default;
	CLONE(cloneV,RotatedImage,pImage)

	Vector rotate()const{
		return m_rotate;
	}
	void setRotate(Vector newRotate){
		m_rotate=newRotate;
	}
protected:
	Color getPixelV(Point p)const override{
		return (*source())[p/rotate()];
	}
};

class GroupImage: public AbstractImage{
	struct SingleImage{
		int z_index;
		pImage source;
		Vector offset,rotate;
		SingleImage(int z,pImage s,Vector off,Vector rot):z_index(z),source(s),offset(off),rotate(rot){}
		pImage toImage(){
			return pImage(new MovedImage(pImage(new RotatedImage(source,rotate)),offset));
		}
	};
	
	std::set<pImage> innerImages;
public:
	GroupImage(){}
	SWAP(GroupImage){
		AbstractImage::swap(o);
		std::swap(innerImages,o.innerImages);
	}
	COPY(GroupImage){
		for(const auto& each:o.innerImages){
			innerImages.insert(each->clone());
		}
	}
	CLONE(cloneV,GroupImage,pImage)
	// to do: add remove move rotate

protected:
	Color getPixelV(Point p)const override{
		Color ret;
		for(auto each:innerImages){
			if(ret.alpha==ull(-1)){
				break;
			}
			ret+=(*each)[p];
		}
		return ret;
	}
	void loadV()const override{}
	void unloadV()const override{
		for(auto& each:innerImages){
			each->unload();
		}
	}
};

// class BufferImage:public AbstractImage{
// protected:
// 	mutable RGBABuffer buf;
// public:
// 	BufferImage(){}
// 	BufferImage(const Frame& data):buf(data.toRGBA()){}
// 	SWAP(BufferImage){
// 		AbstractImage::swap(o);
// 		std::swap(buf,o.buf);
// 	}
// 	COPY(BufferImage):buf(){}
	
// 	int width()const{
// 		load();
// 		if(buf.empty()){
// 			return 0;
// 		}
// 		return buf[0].size();
// 	}
// 	int height()const{
// 		load();
// 		return buf.size();
// 	}
// protected:
// 	Color getPixelV(Point p)const override{
// 		double px=p.real(),py=height()-p.imag();
// 		px=lim(px,0.0,width()-1.0);
// 		py=lim(py,0.0,height()-1.0);
// 		int rx=floor(px),ry=floor(py);
// 		int wx=px-rx,wy=py-ry;
// 		int rxright=lim(rx+1,0,width()-1),rybottom=lim(ry+1,0,height()-1);
// 		Color t=buf[ry][rx];
// 		Color tr=buf[rybottom][rx];
// 		Color tb=buf[ry][rxright];
// 		Color trb=buf[rybottom][rxright];
// 		return t;
// 		return avg(avg(t,tr,wy),avg(tb,trb,wy),wx);
// 	}
// 	void unloadV()const override{
// 		buf.clear();
// 	}
// };

// class UrlImage:public BufferImage{
// 	string m_url;
// public:
// 	UrlImage(string url=""):m_url(url){}
// 	SWAP(UrlImage){
// 		BufferImage::swap(o);
// 		std::swap(m_url,o.m_url);
// 	}
// 	COPY(UrlImage):m_url(o.m_url){}
// 	CLONE(cloneV,UrlImage,pImage)
// protected:
// 	void loadV()const override{
// 		FormatInput src(m_url);
// 		Frame firstFrame;
// 		for(auto& pkt:src.packets()){
// 			if(src.getType(pkt)==AVMEDIA_TYPE_VIDEO){
// 				auto tmp=src.decode({pkt});
// 				if(tmp.empty()){
// 					continue;
// 				}
// 				firstFrame=tmp[0];
// 				break;
// 			}
// 		}
// 		buf=firstFrame.toRGBA();
// 	}
// };

// class FrameImage:public BufferImage{
// 	Frame m_data;
// public:
// 	FrameImage():m_data(){}
// 	FrameImage(const Frame& data):m_data(data){}
// 	SWAP(FrameImage){
// 		BufferImage::swap(o);
// 		m_data.swap(o.m_data);
// 	}
// 	COPY(FrameImage):m_data(o.m_data){}
// 	CLONE(cloneV,FrameImage,pImage)

// protected:
// 	void loadV()const override{
// 		buf=m_data.toRGBA();
// 	}
// };

// class PacketImage:public BufferImage{
// 	Packet m_data;
// 	FormatInput* m_file;
// 	uint index=0;
// public:
// 	PacketImage():m_data(){}
// 	PacketImage(const Packet& data,FormatInput& file,uint idx=0):m_data(data),m_file(&file),index(idx){}
// 	SWAP(PacketImage){
// 		BufferImage::swap(o);
// 		m_data.swap(o.m_data);
// 		std::swap(m_file,o.m_file);
// 		std::swap(index,o.index);
// 	}
// 	COPY(PacketImage):m_data(o.m_data),m_file(o.m_file),index(o.index){}
// 	CLONE(cloneV,PacketImage,pImage)

// protected:
// 	void loadV()const override{
// 		auto frames=m_file->decode({m_data});
// 		const Frame* target=nullptr;
// 		if(index>=frames.size()){
// 			target=&frames.back();
// 		}else{
// 			target=&frames[index];
// 		}
// 		buf=target->toRGBA();
// 	}
// };


}