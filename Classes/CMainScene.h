#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "CRovControler.h"
#include "CCaptureNode.h"
#include "IControler.h"

class CMainScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene(const char* const url,bool show_ctrl,const char* const  _proto)
	{
	    // 'scene' is an autorelease object
	    auto scene = Scene::create();

	    // 'layer' is an autorelease object
	    auto layer = CMainScene::create(url,show_ctrl,_proto);

	    // add layer as a child to scene
	    scene->addChild(layer);

	    // return the scene
	    return scene;
	}

	virtual ~CMainScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init(const char*, bool show_ctrl, const char* _proto);
    
    // a selector callback
	//void menuCloseCallback(cocos2d::Ref* pSender);

	void endEvent(Ref *pSender);

	void onSettintEvent(Ref *pSender);

	void onRestartVideo(Ref *pSender);

	void onMenuCloseEvent(Ref* pSender);

	void onCameraUpEvent(Ref *pSender);

	void onCameraRecEvent(Ref *pSender);

	void onCameraDownEvent(Ref *pSender);

	void onCamScrollEvent(float);

	void slider_power_Event(Ref *pSender, cocos2d::ui::Slider::EventType type);

	void slider_light_Event(Ref *pSender, cocos2d::ui::Slider::EventType type);

	//void onBtnUpTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//void onBtnDownTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//void onBtnLeftTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//void onBtnRightTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onBtnLiftTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onBtnDiveTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onJoySticEvent(const cocos2d::Vec2& ,const cocos2d::Vec2& );

	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    // implement the "static create()" method manually
	static CMainScene* create(const char* url, bool show_ctrl,const char* _proto)
	{
		CMainScene *pRet = new CMainScene();
		if (pRet && pRet->init(url,show_ctrl,_proto))
		{
			pRet->autorelease();
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = NULL;
			return NULL;
		}
	}


private:

	IControler* m_rovCtrl = nullptr;
	Node* menu;
	CCaptureNode* _capture_node = nullptr;
};

#endif // __HELLOWORLD_SCENE_H__
