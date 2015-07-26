#ifndef __ROV_UDP_RECEIVER_
#define __ROV_UDP_RECEIVER_
#include "IStreamReceiver.h"
#include "platform/CCImage.h"
#include <mutex>
#include <thread>
#include <list>

class CCUdpReceiver:public IStreamReceiver{
public:
	CCUdpReceiver();
	
	~CCUdpReceiver();

	bool openReceiver(const char* path);
	
	void closeReceiver();
	
	void push();
	
	cocos2d::Image* pop();
	
	void capture() {
		capture_pic = true;
	}

	bool restart() {
		return false;
	}

	bool getRunningState();

	static bool capture_pic;
	
	static unsigned int udp_receiver(CCUdpReceiver* data);
protected:
	static int ffmpeg_init;
	std::list<cocos2d::Image*> images;
	unsigned char* m_buf;
	int _width,_height;
	std::thread* receiver_handle;
	std::mutex m_lock;
	bool isRunning;
	char m_url[256];
};

#endif