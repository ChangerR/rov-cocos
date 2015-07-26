#include "CCStreamVideoPlayer.h"
#include "CColorConvert.h"
#include "CRovStatus.h"
#include "CCUdpReceiver.h"

const stringc jpeg_tex = "/rov_mjpeg_x_x_1";

CCStreamVideoPlayer::CCStreamVideoPlayer()
{
	//m_recv = new CCMjpegStreamReceiver();
	//m_recv = new CCUdpReceiver();
	m_recv = nullptr;
	_texture = NULL;
}


CCStreamVideoPlayer::~CCStreamVideoPlayer()
{
	if (m_recv)
	{
		m_recv->closeReceiver();
		delete m_recv;
	}
	if (_texture)
		_texture->release();

}

CCStreamVideoPlayer* CCStreamVideoPlayer::create(const char* url,const Color4B& color)
{
	CCStreamVideoPlayer *viwer = new (std::nothrow) CCStreamVideoPlayer();
	if (viwer&&viwer->init(url,color))
	{
		viwer->m_url = url;
		viwer->autorelease();
		return viwer;
	}
	CC_SAFE_DELETE(viwer);
	return nullptr;
}

void CCStreamVideoPlayer::draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (m_recv->getRunningState() != false)
	{
		Image* img = m_recv->pop();
		if (img)
			CRovStatus::Inc_FPS();

		updateTexture(img);

		if (!_texture)
			return;

		_customCommand.init(_globalZOrder);
		_customCommand.func = CC_CALLBACK_0(CCStreamVideoPlayer::onDraw, this, transform, flags);
		renderer->addCommand(&_customCommand);

		for (int i = 0; i < 4; ++i)
		{
			Vec4 pos;
			pos.x = _squareVertices[i].x; pos.y = _squareVertices[i].y; pos.z = _positionZ;
			pos.w = 1;
			_modelViewTransform.transformVector(&pos);
			_noMVPVertices[i] = Vec3(pos.x, pos.y, pos.z) / pos.w;
		}
	}
}

void CCStreamVideoPlayer::onDraw(const Mat4& transform, uint32_t flags)
{
	getGLProgram()->use();
	getGLProgram()->setUniformsForBuiltins(transform);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture->getName());
	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, _noMVPVertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, _squareColors);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, _textureCoord);
	GL::blendFunc(_blendFunc.src, _blendFunc.dst);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 4);
}

void CCStreamVideoPlayer::pause()
{
	m_recv->closeReceiver();
	Node::pause();
}

void CCStreamVideoPlayer::resume()
{
	m_recv->openReceiver(m_url.c_str());
	Node::resume();
}

void CCStreamVideoPlayer::setTexture(Image* img)
{
	if (_texture)
	{
		CC_SAFE_RELEASE(_texture);
		Director::getInstance()->getTextureCache()->removeTextureForKey(jpeg_tex);
	}

	_texture = Director::getInstance()->getTextureCache()->addImage(img, jpeg_tex);
	CC_SAFE_RETAIN(_texture);
	Rect rect = Rect::ZERO;
	if (_texture)
		rect.size = _texture->getContentSize();
	
	CC_SAFE_DELETE(img);
}

void CCStreamVideoPlayer::updateTexture(Image* image)
{
	if (image == NULL)
		return;
	if (_texture == NULL)
		return setTexture(image);

	unsigned char* outTempData = nullptr;
	ssize_t outTempDataLen = 0;
	unsigned char*   tempData = image->getData();
	int imageWidth = image->getWidth();
	int imageHeight = image->getHeight();

	if (imageWidth != _texture->getPixelsWide() || imageHeight != _texture->getPixelsHigh())
	{
		return setTexture(image);
	}

	Texture2D::PixelFormat      pixelFormat = _texture->getPixelFormat();
	Texture2D::PixelFormat      renderFormat = image->getRenderFormat();
	size_t	         tempDataLen = image->getDataLen();

	if (pixelFormat == renderFormat)
	{
		_texture->updateWithData(tempData, 0, 0, imageWidth, imageHeight);
		CC_SAFE_DELETE(image);
		return;
	}

	pixelFormat = CColorConvert::convertDataToFormat(tempData, tempDataLen, renderFormat, pixelFormat, &outTempData, &outTempDataLen);
	_texture->updateWithData(outTempData, 0, 0, imageWidth, imageHeight);

	if (outTempData != nullptr && outTempData != tempData)
	{
		free(outTempData);
	}

	CC_SAFE_DELETE(image);
}

	

void CCStreamVideoPlayer::setContentSize(const Size & size)
{
	_squareVertices[1].x = size.width;
	_squareVertices[2].y = size.height;
	_squareVertices[3].x = size.width;
	_squareVertices[3].y = size.height;

	Node::setContentSize(size);
}

bool CCStreamVideoPlayer::init(const char* url,const Color4B& color)
{
	// default blend function
	_blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
		
	_displayedColor.r = _realColor.r = color.r;
	_displayedColor.g = _realColor.g = color.g;
	_displayedColor.b = _realColor.b = color.b;
	_displayedOpacity = _realOpacity = color.a;

	for (size_t i = 0; i < sizeof(_squareVertices) / sizeof(_squareVertices[0]); i++)
	{
		_squareVertices[i].x = 0.0f;
		_squareVertices[i].y = 0.0f;
	}

	updateColor();

	setContentSize(Director::getInstance()->getVisibleSize());

	_textureCoord[0].x = 0.f; _textureCoord[0].y = 1.f;
	_textureCoord[1].x = 1.f; _textureCoord[1].y = 1.f;
	_textureCoord[2].x = 0.f; _textureCoord[2].y = 0.f;
	_textureCoord[3].x = 1.f; _textureCoord[3].y = 0.f;

	setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));

	if (!strncmp("http://", url, 7)) {
		m_recv = new CCMjpegStreamReceiver;
	}
	else if (!strncmp("udp://", url, 6)) {
		m_recv = new CCUdpReceiver;
	}

	if (m_recv&&m_recv->openReceiver(url))
		return true;

	return false;
}

void CCStreamVideoPlayer::updateColor()
{
	for (unsigned int i = 0; i < 4; i++)
	{
		_squareColors[i].r = _displayedColor.r / 255.0f;
		_squareColors[i].g = _displayedColor.g / 255.0f;
		_squareColors[i].b = _displayedColor.b / 255.0f;
		_squareColors[i].a = _displayedOpacity / 255.0f;
	}
	/*_squareColors[0].r = 1.0f; _squareColors[0].g = 1.0f; _squareColors[0].b = 1.0f; _squareColors[0].a = 1.0f;
	_squareColors[1].r = 1.0f; _squareColors[1].g = 0.0f; _squareColors[1].b = 0.0f; _squareColors[1].a = 1.0f;
	_squareColors[2].r = .0f; _squareColors[2].g = 1.0f; _squareColors[2].b = .0f; _squareColors[2].a = 1.0f;
	_squareColors[3].r = 1.0f; _squareColors[3].g = .0f; _squareColors[3].b = 1.0f; _squareColors[3].a = 1.0f;*/
}


void CCStreamVideoPlayer::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	Node::visit(renderer, parentTransform, parentFlags);	
}

bool CCStreamVideoPlayer::restart()
{
	if (m_recv) {
		m_recv->closeReceiver();
		return m_recv->openReceiver(m_url.c_str());
	}

	return false;
}
