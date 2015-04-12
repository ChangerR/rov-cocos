#include "CUIScrollBar.h"


CUIScrollBar::CUIScrollBar()
{
	_handler = nullptr;
	_background = nullptr;
	_button = nullptr;
	_touch_sel = false;
}


CUIScrollBar::~CUIScrollBar()
{
	CC_SAFE_RELEASE(_background);
	CC_SAFE_RELEASE(_button);
	CC_SAFE_RELEASE(_touchEventListener);
}

bool CUIScrollBar::init(const char* button, const char* background)
{
	do 
	{
		CC_BREAK_IF(!Node::init());

		_button = Sprite::create(button);
		CC_BREAK_IF(!_button);

		_background = Sprite::create(background);
		CC_BREAK_IF(!_background);

		_button->retain();
		_background->retain();

		float width, height;
		width = _button->getContentSize().width > _background->getContentSize().width ? _button->getContentSize().width : _background->getContentSize().width;
		height = _background->getContentSize().height;

		Node::setContentSize(Size(width, height));

		_button->setPosition(Vec2(width / 2, height / 2));
		_background->setPosition(Vec2(width / 2, height / 2));

		addChild(_background);
		addChild(_button);

		_touchEventListener = EventListenerTouchOneByOne::create();
		_touchEventListener->retain();

		_touchEventListener->onTouchBegan = CC_CALLBACK_2(CUIScrollBar::onTouchBegan, this);
		_touchEventListener->onTouchMoved = CC_CALLBACK_2(CUIScrollBar::onTouchMoved, this);
		_touchEventListener->onTouchEnded = CC_CALLBACK_2(CUIScrollBar::onTouchEnded, this);
		_touchEventListener->onTouchCancelled = CC_CALLBACK_2(CUIScrollBar::onTouchCancelled, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchEventListener, this);

		return true;
	} while (0);

	return false;
}

CUIScrollBar* CUIScrollBar::create(const char* button, const char* background)
{
	CUIScrollBar* bar = new(std::nothrow) CUIScrollBar();
	if (bar&&bar->init(button,background))
	{
		bar->autorelease();
		return bar;
	}
	return nullptr;
}

bool CUIScrollBar::onTouchBegan(cocos2d::Touch* _touch, cocos2d::Event*)
{
	if (this->getBoundingBox().containsPoint(_touch->getLocation())&&_button->getBoundingBox().containsPoint(this->convertToNodeSpace(_touch->getLocation())))
	{
		_touch_sel = true;
		return true;
	}

	return false;
}

void CUIScrollBar::onTouchMoved(cocos2d::Touch* _touch, cocos2d::Event*)
{
	
	if (_touch_sel)
	{
		Vec2 hit = _background->convertToNodeSpace(_touch->getLocation());
		hit.y = hit.y > 0 ? hit.y : 0;
		hit.y = hit.y > _background->getContentSize().height ? _background->getContentSize().height : hit.y;

		_button->setPosition(Vec2(_contentSize.width / 2, hit.y));
		_pos = hit.y;	
	}
}

void CUIScrollBar::onTouchEnded(cocos2d::Touch*, cocos2d::Event*)
{
	static float pos = 0.5f * _background->getContentSize().height;
	if (_touch_sel)
	{
		if (_pos != pos)
		{
			pos = _pos;
			if (bool(_handler))
				_handler(pos / _background->getContentSize().height);
		}
		_touch_sel = false;
	}
}

void CUIScrollBar::onTouchCancelled(cocos2d::Touch*, cocos2d::Event*)
{
	_touch_sel = false;
}

void CUIScrollBar::setScrollBarEvent(ScollBarHandler handler)
{
	_handler = handler;
}
