#ifndef __ROV_CC_CAPTURE_NODE_
#define __ROV_CC_CAPTURE_NODE_
#include "cocos2d.h"

class CCaptureNode :
	public cocos2d::Node
{
public:
	CCaptureNode();
	virtual ~CCaptureNode();

	bool init();

	void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags);

	void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);

	void capture();

	static CCaptureNode* create();

private:
	bool _capture;
	cocos2d::CustomCommand _customCommand;
	cocos2d::Image* _buffer;
};
#endif
