#ifndef __ROV_LOGIN__H
#define __ROV_LOGIN__H
#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "stringc.h"
#include "ui/UIEditBox/UIEditBox.h"
class CRovLoginLayer :public cocos2d::Layer
{
public:

	static cocos2d::Scene* createScene();

	CRovLoginLayer();

	virtual ~CRovLoginLayer();

	virtual bool init();

	void endEvent(Ref *pSender);

	void loginEvent(Ref *pSender);

	void guestLoginEvent(Ref *pSender);

	CREATE_FUNC(CRovLoginLayer);
public:

	class RovEditDelegate :public cocos2d::ui::EditBoxDelegate {
	public:
		RovEditDelegate(CRovLoginLayer* father) {
			_father = father;
		}

		virtual void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox) {};

		virtual void editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox) {};

		virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text) {};

		virtual void editBoxReturn(cocos2d::ui::EditBox* editBox){
			if (editBox == _father->_ip_e)
			{
				_father->_ip.clear();
				_father->_ip.append(editBox->getText());
			}else if (editBox == _father->_username_e)
			{
				_father->_name.clear();
				_father->_name.append(editBox->getText());
			}else if (editBox == _father->_passwd_e)
			{
				_father->_passwd.clear();
				_father->_passwd.append(editBox->getText());
			}
		};
	private:
		CRovLoginLayer* _father;
	};

private:
	cocos2d::ui::EditBox* _ip_e;
	cocos2d::ui::EditBox* _username_e;
	cocos2d::ui::EditBox* _passwd_e;
	stringc _ip;
	stringc _name;
	stringc _passwd;
	RovEditDelegate* _delegate;
};

#endif