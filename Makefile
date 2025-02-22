CXXFLAGS += -Wall -O0 -Werror -std=c++20 -g

oFile = Image.o ffmpegEntity/Format.o ffmpegEntity/Codec.o ffmpegEntity/Frame.o ffmpegEntity/Color.o animBasic.o

Test: $(oFile) ../../include/Char033.h /lib/x86_64-linux-gnu/libav*.so /lib/x86_64-linux-gnu/libswscale.so /lib/x86_64-linux-gnu/libswresample.so

Image.o: Image.h ffmpegEntity/Format.o animBasic.o

ffmpegEntity/Format.o: ffmpegEntity/Format.h ffmpegEntity/Codec.o

ffmpegEntity/Codec.o: ffmpegEntity/Codec.h ffmpegEntity/Frame.o

ffmpegEntity/Frame.o: ffmpegEntity/Frame.h ffmpegEntity/Color.o

ffmpegEntity/Color.o: ffmpegEntity/Color.h

animBasic.o: animBasic.h

clean:
	-rm *.o Test ffmpegEntity/*.o -f