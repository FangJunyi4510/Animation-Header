cmake_minimum_required(VERSION 3.10)

project(FFmpegEntity)

# 所有源文件
set(SOURCES
    Basic/Basic.cpp
    Basic/Color.cpp
    Basic/Formats.cpp
    Basic/Frame.cpp
    Basic/Packet.cpp
    Codec/Codec.cpp
    Codec/Decoder.cpp
    Codec/Encoder.cpp
    Codec/VideoEncoder.cpp
    Codec/AudioEncoder.cpp
    Format/AVFormat.cpp
    Format/AVInput.cpp
    Format/AVOutput.cpp

    Basic/Basic.h
    Basic/Color.h
    Basic/Formats.h
    Basic/Frame.h
    Basic/Packet.h
    Codec/Codec.h
    Codec/Decoder.h
    Codec/Encoder.h
    Codec/VideoEncoder.h
    Codec/AudioEncoder.h
    Format/AVFormat.h
    Format/AVInput.h
    Format/AVOutput.h
)

add_library(FFmpegEntity ${SOURCES})

target_include_directories(FFmpegEntity PUBLIC
	Format
	Codec
	Basic
	${CMAKE_SOURCE_DIR}/../../include
)

# 链接到 FFmpeg 库
target_link_libraries(FFmpegEntity PRIVATE
	/lib/x86_64-linux-gnu/libavcodec.so
	/lib/x86_64-linux-gnu/libavfilter.so
	/lib/x86_64-linux-gnu/libavutil.so
	/lib/x86_64-linux-gnu/libswscale.so
	/lib/x86_64-linux-gnu/libavdevice.so
	/lib/x86_64-linux-gnu/libavformat.so
	/lib/x86_64-linux-gnu/libswresample.so
)

# 在 CMakeLists.txt 中添加 Sanitizers
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

# 防止裁减未使用符号，似乎暂时不需要
# target_link_options(FFmpegEntity INTERFACE 
# 	"-Wl,--whole-archive"
# 	"$<TARGET_FILE:FFmpegEntity>"
# 	"-Wl,--no-whole-archive"
# )
