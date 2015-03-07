#ifndef __CUIJOYSTICK_NODE__
#define __CUIJOUSTICK_NODE__
#include "cocos2d.h"

class CUIJoystick :
	public cocos2d::Node
{
public:
	virtual ~CUIJoystick();

	bool init(const char* _joy, const char* _back, bool _use_local = true);

	static CUIJoystick* create(const char* _joy, const char* _back, bool _use_local = true);

	typedef std::function<void(const cocos2d::Vec2& newPos, const cocos2d::Vec2& lastPos)> JoystickHandler;

	void setJoystickPositionChangeHandler(JoystickHandler);

	void setContentSize(const cocos2d::Size& contentSize);

#ifdef __ROV_JOYSTICK_DEBUG_
	void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags);

	void onDraw(const cocos2d::Mat4& transform, uint32_t flags);
#endif

private:
	CUIJoystick();
	cocos2d::Sprite* _joystick;
	cocos2d::Sprite* _joy_background;
	cocos2d::Vec2 _joy_pos;

	cocos2d::Size _origin_size;
	JoystickHandler _handler;
	cocos2d::EventListenerTouchOneByOne* _touchEventListener;

	bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
	void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
	void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
	void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*);

	void handleTouchChange(cocos2d::Touch*);

	cocos2d::Node* _background;

#ifdef __ROV_JOYSTICK_DEBUG_
	cocos2d::CustomCommand _custom;
#endif
};

#endif
