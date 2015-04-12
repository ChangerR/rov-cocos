#include "CUIJoystick.h"


CUIJoystick::~CUIJoystick()
{
	CC_SAFE_RELEASE(_background);
	CC_SAFE_RELEASE(_joystick);
	CC_SAFE_RELEASE(_joy_background);
	CC_SAFE_RELEASE(_touchEventListener);
}

bool CUIJoystick::init(const char* _joy, const char* _back, bool _use_local)
{
	if (!Node::init())
		return false;
	do 
	{
		if (_use_local)
		{
			_joystick = cocos2d::Sprite::create(_joy);
			_joy_background = cocos2d::Sprite::create(_back);
		}
		else {
			_joystick = cocos2d::Sprite::createWithSpriteFrameName(_joy);
			_joy_background = cocos2d::Sprite::createWithSpriteFrameName(_back);
		}

		CC_BREAK_IF(!_joystick||!_joy_background);
		_joystick->retain();
		_joy_background->retain();

		_origin_size = _contentSize = _joy_background->getContentSize() * 2;
		setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
		this->ignoreAnchorPointForPosition(false);

		_background = cocos2d::Node::create();
		_background->setContentSize(_origin_size);
		_background->setPosition(_origin_size / 2);
		CC_SAFE_RETAIN(_background);
		addChild(_background);

		_joystick->setPosition(getContentSize() / 2);
		_joy_background->setPosition(getContentSize() / 2);

		addChild(_joy_background);
		addChild(_joystick);

		_touchEventListener = cocos2d::EventListenerTouchOneByOne::create();
		CC_BREAK_IF(!_touchEventListener);

		_touchEventListener->retain();
		_touchEventListener->onTouchBegan = CC_CALLBACK_2(CUIJoystick::onTouchBegan, this);
		_touchEventListener->onTouchMoved = CC_CALLBACK_2(CUIJoystick::onTouchMoved, this);
		_touchEventListener->onTouchEnded = CC_CALLBACK_2(CUIJoystick::onTouchEnded, this);
		_touchEventListener->onTouchCancelled = CC_CALLBACK_2(CUIJoystick::onTouchCancelled, this);

		_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchEventListener, this);
		return true;
	} while (0);
	return false;
}

CUIJoystick::CUIJoystick()
{
	_joystick = nullptr;
	_joy_background = nullptr;
	_origin_size = _joy_pos = cocos2d::Vec2::ZERO;
	_touchEventListener = nullptr;
	_handler = nullptr;
	_background = nullptr;
}

bool CUIJoystick::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event*)
{
	if (this->getBoundingBox().containsPoint(touch->getLocation()))
	{
		handleTouchChange(touch);
		return true;
	}

	return false;
}

void CUIJoystick::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event*)
{
	if (this->getBoundingBox().containsPoint(touch->getLocation()))
		handleTouchChange(touch);
}

void CUIJoystick::onTouchEnded(cocos2d::Touch*, cocos2d::Event*)
{
	_joystick->setPosition(_contentSize / 2);

	if (bool(_handler))
		_handler(cocos2d::Vec2::ZERO, cocos2d::Vec2::ZERO);

	_joy_pos = cocos2d::Vec2::ZERO;
}

void CUIJoystick::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{
	onTouchEnded(touch, event);
}

void CUIJoystick::setJoystickPositionChangeHandler(JoystickHandler handle)
{
	_handler = handle;
}

void CUIJoystick::handleTouchChange(cocos2d::Touch* touch)
{
	cocos2d::Vec2 origin = _contentSize / 2;
	float bigR = _contentSize.width / 2;
	float smallR = _contentSize.width / 8;
	cocos2d::Vec2 hit = this->_background->convertToNodeSpaceAR(touch->getLocation());
	
	if (hit.getDistance(cocos2d::Vec2::ZERO) + smallR > bigR)
	{
		float x = (bigR - smallR) / sqrt(1 + hit.y * hit.y / hit.x / hit.x);
		float y = abs(hit.y / hit.x * x);

		if (hit.x > 0)
		{
			if (hit.y > 0)
			{
				hit.x = x;
				hit.y = y;
			}
			else
			{
				hit.x = x;
				hit.y = -y;
			}
		}
		else
		{
			if (hit.y > 0)
			{
				hit.x = -x;
				hit.y = y;
			}
			else
			{
				hit.x = -x;
				hit.y = -y;
			}
		}
	}

	_joystick->setPosition(hit + origin);

	hit = hit / (bigR - smallR);

	if (bool(_handler))
		_handler(hit, _joy_pos);

	_joy_pos = hit;
}

CUIJoystick* CUIJoystick::create(const char* _joy, const char* _back, bool _use_local /*= true*/)
{
	CUIJoystick* joy = new (std::nothrow)CUIJoystick();
	if (joy&&joy->init(_joy,_back,_use_local))
	{
		joy->autorelease();
		return joy;
	}
	CC_SAFE_RELEASE(joy);
	return nullptr;
}

void CUIJoystick::setContentSize(const cocos2d::Size& contentSize)
{
	float length = contentSize.width < contentSize.height ? contentSize.width : contentSize.height;
	
	cocos2d::Node::setContentSize(cocos2d::Size(length, length));
	_background->setContentSize(_contentSize);

	_joy_background->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
	_joystick->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
	_background->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);

	_joystick->setPosition(_contentSize / 2);
	_joy_background->setPosition( _contentSize / 2);
	_background->setPosition(_contentSize / 2);

	float _scale_s = length / _origin_size.width;
	_joystick->setScale(_scale_s);
	_joy_background->setScale(_scale_s);
	
}

#ifdef __ROV_JOYSTICK_DEBUG_
void CUIJoystick::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	_custom.init(_globalZOrder);
	_custom.func = CC_CALLBACK_0(CUIJoystick::onDraw, this,transform,flags);
	renderer->addCommand(&_custom);
}

void CUIJoystick::onDraw(const cocos2d::Mat4& transform, uint32_t flags)
{
	auto director = cocos2d::Director::getInstance();
	director->pushMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	cocos2d::DrawPrimitives::setDrawColor4B(255, 255, 0, 255);
	cocos2d::DrawPrimitives::drawRect(cocos2d::Vec2::ZERO, _contentSize);
	cocos2d::DrawPrimitives::drawLine(cocos2d::Vec2::ZERO, cocos2d::Vec2(_contentSize.width, _contentSize.height));
	cocos2d::DrawPrimitives::drawLine(cocos2d::Vec2(0, _contentSize.height), cocos2d::Vec2(_contentSize.width, 0));

	director->popMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

#endif
