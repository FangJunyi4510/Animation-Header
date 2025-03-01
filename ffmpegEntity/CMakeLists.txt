cmake_minimum_required(VERSION 3.10)

project(FFmpegEntity)

# 所有源文件
set(SOURCES
	./Format.cpp
	./Codec.cpp
	./Frame.cpp
	./Color.cpp
)

add_compile_options(-std=c++17 -Wall -Wextra)

add_library(FFmpegEntity ${SOURCES})

link_directories()

set(FFmpegLibraries
	/lib/x86_64-linux-gnu/libavcodec.so
	/lib/x86_64-linux-gnu/libavfilter.so
	/lib/x86_64-linux-gnu/libavutil.so
	/lib/x86_64-linux-gnu/libswscale.so
	/lib/x86_64-linux-gnu/libavdevice.so
	/lib/x86_64-linux-gnu/libavformat.so
	/lib/x86_64-linux-gnu/libswresample.so
)

# 链接到 FFmpeg 库
target_link_libraries(FFmpegEntity ${FFmpegLibraries})