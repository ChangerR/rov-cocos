#ifndef __CENTERTAINMENTCONTROLER_ROV_
#define __CENTERTAINMENTCONTROLER_ROV_
#include "stringc.h"
#include "cocos2d.h"
#include "IControler.h"
#include "CCStreamVideoPlayer.h"
#ifdef _WIN32
#include <winsock2.h>
#endif
#ifdef __linux__
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
typedef int SOCKET;
#define SOCKADDR_IN struct sockaddr_in
#define SOCKADDR struct sockaddr
#define SOCKET_ERROR -1
#define closesocket ::close
#endif

class CEntertainmentControler :public IControler{
public:
	CEntertainmentControler();
	virtual ~CEntertainmentControler();
	
	bool init(const stringc& ip);
	bool openControl();
	void closeControl();

	static unsigned int __stdcall _recv_thread(CEntertainmentControler* pointer);
	
	void heartBeat(float dt);
	
	void setCameraTilt(float value);

	void adjustCameraTilt(float value);

	void setLights(float value);

	void adjustLights(float value);

	void setPower(int value);

	bool getRunningState();

	void setThrust(int t, int y, int l);

	bool restartStream();

	void setRovStatus(CRovStatus* status);

	void control_update(float);

	static CEntertainmentControler* create(const stringc& url);

	int sendCmd(const char* c);

	void onStreamOn();

	void beginPing(float);

	void endPing(const char*);

private:
	CCStreamVideoPlayer* _displayer;
	bool _isRunning;
	bool _isHandSank;
	SOCKET _sock;
	std::thread* _recv;
	stringc _ip;
	std::mutex _lock;

	long _ping_time;
	long _start_time;
	bool _ping_set;
	int _ping_num;

	int _thr, _yaw, _lift;
	int _o_thr, _o_yaw, _o_lift;
	CRovStatus* _status;

	int _power;
#ifdef _WIN32
	static int socket_init;
	static bool winsock_init();
	static void winsock_close();
#endif

};

#endif