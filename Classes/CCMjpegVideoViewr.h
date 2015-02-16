#ifndef __ROV_CCMJPEG_H
#define __ROV_CCMJPEG_H
#include "cocos2d.h"
#include "curl/curl.h"
#include "CCMjpegStreamReceiver.h"
#include "ui/UIText.h"
using namespace cocos2d;

class CCMjpegVideoViewr : public Node
{
public:
	~CCMjpegVideoViewr();

	static CCMjpegVideoViewr* create(const char* url, const Color4B& color);

	bool init(const Color4B& color);

	virtual void draw(Renderer *renderer, const Mat4& transform, uint32_t flags);

	virtual void pause();

	virtual void resume();

	void setTexture(Image* img);

	void updateTexture(Image* img);

	void onDraw(const Mat4& transform, uint32_t flags);

	virtual void setContentSize(const Size & size);

	virtual void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);

	void updateColor();

	void calculateDeltaTime();

	float Get_FPS();

protected:
	CCMjpegVideoViewr();
private:
	CCMjpegStreamReceiver* m_recv;
	stringc m_url;
	BlendFunc        _blendFunc;            /// It's required for TextureProtocol inheritance
	Texture2D*       _texture;              /// Texture2D object that is used to render the sprite
	CustomCommand      _customCommand;          /// quad command
	Vec3 _noMVPVertices[4];
	Vec2 _squareVertices[4];
	Vec2 _textureCoord[4];
	Color4F  _squareColors[4];  
	float  fps = 0;
	int    frameCount = 0;
	float  currentTime = 0.0f;
	float  lastTime = 0.0f;
	ui::Text* _FPSLabel;
};

#endif