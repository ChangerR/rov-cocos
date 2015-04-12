#include "CUITabMenu.h"


CUITabMenu::CUITabMenu()
{
}


CUITabMenu::~CUITabMenu()
{
}

bool CUITabMenu::init()
{
	do 
	{
		CC_BREAK_IF(!Node::init());

		_backgound = ui::Layout::create();

		CC_BREAK_IF(!_backgound);

		return true;
	} while (0);

	return false;
}
