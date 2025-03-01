#include "Frame.h"

namespace my_ffmpeg{

Swscale::Swscale(VideoFormat src,VideoFormat dst):
    context(sws_getContext(src.width,src.height,src.pix_fmt,dst.width,dst.height,dst.pix_fmt,SWS_BILINEAR,nullptr,nullptr,nullptr)){}

Frame Swscale::scale(const Frame& src)const{
    Frame ret;
    sws_scale_frame(context,ret.data(),src.data());
    return ret;
}

VideoFormat::VideoFormat(const Frame& frame):VideoFormat(frame.data()->width,frame.data()->height,AVPixelFormat(frame.data()->format)){}
bool operator==(const VideoFormat& a,const VideoFormat& b){
    return a.width==b.width && a.height==b.height && a.pix_fmt==b.pix_fmt;
}
bool operator!=(const VideoFormat& a,const VideoFormat& b){
    return !(a==b);
}

bool operator==(const AudioFormat& a,const AudioFormat& b){
    return !av_channel_layout_compare(&a.channelLayout,&b.channelLayout) && a.sampleFormat==b.sampleFormat && a.sampleRate==b.sampleRate;
}
bool operator!=(const AudioFormat& a,const AudioFormat& b){
    return !(a==b);
}

}