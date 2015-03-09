#ifndef __CUISCOLLBAR__H
#define __CUISCOLLBAR__H
#include "cocos2d.h"

USING_NS_CC;

class CUIScrollBar :
	public cocos2d::Node
{
public:
	typedef std::function<void(float)> ScollBarHandler;

	virtual ~CUIScrollBar();

	bool init(const char* button,const char* background);

	void setScrollBarEvent(ScollBarHandler handler);

	static CUIScrollBar* create(const char* button, const char* background);

private:
	CUIScrollBar();

	ScollBarHandler _handler;
	Sprite* _background;
	Sprite* _button;

	EventListenerTouchOneByOne* _touchEventListener;

	bool onTouchBegan(Touch*,Event*);
	void onTouchMoved(Touch*,Event*);
	void onTouchEnded(Touch*,Event*);
	void onTouchCancelled(Touch*,Event*);

	bool _touch_sel;
	float _pos;
};

#endif
