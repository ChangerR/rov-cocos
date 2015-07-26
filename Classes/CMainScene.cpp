#include "CMainScene.h"
#include "CRovStatus.h"
#include "ui/UIWidget.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "ui/UIButton.h"
#include "CUIJoystick.h"
#include "CUIScrollBar.h"
#include "CCaptureNode.h"
#include "CCStreamVideoPlayer.h"
#include "CEntertainmentControler.h"

USING_NS_CC;


// on "init" you need to initialize your instance
bool CMainScene::init(const char* ip, bool show_ctrl,const char* _proto)
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
	menu = nullptr;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//auto status = CRovStatus::create(visibleSize);
	//addChild(status, 2);

	m_rovCtrl = nullptr;

	if (_check_is_ip(ip))
	{
		if (strcmp(_proto, "mjpeg") == 0) {
			stringc url = stringc("http://") + ip;
			m_rovCtrl = CRovControler::create(url + ":8080/", 200);
		}else if (strcmp(_proto,"mpegts") == 0)
		{
			m_rovCtrl = CEntertainmentControler::create(ip);
			if (m_rovCtrl)
				((CEntertainmentControler*)m_rovCtrl)->openControl();
		}		
	}
	else {
		CCLOG("%s is not a ip address\n",ip);
	}

	if (m_rovCtrl){
		m_rovCtrl->setAnchorPoint(cocos2d::Vec2::ZERO);
		m_rovCtrl->setContentSize(visibleSize);
		auto status = CRovStatus::create(visibleSize);
		addChild(m_rovCtrl, 0);
		addChild(status, 2);
		m_rovCtrl->setRovStatus(status);
	}

	ui::Button* b_exit = ui::Button::create("CloseNormal.png", "CloseSelected.png", "", ui::Widget::TextureResType::PLIST);

	b_exit->setAnchorPoint(Vec2(1.0f, 0.0f));
	b_exit->setPosition(Vec2(visibleSize.width, 0.f));
	b_exit->setEnabled(true);
	b_exit->setPressedActionEnabled(true);
	b_exit->setScale(visibleSize.width / 15.f / b_exit->getContentSize().width);

	if (show_ctrl)
	{
		
		Size button_size = Size::ZERO;
		float button_height = visibleSize.width / 10.f;

		auto b_lift = ui::Button::create("lift.png", "lift_red.png", "", ui::Widget::TextureResType::PLIST);
		if (b_lift)
		{
			b_lift->addTouchEventListener(CC_CALLBACK_2(CMainScene::onBtnLiftTouchEvent, this));
			button_size = b_lift->getContentSize();
			if (button_size.height == 0.f)
			{
				CC_SAFE_RELEASE(b_lift);
				return false;
			}
			b_lift->setScale(button_height / button_size.height);
			b_lift->setPosition(Vec2(visibleSize.width - 100 - button_height / 2, 25 + button_height * 3 / 2));
			this->addChild(b_lift, 8);
		}

		auto b_divi = ui::Button::create("lift_down.png", "lift_down_red.png", "", ui::Widget::TextureResType::PLIST);
		if (b_divi)
		{
			b_divi->addTouchEventListener(CC_CALLBACK_2(CMainScene::onBtnDiveTouchEvent, this));
			b_divi->setScale(button_height / button_size.height);
			b_divi->setPosition(Vec2(visibleSize.width - 100 - button_height / 2, 25 + button_height / 2));
			this->addChild(b_divi, 8);
		}

		
		auto _joy = CUIJoystick::create("res/joystick_manuel.png", "res/joystick_halo.png");
		if (_joy)
		{
			_joy->setContentSize(Size(visibleSize.width / 4, visibleSize.width / 4));
			_joy->setAnchorPoint(Vec2::ZERO);
			_joy->setPosition(Vec2(30,30));
			_joy->setJoystickPositionChangeHandler(CC_CALLBACK_2(CMainScene::onJoySticEvent, this));
			this->addChild(_joy, 8);
		}
		
//		auto b_up = ui::Button::create("up.png", "up_red.png","",ui::Widget::TextureResType::PLIST);
//		if (b_up)
//		{
//
//			b_up->addTouchEventListener(CC_CALLBACK_2(RovScene::onBtnUpTouchEvent, this));
//			
//			b_up->setScale(button_height/ button_size.height);
//			b_up->setPosition(Vec2(25 + button_height*3/2 ,25 + button_height * 5 /2));
//			this->addChild(b_up,8);
//		}
//		
//		auto b_left = ui::Button::create("left.png", "left_red.png","", ui::Widget::TextureResType::PLIST);
//		if (b_left)
//		{
//			b_left->addTouchEventListener(CC_CALLBACK_2(RovScene::onBtnLeftTouchEvent, this));
//			b_left->setScale(button_height / button_size.height);
//			b_left->setPosition(Vec2(25 + button_height / 2, 25 + button_height * 3 / 2));
//			this->addChild(b_left, 8);
//		}
//
//		auto b_right = ui::Button::create("right.png", "right_red.png", "", ui::Widget::TextureResType::PLIST);
//		if (b_right)
//		{
//			b_right->addTouchEventListener(CC_CALLBACK_2(RovScene::onBtnRightTouchEvent, this));
//			b_right->setScale(button_height / button_size.height);
//			b_right->setPosition(Vec2(25 + button_height * 5 / 2, 25 + button_height * 3 / 2));
//			this->addChild(b_right, 8);
//		}
//
//		auto b_down = ui::Button::create("down.png", "down_red.png", "", ui::Widget::TextureResType::PLIST);
//		if (b_down)
//		{
//			b_down->addTouchEventListener(CC_CALLBACK_2(RovScene::onBtnDownTouchEvent, this));
//			b_down->setScale(button_height / button_size.height);
//			b_down->setPosition(Vec2(25 + button_height * 3 / 2, 25 + button_height / 2));
//			this->addChild(b_down, 8);
//		}

		auto b_setting = ui::Button::create("setting.png","setting_sel.png", "", ui::Widget::TextureResType::PLIST);
		if (b_setting)
		{
			b_setting->setAnchorPoint(Vec2(1.f,1.f));
			b_setting->setPosition(Vec2(visibleSize.width,visibleSize.height));
			b_setting->addClickEventListener(CC_CALLBACK_1(CMainScene::onSettintEvent,this));
			b_setting->setScale(0.5f);
			this->addChild(b_setting, 8);

			menu = CSLoader::createNode("Layer.csb");
			CC_SAFE_RETAIN(menu);
			if (menu)
			{
				menu->setScale(visibleSize.width/2/menu->getContentSize().width);
				ui::Helper::doLayout(menu);
				menu->setVisible(false);
				menu->ignoreAnchorPointForPosition(false);
				menu->setAnchorPoint(Vec2(.5f, .5f));
				menu->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y + visibleSize.height/2));
				this->addChild(menu, 10);	

				auto list_ = menu->getChildByTag(21);

				if (list_)
				{
					auto menu_close = (ui::Button*)list_->getChildByName("Button_1");
					if (menu_close)
						menu_close->addClickEventListener(CC_CALLBACK_1(CMainScene::onMenuCloseEvent,this));

					auto power_viewr = (ui::Layout*)list_->getChildByName("ListView_2");

					if (power_viewr)
					{
						ui::Slider* slider = ui::Slider::create();
						slider->loadBarTexture("res/sliderTrack.png");
						slider->loadSlidBallTextures("res/sliderThumb.png", "res/sliderThumb.png", "");
						slider->loadProgressBarTexture("res/sliderProgress.png");
						//slider->setContentSize(Size(menu_light_view->getContentSize().width * 150 / 480, slider->getContentSize().height * 2.0f));
						slider->addEventListener(CC_CALLBACK_2(CMainScene::slider_power_Event, this));
						slider->setPercent(10);
						power_viewr->addChild(slider);
						power_viewr->requestDoLayout();
					}

					auto menu_light_view = (ui::Layout*)list_->getChildByName("ListView_4");

					if (menu_light_view)
					{
						ui::Slider* slider = ui::Slider::create();
						slider->loadBarTexture("res/sliderTrack.png");
						slider->loadSlidBallTextures("res/sliderThumb.png", "res/sliderThumb.png", "");
						slider->loadProgressBarTexture("res/sliderProgress.png");
						//slider->setContentSize(Size(menu_light_view->getContentSize().width * 150 / 480, slider->getContentSize().height * 2.0f));
						slider->addEventListener(CC_CALLBACK_2(CMainScene::slider_light_Event, this));
						menu_light_view->addChild(slider);
						menu_light_view->requestDoLayout();
					}

					auto list_2 = menu->getChildByName("ListView_3");

					if (list_2)
					{
						auto bc_up = (ui::Button*)list_2->getChildByName("Button_2");
						if (bc_up)
							bc_up->addClickEventListener(CC_CALLBACK_1(CMainScene::onCameraUpEvent,this));

						auto bc_rec = (ui::Button*)list_2->getChildByName("Button_3");
						if (bc_rec)
							bc_rec->addClickEventListener(CC_CALLBACK_1(CMainScene::onCameraRecEvent, this));

						auto bc_down = (ui::Button*)list_2->getChildByName("Button_4");
						if (bc_down)
							bc_down->addClickEventListener(CC_CALLBACK_1(CMainScene::onCameraDownEvent, this));
					}
				}
			
			}

		}
		auto b_restart = ui::Button::create("res/restart.png", "res/restart2.png");
		if (b_restart)
		{
			b_restart->addClickEventListener(CC_CALLBACK_1(CMainScene::onRestartVideo, this));
			b_restart->setScale(b_setting->getContentSize().height * 0.5f/ b_restart->getContentSize().height );
			b_restart->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
			b_restart->setPosition(Vec2(visibleSize.width - b_setting->getContentSize().height*2.5f, visibleSize.height));
			this->addChild(b_restart, 8);
		}
	}
	

	if (b_exit) {
		b_exit->addClickEventListener(CC_CALLBACK_1(CMainScene::endEvent, this));
		this->addChild(b_exit, 8);
	}

	auto bar = CUIScrollBar::create("res/sliderThumb.png","res/sliderVertical.png");
	if (bar)
	{
		bar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
		bar->setPosition(Vec2(visibleSize.width - 15, visibleSize.height / 2));
		bar->setScrollBarEvent(CC_CALLBACK_1(CMainScene::onCamScrollEvent, this));
		this->addChild(bar, 8);
	}

	_capture_node = CCaptureNode::create();
	if (_capture_node)
		addChild(_capture_node, 100);

	auto key_listener = EventListenerKeyboard::create();
	if (key_listener)
	{
		key_listener->onKeyPressed = CC_CALLBACK_2(CMainScene::onKeyPressed, this);
		key_listener->onKeyReleased = CC_CALLBACK_2(CMainScene::onKeyReleased, this);
		this->_eventDispatcher->addEventListenerWithSceneGraphPriority(key_listener, this);
	}
    return true;
}

void CMainScene::endEvent(Ref *pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif

	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

CMainScene::~CMainScene()
{
	CC_SAFE_RELEASE(m_rovCtrl);
	CC_SAFE_RELEASE(menu);
	CC_SAFE_RELEASE(_capture_node);
}

void CMainScene::onSettintEvent(Ref *pSender)
{
	if (menu)
	{
		menu->setVisible(true);
	}
}

//void RovScene::onBtnUpTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
//{
//	if (!m_rovCtrl)return;
//
//	switch (type)
//	{
//	case cocos2d::ui::Widget::TouchEventType::BEGAN:
//		m_rovCtrl->setPosition(FORWARD_VALUE, 0, 0);
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::MOVED:
//
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::ENDED:
//		m_rovCtrl->setPosition(0, 0, 0);
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::CANCELED:
//		m_rovCtrl->setPosition(0, 0, 0);
//		break;
//
//	default:
//		break;
//	}
//}
//
//void RovScene::onBtnDownTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
//{
//	if (!m_rovCtrl)return;
//
//	switch (type)
//	{
//	case cocos2d::ui::Widget::TouchEventType::BEGAN:
//		m_rovCtrl->setPosition(BACK_VALUE, 0, 0);
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::MOVED:
//
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::ENDED:
//		m_rovCtrl->setPosition(0, 0, 0);
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::CANCELED:
//		m_rovCtrl->setPosition(0, 0, 0);
//		break;
//
//	default:
//		break;
//	}
//}
//
//void RovScene::onBtnLeftTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
//{
//	if (!m_rovCtrl)return;
//
//	switch (type)
//	{
//	case cocos2d::ui::Widget::TouchEventType::BEGAN:
//		m_rovCtrl->setPosition(0, LEFT_VALUE, 0);
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::MOVED:
//
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::ENDED:
//		m_rovCtrl->setPosition(0, 0, 0);
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::CANCELED:
//		m_rovCtrl->setPosition(0, 0, 0);
//		break;
//
//	default:
//		break;
//	}
//}
//
//void RovScene::onBtnRightTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
//{
//	if (!m_rovCtrl)return;
//
//	switch (type)
//	{
//	case cocos2d::ui::Widget::TouchEventType::BEGAN:
//		m_rovCtrl->setPosition(0, RIGHT_VALUE, 0);
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::MOVED:
//
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::ENDED:
//		m_rovCtrl->setPosition(0, 0, 0);
//		break;
//
//	case cocos2d::ui::Widget::TouchEventType::CANCELED:
//		m_rovCtrl->setPosition(0, 0, 0);
//		break;
//
//	default:
//		break;
//	}
//}

void CMainScene::onBtnLiftTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (!m_rovCtrl)return;

	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, INVALID_THRUST_ARG, UP_VALUE);
		break;

	case cocos2d::ui::Widget::TouchEventType::MOVED:

		break;

	case cocos2d::ui::Widget::TouchEventType::ENDED:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, INVALID_THRUST_ARG, 0);
		break;

	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, INVALID_THRUST_ARG, 0);
		break;

	default:
		break;
	}
}

void CMainScene::onBtnDiveTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (!m_rovCtrl)return;

	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, INVALID_THRUST_ARG, DOWN_VALUE);
		break;

	case cocos2d::ui::Widget::TouchEventType::MOVED:

		break;

	case cocos2d::ui::Widget::TouchEventType::ENDED:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, INVALID_THRUST_ARG, 0);
		break;

	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, INVALID_THRUST_ARG, 0);
		break;

	default:
		break;
	}
}


void CMainScene::onMenuCloseEvent(Ref* pSender)
{
	if (menu)
	{
		menu->setVisible(false);
	}
}

void CMainScene::slider_power_Event(Ref *pSender, ui::Slider::EventType type)
{
	if (type == ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		ui::Slider* slider = dynamic_cast<ui::Slider*>(pSender);
		float percent = slider->getPercent() / 100.f;
		int i = 0;

		if (percent <= 0.2f)
			i = 1;
		else if (percent <= 0.4f)
			i = 2;
		else if (percent <= 0.6f)
			i = 3;
		else if (percent <= 0.8f)
			i = 4;
		else
			i = 5;

		if (m_rovCtrl)
			m_rovCtrl->setPower(i);
	}
}

void CMainScene::slider_light_Event(Ref *pSender, ui::Slider::EventType type)
{
	if (type == ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		ui::Slider* slider = dynamic_cast<ui::Slider*>(pSender);
		float percent = slider->getPercent() / 100.f;
		if (m_rovCtrl)
			m_rovCtrl->setLights(percent);
	}
}

void CMainScene::onCameraUpEvent(Ref *pSender)
{
	if (m_rovCtrl)
	{
		m_rovCtrl->adjustCameraTilt(0.1f);
	}
}

void CMainScene::onCameraRecEvent(Ref *pSender)
{
	if (m_rovCtrl)
	{
		m_rovCtrl->setCameraTilt(0.f);
	}
}

void CMainScene::onCameraDownEvent(Ref *pSender)
{
	if (m_rovCtrl)
	{
		m_rovCtrl->adjustCameraTilt(-0.1f);
	}
}

void CMainScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (!m_rovCtrl)
		return;

	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		m_rovCtrl->setThrust(FORWARD_VALUE, INVALID_THRUST_ARG, INVALID_THRUST_ARG);
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		m_rovCtrl->setThrust(BACK_VALUE, INVALID_THRUST_ARG, INVALID_THRUST_ARG);
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, LEFT_VALUE, INVALID_THRUST_ARG);
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, RIGHT_VALUE, INVALID_THRUST_ARG);
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_SHIFT:
	case EventKeyboard::KeyCode::KEY_RIGHT_SHIFT:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, INVALID_THRUST_ARG, UP_VALUE);
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_CTRL:
	case EventKeyboard::KeyCode::KEY_RIGHT_CTRL:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, INVALID_THRUST_ARG, DOWN_VALUE);
		break;
	default:
		break;
	}
}

void CMainScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (!m_rovCtrl)
		return;

	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_BREAK:
		cocos2d::Director::getInstance()->end();
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		m_rovCtrl->setThrust(0 ,INVALID_THRUST_ARG , INVALID_THRUST_ARG);
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		m_rovCtrl->setThrust(0, INVALID_THRUST_ARG, INVALID_THRUST_ARG);
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, 0, INVALID_THRUST_ARG);
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, 0, INVALID_THRUST_ARG);
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_SHIFT:
	case EventKeyboard::KeyCode::KEY_RIGHT_SHIFT:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, INVALID_THRUST_ARG, 0);
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_CTRL:
	case EventKeyboard::KeyCode::KEY_RIGHT_CTRL:
		m_rovCtrl->setThrust(INVALID_THRUST_ARG, INVALID_THRUST_ARG, 0);
		break;
	case EventKeyboard::KeyCode::KEY_R:
		_capture_node->capture();
		break;
	default:
		break;
	}
}

#define SIN15 0.258819f
#define SIN75 0.965925f

void CMainScene::onJoySticEvent(const cocos2d::Vec2& pos, const cocos2d::Vec2& lastpos)
{
	if (!m_rovCtrl)
		return;
	
	float distance = pos.getLength();
	if (distance < 0.2f) {
		m_rovCtrl->setThrust(0, 0, INVALID_THRUST_ARG);
		return;
	}

	float sintheta = pos.y / distance;
	int t = 0, y = 0;
	int isNeg = pos.x >= 0 ? 1 : -1;

	if (sintheta <= -SIN75) {
		t = -1;
	}
	else if (sintheta <= -SIN15) {
		t = -1;
		y = -1;
	}
	else if (sintheta <= SIN15) {
		y = 1;
	}
	else if (sintheta <= SIN75) {
		t = 1;
		y = 1;
	}
	else
	{
		t = 1;
	}
	
	m_rovCtrl->setThrust(t, y * isNeg, INVALID_THRUST_ARG);
}

void CMainScene::onCamScrollEvent(float pos)
{
	if (!m_rovCtrl)
		return;

	m_rovCtrl->setCameraTilt((pos - 0.5f) * 2);
}

void CMainScene::onRestartVideo(Ref *pSender)
{
	if (m_rovCtrl)
		m_rovCtrl->restartStream();
}







