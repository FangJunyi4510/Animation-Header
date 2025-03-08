#include <ft2build.h>
#include FT_FREETYPE_H
#include <vector>
#include <stdexcept>
#include <iostream>


// 定义像素结构 (RGBA)
using Pixel=anim::Color;

// 像素矩阵类型
using PixelMatrix = std::vector<std::vector<Pixel>>;

// 文字渲染函数
void renderTextToMatrix(
	PixelMatrix& matrix,
	const std::string& text,
	int startX,
	int startY,
	unsigned int fontSize,
	const Pixel& color,
	const char* fontPath = "/usr/share/fonts/truetype/ubuntu/UbuntuMono[wght].ttf") 
{
	static FT_Library ft;
	static bool initialized = false;
	
	// 初始化 FreeType 库
	if (!initialized) {
		if (FT_Init_FreeType(&ft))
			throw std::runtime_error("FreeType 初始化失败");
		initialized = true;
	}

	// 加载字体
	FT_Face face;
	if (FT_New_Face(ft, fontPath, 0, &face))
		throw std::runtime_error("字体加载失败");

	// 设置字体尺寸
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	int penX = startX;
	int penY = startY;

	for (char c : text) {
		// 加载字符字形
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			continue;

		FT_GlyphSlot glyph = face->glyph;

		// 计算绘制位置
		int charTop = penY - glyph->bitmap_top;
		int xOffset = penX + glyph->bitmap_left;

		// 遍历位图像素
		for (int y = 0; y < glyph->bitmap.rows; ++y) {
			int yPos = charTop + y;
			if (yPos < 0 || yPos >= matrix.size()) continue;

			for (int x = 0; x < glyph->bitmap.width; ++x) {
				int xPos = xOffset + x;
				if (xPos < 0 || xPos >= matrix[yPos].size()) continue;

				// 获取 alpha 值
				unsigned char alpha = glyph->bitmap.buffer[y * glyph->bitmap.width + x];
				
				// 混合颜色（简单 alpha 混合）
				Pixel& dest = matrix[yPos][xPos];
				dest.red = (color.red * alpha + dest.red * (255 - alpha)) ;
				dest.green = (color.green * alpha + dest.green * (255 - alpha)) ;
				dest.blue = (color.blue * alpha + dest.blue * (255 - alpha)) ;
				dest.alpha = 65535;  // 假设目标完全不透明
			}
		}

		// 移动笔触位置
		penX += glyph->advance.x >> 6;
	}

	FT_Done_Face(face);
}

int main() {
	// 创建 100x100 的像素矩阵
	PixelMatrix matrix(100, std::vector<Pixel>(100, {0, 0, 0, 255}));
	
	// 白色文字
	Pixel white = {255, 255, 255, 255};
	
	try {
		renderTextToMatrix(matrix, "Hello World!", 10, 50, 24, white);
	} catch (const std::exception& e) {
		std::cerr << "错误: " << e.what() << std::endl;
	}
	// anim::VideoFrame frame(100,100);
	// for(int i=0;i<100;++i){
	//     for(int j=0;j<100;++j){
	//         frame[i][j]=matrix[i][j];
	//     }
	// }
	// my_ffmpeg::AVOutput out("1.png",{new my_ffmpeg::VideoEncoder({100,100,AV_PIX_FMT_RGBA},AV_CODEC_ID_PNG)});
	// out.encode(AVMEDIA_TYPE_VIDEO,{frame.toFrame()});
	return 0;
}