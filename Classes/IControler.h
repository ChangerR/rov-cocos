#ifndef __INTERFACE_ROV_CTRL_
#define __INTERFACE_ROV_CTRL_
#include "cocos2d.h"
#include "CRovStatus.h"

class IControler :public cocos2d::Node{
public:	

	virtual void setCameraTilt(float value) = 0;
	
	virtual void adjustCameraTilt(float value) = 0;
	
	virtual void setLights(float value) = 0;
	
	virtual void adjustLights(float value) = 0;
	
	virtual void setPower(float value) = 0;
	
	virtual bool getRunningState() = 0;
	
	virtual void setPosition(float t,float y,float l) = 0;
	
	virtual bool restartStream() = 0;
	
	virtual void setRovStatus(CRovStatus* status) = 0;
	
	virtual void control_update(float) = 0;
};

#endif