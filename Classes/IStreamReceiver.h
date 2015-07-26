#ifndef _ROV_I_STREAMRECEIVER_H
#define _ROV_I_STREAMRECEIVER_H
#include "platform/CCImage.h"

class IStreamReceiver {	
public:

	virtual bool openReceiver(const char* path) = 0;
	
	virtual void closeReceiver() = 0;
	
	virtual void push() = 0;
	
	virtual cocos2d::Image* pop() = 0;
	
	virtual void capture() = 0;

	virtual bool getRunningState() = 0;

	virtual bool restart() = 0;
};

#endif