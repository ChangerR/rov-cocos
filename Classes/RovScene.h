#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "RovControler.h"
#include "CCaptureNode.h"

class RovScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene(const char* url,bool show_ctrl);

	virtual ~RovScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init(const char*, bool show_ctrl);
    
    // a selector callback
	//void menuCloseCallback(cocos2d::Ref* pSender);

	void endEvent(Ref *pSender);

	void onSettintEvent(Ref *pSender);

	void onMenuCloseEvent(Ref* pSender);

	void onCameraUpEvent(Ref *pSender);

	void onCameraRecEvent(Ref *pSender);

	void onCameraDownEvent(Ref *pSender);

	void slider_power_Event(Ref *pSender, cocos2d::ui::Slider::EventType type);

	void slider_light_Event(Ref *pSender, cocos2d::ui::Slider::EventType type);

	//void onBtnUpTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//void onBtnDownTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//void onBtnLeftTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//void onBtnRightTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onBtnLiftTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onBtnDiveTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onJoySticEvent(const cocos2d::Vec2& pos,const cocos2d::Vec2& lastpos);

	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    // implement the "static create()" method manually
	static RovScene* create(const char* url, bool show_ctrl);

	void Update(float dt);

private:

	CRovControler* m_rovCtrl = nullptr;
	Node* menu;
	CCaptureNode* _capture_node = nullptr;
};

#endif // __HELLOWORLD_SCENE_H__
