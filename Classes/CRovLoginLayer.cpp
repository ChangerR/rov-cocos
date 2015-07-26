#include "CRovLoginLayer.h"
#include "CMainScene.h"
#include "ui/UIButton.h"
#include "ui/UIText.h"

USING_NS_CC;

Scene* CRovLoginLayer::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = CRovLoginLayer::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

CRovLoginLayer::CRovLoginLayer()
{
	_delegate = new RovEditDelegate(this);
	_ip_e = nullptr;
	_username_e = nullptr;
	_passwd_e = nullptr;
}


CRovLoginLayer::~CRovLoginLayer()
{
	delete _delegate;
	CC_SAFE_RELEASE(_ip_e);
	CC_SAFE_RELEASE(_username_e);
	CC_SAFE_RELEASE(_passwd_e);
}

bool CRovLoginLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto s_back = Sprite::create("res/back.png");
	if (s_back)
	{
		CCLOG("%f %f", s_back->getContentSize().width, s_back->getContentSize().height);
		CCLOG("%f %f", visibleSize.width, visibleSize.height);;
		s_back->setScaleX(visibleSize.width / s_back->getContentSize().width);
		s_back->setScaleY(visibleSize.height / s_back->getContentSize().height);
		//s_back->setAnchorPoint(Vec2(0.5f,0.5f));
		//this->
		setContentSize(visibleSize);
		s_back->setPosition(Vec2(origin.x + visibleSize.width /2 , origin.y + visibleSize.height / 2));
		this->addChild(s_back, 1);
	}

	auto b_exit = ui::Button::create("CloseNormal.png", "CloseSelected.png", "", ui::Widget::TextureResType::PLIST);
	if (b_exit)
	{
		b_exit->addClickEventListener(CC_CALLBACK_1(CRovLoginLayer::endEvent, this));
		b_exit->setAnchorPoint(Vec2(1.0f, 0.f));
		b_exit->setPosition(Vec2(visibleSize.width, 0));
		b_exit->setScale(visibleSize.width / 15.f / b_exit->getContentSize().width);
		this->addChild(b_exit, 8);
	}

	auto editBoxSize = Size(visibleSize.width / 2, 40);

	auto _t = ui::Text::create("IP ADDRESS:", "fonts/Marker Felt.ttf",25);
	_t->setColor(Color3B::YELLOW);
	_t->setAnchorPoint(Vec2(0, 0.5f));
	_t->setPosition(Vec2(origin.x + visibleSize.width / 4, origin.y + visibleSize.height * 3 / 4));
	_t->setContentSize(editBoxSize);
	addChild(_t, 8);

	std::string pNormalSprite = "res/orange_edit.png";
	_ip_e = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create(pNormalSprite));
	_ip_e->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 3 / 4 - 50));
	_ip_e->setFontName("Marker Felt");
	_ip_e->setFontSize(25);
	_ip_e->setFontColor(Color3B::RED);
	_ip_e->setPlaceHolder("IP:");
	_ip_e->setPlaceholderFontColor(Color3B::WHITE);
	_ip_e->setMaxLength(15);
	_ip_e->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	_ip_e->setDelegate(_delegate);
	CC_SAFE_RETAIN(_ip_e);
	addChild(_ip_e,8);

	_t = ui::Text::create("USER NAME:", "fonts/Marker Felt.ttf", 25);
	_t->setColor(Color3B::YELLOW);
	_t->setAnchorPoint(Vec2(0, 0.5f));
	_t->setPosition(Vec2(origin.x + visibleSize.width / 4, origin.y + visibleSize.height * 3 / 4 - 100));
	_t->setContentSize(editBoxSize);
	addChild(_t, 8);

	_username_e = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create(pNormalSprite));
	_username_e->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 3 / 4 - 150));
	_username_e->setFontName("Marker Felt");
	_username_e->setFontSize(25);
	_username_e->setFontColor(Color3B::RED);
	_username_e->setPlaceHolder("NAME:");
	_username_e->setPlaceholderFontColor(Color3B::WHITE);
	_username_e->setMaxLength(16);
	_username_e->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	_username_e->setDelegate(_delegate);
	CC_SAFE_RETAIN(_username_e);
	addChild(_username_e, 8);

	_t = ui::Text::create("PASSWORD:", "fonts/Marker Felt.ttf", 25);
	_t->setColor(Color3B::YELLOW);
	_t->setAnchorPoint(Vec2(0, 0.5f));
	_t->setPosition(Vec2(origin.x + visibleSize.width / 4, origin.y + visibleSize.height * 3 / 4 - 200));
	_t->setContentSize(editBoxSize);
	addChild(_t, 8);

	_passwd_e = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create(pNormalSprite));
	_passwd_e->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 3 / 4 - 250));
	_passwd_e->setFontName("Marker Felt");
	_passwd_e->setFontSize(25);
	_passwd_e->setFontColor(Color3B::RED);
	_passwd_e->setPlaceHolder("PASSWD:");
	_passwd_e->setPlaceholderFontColor(Color3B::WHITE);
	_passwd_e->setMaxLength(16);
	_passwd_e->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	_passwd_e->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
	_passwd_e->setDelegate(_delegate);
	CC_SAFE_RETAIN(_passwd_e);
	addChild(_passwd_e, 8);

	auto b_login = ui::Button::create("Default/Button_sc_Normal.png", "Default/Button_sc_Press.png", "Default/Button_Disable.png");
	if (b_login)
	{
		b_login->addClickEventListener(CC_CALLBACK_1(CRovLoginLayer::loginEvent, this));
		b_login->setTitleFontSize(24);
		b_login->setTitleColor(Color3B(0x41, 0x41, 0x46));
		b_login->setTitleText("login");
		b_login->setAnchorPoint(Vec2(0.f, 0.5f));
		b_login->setScale9Enabled(true);
		b_login->setContentSize(Size(editBoxSize.width / 3, editBoxSize.height));
		b_login->setPosition(Vec2(origin.x + visibleSize.width  /2 - editBoxSize.width /2  + 50, origin.y + visibleSize.height * 3 / 4 - 340));
		this->addChild(b_login, 8);
	}

	auto b_guestlogin = ui::Button::create("Default/Button_Normal.png", "Default/Button_Press.png", "Default/Button_Disable.png");
	if (b_guestlogin)
	{
		b_guestlogin->addClickEventListener(CC_CALLBACK_1(CRovLoginLayer::guestLoginEvent, this));
		b_guestlogin->setTitleFontSize(24);
		b_guestlogin->setTitleColor(Color3B(0x41, 0x41, 0x46));
		b_guestlogin->setTitleText("guest login");
		b_guestlogin->setAnchorPoint(Vec2(1.f, 0.5f));
		b_guestlogin->setScale9Enabled(true);
		b_guestlogin->setContentSize(Size(editBoxSize.width / 3, editBoxSize.height));
		b_guestlogin->setPosition(Vec2(origin.x + visibleSize.width /2 + editBoxSize.width / 2 - 50, origin.y + visibleSize.height * 3 / 4 - 340));
		this->addChild(b_guestlogin, 8);
	}
	
	return true;
}

void CRovLoginLayer::endEvent(Ref *pSender)
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

void CRovLoginLayer::loginEvent(Ref *pSender)
{
	Scene* sc = CMainScene::createScene(_ip.c_str(),true,"mjpeg");

	if (sc)
	{
		Director::getInstance()->replaceScene(sc);
	}
	
}

void CRovLoginLayer::guestLoginEvent(Ref *pSender)
{
	Scene* sc = CMainScene::createScene(_ip.c_str(), false,"mjpeg");
	if (sc)
	{
		Director::getInstance()->replaceScene(sc);
	}
}
