#ifndef __ROV_CCMJPEG_H
#define __ROV_CCMJPEG_H
#include "cocos2d.h"
#include "curl/curl.h"
#include "CCMjpegStreamReceiver.h"
#include "IStreamReceiver.h"

using namespace cocos2d;

class CCStreamVideoPlayer : public Node
{
public:
	~CCStreamVideoPlayer();

	static CCStreamVideoPlayer* create(const char* url, const Color4B& color);

	bool init(const char* url,const Color4B& color);

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

	bool restart();
protected:
	CCStreamVideoPlayer();
private:
	IStreamReceiver* m_recv;
	stringc m_url;
	BlendFunc        _blendFunc;            /// It's required for TextureProtocol inheritance
	Texture2D*       _texture;              /// Texture2D object that is used to render the sprite
	CustomCommand      _customCommand;          /// quad command
	Vec3 _noMVPVertices[4];
	Vec2 _squareVertices[4];
	Vec2 _textureCoord[4];
	Color4F  _squareColors[4];  
};

#endif