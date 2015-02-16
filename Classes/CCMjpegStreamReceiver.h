#ifndef __ROV_RECEIVER_
#define __ROV_RECEIVER_
#include "CCAsyncHttpClient.h"
#include "platform/CCImage.h"
#include <mutex>
#include <thread>

class CCMjpegStreamReceiver 
{
public:

	CCMjpegStreamReceiver(int max_ = 1024*512);

	~CCMjpegStreamReceiver(void);

	const stringc& getResponse();

	bool getRecvData(int *len);

	void closeReceiver();

	cocos2d::Image* pop();

	void push();

	bool openReceiver(const char* path);

	bool getRunningState() const {
		return isRunning;
	}

	static bool capture_pic;

private:

	CCAsyncHttpClient* p_conn;
	std::list<cocos2d::Image*> images;
	u8* m_buf;
	int max_size;
	std::thread* receiver_handle;
	std::mutex m_lock;
	bool isRunning;
};
#endif
