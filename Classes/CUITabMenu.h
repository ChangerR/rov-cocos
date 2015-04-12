#ifndef __CUITABMENU_H
#define __CUITABMENU_H
#include "cocos2d.h"
#include "ui/UILayoutManager.h"
#include "ui/UILayout.h"
USING_NS_CC;

class CUITabMenu :
	public cocos2d::Node
{
public:
	virtual ~CUITabMenu();

	bool init();

private:
	CUITabMenu();

	ui::Layout* _backgound;
};


#endif
