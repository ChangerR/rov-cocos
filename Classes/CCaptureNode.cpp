#include "CCaptureNode.h"
#include "stringc.h"

CCaptureNode::CCaptureNode()
{
}


CCaptureNode::~CCaptureNode()
{
	CC_SAFE_DELETE(_buffer);
}

void CCaptureNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(CCaptureNode::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}

void CCaptureNode::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{
	do 
	{
		CC_BREAK_IF(!_capture);

		unsigned char * _data = _buffer->getData();

		glPixelStorei(GL_PACK_ALIGNMENT, 4);
		glFinish();
		glReadPixels(0, 0, _buffer->getWidth(),
			_buffer->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, _data);
		int pitch = _buffer->getWidth() * 4;
		unsigned char * buf = new unsigned char[pitch];
		for (int i = 0; i < _buffer->getHeight() / 2; i++)
		{
			memcpy(buf, _data + i * pitch, pitch);
			memcpy(_data + i * pitch, _data + (_buffer->getHeight() - 1 - i)*pitch, pitch);
			memcpy(_data + (_buffer->getHeight() - 1 - i)*pitch, buf, pitch);
		}
		delete buf;
	
		stringc filename;
		formatRandomString(filename);
		filename += ".png";
		_buffer->saveToFile(filename.c_str());
		_capture = false;
	} while (0);
}

bool CCaptureNode::init()
{
	if (!Node::init())
	{
		return false;
	}
	setContentSize(cocos2d::Director::getInstance()->getVisibleSize());
	setAnchorPoint(cocos2d::Vec2::ZERO);
	int width = _contentSize.width;
	int height = _contentSize.height;

	char* buf = new char[width * height * 4];
	memset(buf, 0, width*height * 4);
	_buffer = new cocos2d::Image;
	_buffer->initWithRawData((unsigned char*)buf, width*height * 4, width, height, 4);
	delete[] buf;
	_capture = false;
	return true;
}

void CCaptureNode::capture()
{
	_capture = true;
}

CCaptureNode* CCaptureNode::create()
{
	CCaptureNode* bar = new(std::nothrow) CCaptureNode();
	if (bar&&bar->init())
	{
		bar->autorelease();
		return bar;
	}
	return nullptr;
}
