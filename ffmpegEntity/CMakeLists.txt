cmake_minimum_required(VERSION 3.10)

project(FFmpegEntity)

# 所有源文件
set(SOURCES
	Format/Format.cpp
	Codec/Codec.cpp
	Frame/Frame.cpp
	Basic/Color.cpp
)

add_library(FFmpegEntity ${SOURCES})

target_include_directories(FFmpegEntity PUBLIC
	Format
	Codec
	Frame
	Basic
	${CMAKE_SOURCE_DIR}/../../include
)

# 链接到 FFmpeg 库
target_link_libraries(FFmpegEntity 
	/lib/x86_64-linux-gnu/libavcodec.so
	/lib/x86_64-linux-gnu/libavfilter.so
	/lib/x86_64-linux-gnu/libavutil.so
	/lib/x86_64-linux-gnu/libswscale.so
	/lib/x86_64-linux-gnu/libavdevice.so
	/lib/x86_64-linux-gnu/libavformat.so
	/lib/x86_64-linux-gnu/libswresample.so
)
