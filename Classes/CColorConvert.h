#ifndef __CCOLORCONVERT__ROV__
#define __CCOLORCONVERT__ROV__
#include "renderer/CCTexture2D.h"

class CColorConvert
{
public:
	CColorConvert(){}
	~CColorConvert(){}

	/**convert functions*/

	/**
	Convert the format to the format param you specified, if the format is PixelFormat::Automatic, it will detect it automatically and convert to the closest format for you.
	It will return the converted format to you. if the outData != data, you must delete it manually.
	*/
	static cocos2d::Texture2D::PixelFormat convertDataToFormat(const unsigned char* data, ssize_t dataLen, cocos2d::Texture2D::PixelFormat originFormat, cocos2d::Texture2D::PixelFormat format, unsigned char** outData, ssize_t* outDataLen);

	static cocos2d::Texture2D::PixelFormat convertI8ToFormat(const unsigned char* data, ssize_t dataLen, cocos2d::Texture2D::PixelFormat format, unsigned char** outData, ssize_t* outDataLen);
	static cocos2d::Texture2D::PixelFormat convertAI88ToFormat(const unsigned char* data, ssize_t dataLen, cocos2d::Texture2D::PixelFormat format, unsigned char** outData, ssize_t* outDataLen);
	static cocos2d::Texture2D::PixelFormat convertRGB888ToFormat(const unsigned char* data, ssize_t dataLen, cocos2d::Texture2D::PixelFormat format, unsigned char** outData, ssize_t* outDataLen);
	static cocos2d::Texture2D::PixelFormat convertRGBA8888ToFormat(const unsigned char* data, ssize_t dataLen, cocos2d::Texture2D::PixelFormat format, unsigned char** outData, ssize_t* outDataLen);

	//I8 to XXX
	static void convertI8ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertI8ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertI8ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertI8ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertI8ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertI8ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

	//AI88 to XXX
	static void convertAI88ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

	//RGB888 to XXX
	static void convertRGB888ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGB888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGB888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGB888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGB888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGB888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

	//RGBA8888 to XXX
	static void convertRGBA8888ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
};

#endif