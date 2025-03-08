#include "Decoder.h"

namespace my_ffmpeg{

Decoder::Decoder(AVStream* s){
    if(!s){
        return;
    }
    setStream(s);
    codec=avcodec_find_decoder(stream->codecpar->codec_id);
    context=avcodec_alloc_context3(codec);
    readStream();
    open();
}

}
