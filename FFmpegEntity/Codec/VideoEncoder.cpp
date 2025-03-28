#include "VideoEncoder.h"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>

namespace my_ffmpeg {

class VideoEncoder::Handler {
	std::mutex lock;
	std::condition_variable condition;
    bool stop = false;
	vector<std::thread> th;
    std::atomic<int> working = 0;

	deque<Frame> buffer;
	vector<Frame> converted;
	VideoFormat outputFormat;
public:
    Handler(VideoFormat oFormat): outputFormat(oFormat) {
        th = vector<std::thread>(std::thread::hardware_concurrency());
        for(auto& t : th) {
            t = std::thread([&]() {
                while(!stop) {
					std::unique_lock<std::mutex> locker(lock);
                    condition.wait(locker, [ = ]() {return !buffer.empty() || stop;});
                    if(stop) {
						break;
					}
                    auto cur = buffer[0];
					++working;
					buffer.pop_front();
					locker.unlock();

                    auto res = Swscale(cur, outputFormat).scale(cur);

					locker.lock();
					converted.push_back(res);
					--working;
					// std::cerr<<c033::pGreen<<"convert buffer.size()="<<buffer.size()<<c033::pNone<<"\n";
				}
			});
		}
	}
    void push(const vector<Frame>& source) {
		std::lock_guard<std::mutex> locker(lock);
        for(const auto& i : source) {
            if(i == outputFormat) {
				converted.push_back(i);
				continue;
			}
			buffer.push_back(i);
		}

		// std::cerr<<c033::pRed<<"push buffer.size()="<<buffer.size()<<c033::pNone<<"\n";
		// std::cerr<<c033::pYellow<<"converting = "<<working<<c033::pNone<<"\n";

        if(buffer.size() >= std::thread::hardware_concurrency()) {
			condition.notify_all();
        } else {
			condition.notify_one();
		}
	}
    vector<Frame> pop() {
		std::lock_guard<std::mutex> locker(lock);
		// std::cerr<<c033::pRed<<"pop(): buffer.size()="<<buffer.size()<<" converted.size()="<<converted.size()<<c033::pNone<<"\n";
        auto ret = converted;
		converted.clear();
		return ret;
	}
    vector<Frame> flush() {
		// std::cerr<<c033::pPurple<<"flushing"<<c033::pNone<<"\n";
		while(!buffer.empty() || working);
		return pop();
	}
    ~Handler() {
		// std::cerr<<"~Handler(): buffer.size()="<<buffer.size()<<" converted.size()="<<converted.size()<<"\n";
        stop = true;
		condition.notify_all();
        for(auto& t : th) {
			t.join();
		}
	}
};

double VideoEncoder::step()const {
    return 1.0 / fps;
}
vector<Frame> VideoEncoder::convertFormat(const vector<Frame>& source) {
	handler->push(source);
	return handler->pop();
}
vector<Frame> VideoEncoder::flushBuffer() {
	return handler->flush();
}
VideoEncoder::VideoEncoder(VideoFormat format, AVCodecID id, int fps_, int64_t bit_rate, const std::map<string, string>& opts): Encoder(id, bit_rate, opts), handler(new Handler(format)), fps(fps_) {

	context->pix_fmt = format.pix_fmt;
	context->width = format.width;
	context->height = format.height;

	context->gop_size = fps;   // I帧间隔
    context->max_b_frames = context->gop_size - 1; // B帧 ( I BBB P BBB P BBB P BBB P BBB I )

    context->time_base = {1, fps};
    context->framerate = {fps, 1};
}
VideoEncoder::~VideoEncoder()noexcept {
	delete handler;
}

}
