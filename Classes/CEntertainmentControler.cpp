#include "CEntertainmentControler.h"
#define SERVER_PORT 8080
#define __CCUDP_VIDEO_NAME "__CCUDP_PLAYER_X_JSSX_"
#ifdef _WIN32
#include <Windows.h>
#endif

CEntertainmentControler::CEntertainmentControler() {
	_ip = "";
	_recv = NULL;
	_sock = INVALID_SOCKET;
	_isRunning = false;
	_isHandSank = false;
	_displayer = NULL;
#ifdef _WIN32
	if(socket_init <= 0) {
		socket_init = 0;
		winsock_init();
	}
	socket_init += 1;
#endif
}

CEntertainmentControler::~CEntertainmentControler() {
	
	CC_SAFE_RELEASE(_displayer);
	if (_isRunning)
	{
		closeControl();
	}
#ifdef _WIN32
	socket_init -= 1;
	if(socket_init <= 0) {
		winsock_close();
	}
#endif
}

bool CEntertainmentControler::init(const stringc& ip) {
	if ( !Node::init() )
    {
        return false;
    }
	
	_ip = ip;
	_ping_time = 0;
	
	schedule(schedule_selector(CEntertainmentControler::heartBeat), 45.f);
	schedule(schedule_selector(CEntertainmentControler::beginPing), 1.f);
	schedule(schedule_selector(CEntertainmentControler::control_update), 0.2f);

	_displayer = nullptr;
	return true;
}

bool CEntertainmentControler::openControl() {
	struct sockaddr_in _sockaddr;
	
	if(_isRunning == true)
		return false;
	
	_ping_time = 0;
	_start_time = 0;
	_ping_set = false;
	_ping_num = 1;

	_thr = _yaw = _lift = 0;
	_o_lift = _o_yaw = _o_lift = 0;

	_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	if(_sock == INVALID_SOCKET) {
		CCLOG("can not open socket\n");				
		return false;	
	}
	
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_addr.s_addr = inet_addr(_ip.c_str());
	_sockaddr.sin_port = htons(SERVER_PORT);
	
	if(SOCKET_ERROR == connect(_sock,(struct sockaddr*)&_sockaddr,sizeof(_sockaddr))) {
		CCLOG("connect server error\n");				
		return false;	
	}
	
	_isRunning = true;
	_isHandSank = false;
	
	_recv = new std::thread(CEntertainmentControler::_recv_thread,this);
	
	return true;
}

void CEntertainmentControler::closeControl() {
	sendCmd("0:::\r\n");
	_isRunning = false;

	if(_recv) {
		_recv->join();
		delete _recv;
	}
	if (_sock != INVALID_SOCKET) {
		shutdown(_sock, 0x02);
		closesocket(_sock);
	}
	_sock = INVALID_SOCKET;
}

inline int _inner_getline(char* _buffer,char* buf,int* wpos) {
	int nRead = 0;
	for(;nRead < *wpos;nRead++) {
		if(_buffer[nRead] == '\n')
			break;
	}
	
	if(_buffer[nRead] == '\n') {
		if(nRead > 4096)
			return -1;
		memcpy(buf,_buffer,nRead);
		if (buf[nRead - 1] == '\r') {
			buf[nRead - 1] = 0;
		}
		else {
			buf[nRead] = 0;
		}
		nRead++;
		*wpos -= nRead;
		for(int i = 0; i < *wpos;i++) {
			_buffer[i] = _buffer[nRead + i];
		}
		return nRead - 1;
	}
	return  -1;
}

unsigned int CEntertainmentControler::_recv_thread(CEntertainmentControler* pointer) {

	fd_set _set;
	struct timeval timeout;
	char *msg;
	char *str;
	char _hand[] = "ROVA:::\r\n";
	int sel_ret;
	int wpos = 0;
	int nrv = 0;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	
	msg = new char[4096];
	str = new char[1024];
	
	pointer->_lock.lock();
	if(SOCKET_ERROR == send(pointer->_sock,_hand,9,0)) {
		delete[] msg;
		delete[] str;
		pointer->_lock.unlock();
		CCLOG("CEntertainmentControler CLASS _recv_thread send error");
		return 1;
	}
	pointer->_lock.unlock();

	pointer->heartBeat(1.f);

	while(pointer->_isRunning) {
		FD_ZERO(&_set);
		FD_SET(pointer->_sock,&_set);
		
		if (SOCKET_ERROR == (sel_ret = select(pointer->_sock + 1,&_set, NULL, NULL,&timeout))) {
			delete[] msg;
			delete[] str;
			CCLOG("**ERROR*** slserver select function error\n");
			return 1;
		}
		
		if(sel_ret && FD_ISSET(pointer->_sock,&_set)) {
			nrv = recv(pointer->_sock,msg + wpos,4096 - wpos,0);
			wpos += nrv;
			
			while(_inner_getline(msg,str,&wpos) != -1) {
				if(pointer->_isHandSank) {
//CCLOG("recv %s",str);
					switch (*str)
					{
					case '2':
						if (!strcmp(str + 4,"stream_on"))
						{
							pointer->onStreamOn();
						}
						break;
					case '3':
						if (!strncmp("pong:", str + 4, 5)) {
							pointer->endPing(str + 9);
						}
						break;
					case '7':
						pointer->_status->parseNavData(str + 4);
						break;
					default:
						break;
					}
				}else{
					if(strcmp(str,"SLOK:::") != 0) {
						pointer->_isRunning = false;
						closesocket(pointer->_sock);
						pointer->_sock = INVALID_SOCKET;
						CCLOG("CEntertainmentControler CLASS _recv_thread:ERROR server");
						break;
					}
					pointer->_isHandSank = true;
					pointer->sendCmd("9:::start\r\n");
					pointer->sendCmd("9:::listen\r\n");
				}
			}
		}else
			std::this_thread::yield();
	}

	delete[] msg;
	delete[] str;
	return 0;
}

void CEntertainmentControler::heartBeat(float dt) {
	static const char heartbeat[] = "1:::\r\n";
	
	if(_isRunning&&_sock != INVALID_SOCKET) {
		sendCmd(heartbeat);
	}
}

#ifdef _WIN32
int CEntertainmentControler::socket_init = 0;

bool CEntertainmentControler::winsock_init() {
	WSADATA wsadata;
	if( WSAStartup( MAKEWORD( 2, 2 ), &wsadata ) )
	{
		CCLOG( "initializationing error!\n" );
		WSACleanup( );
		return false;
	}
	
	return true;
}

void CEntertainmentControler::winsock_close() {
	WSACleanup( );
}

#endif

void CEntertainmentControler::setCameraTilt(float value)
{

}

void CEntertainmentControler::adjustCameraTilt(float value)
{

}

void CEntertainmentControler::setLights(float value)
{

}

void CEntertainmentControler::adjustLights(float value)
{

}

void CEntertainmentControler::setPower(int value)
{
	_power = value;
}

bool CEntertainmentControler::getRunningState()
{
	return _isRunning;
}

void CEntertainmentControler::setThrust(int t, int y, int l)
{
	if (t != INVALID_THRUST_ARG)
		_thr = t;

	if (y != INVALID_THRUST_ARG)
		_yaw = y;

	if (l != INVALID_THRUST_ARG)
		_lift = l;
}

bool CEntertainmentControler::restartStream()
{
	return false;
}

void CEntertainmentControler::setRovStatus(CRovStatus* status)
{
	_status = status;
}

void CEntertainmentControler::control_update(float)
{
	if (_sock == INVALID_SOCKET)
		return;

	if (_o_lift != _lift || _o_thr != _thr || _o_yaw != _yaw)
	{
		char stmp[32];

		_o_thr = _thr;
		_o_yaw = _yaw;
		_o_lift = _lift;

		sprintf(stmp, "2:::go(%d,%d,%d)\r\n", _thr, _yaw, _lift);
		
		sendCmd(stmp);
	}
}

CEntertainmentControler* CEntertainmentControler::create(const stringc& url)
{
	CEntertainmentControler* joy = new (std::nothrow)CEntertainmentControler();
	if (joy&&joy->init(url))
	{
		joy->autorelease();
		return joy;
	}
	CC_SAFE_RELEASE(joy);
	return nullptr;
}

int CEntertainmentControler::sendCmd(const char* c)
{
	int len = strlen(c);

	_lock.lock();
	if (SOCKET_ERROR == send(_sock,c, len, 0)) {
		_lock.unlock();
		CCLOG("CEntertainmentControler CLASS  send error");
		return -1;
	}
	_lock.unlock();

	return len;
}

void CEntertainmentControler::onStreamOn()
{
	if (_displayer)
		return;

	stringc url = "udp://";

	url += _ip;
	url += ":9000";

	_displayer = CCStreamVideoPlayer::create(url.c_str(), cocos2d::Color4B(255,255,255,255));

	if(_displayer) {
		CC_SAFE_RETAIN(_displayer);
		_displayer->setName(__CCUDP_VIDEO_NAME);
		_displayer->setPosition(cocos2d::Director::getInstance()->getVisibleOrigin());
		addChild(_displayer, 0);
	}

}

void CEntertainmentControler::beginPing(float)
{
	char _tmps[32];
	
	if (_ping_set == false) 
		_ping_time = 999;
	_ping_num++;

	CRovStatus::_ping = _ping_time;

	sprintf_s(_tmps, "3:::ping:%d\r\n", _ping_num);
	_ping_set = false;
	_start_time = clock();
	sendCmd(_tmps);
}

void CEntertainmentControler::endPing(const char* s)
{
	do 
	{
		if (s == NULL)
			break;
		int i = atoi(s);

		if (i == _ping_num) {
			_ping_time = clock() - _start_time;

			_ping_time = _ping_time == 0 ? 1 : _ping_time;

			_ping_set = true;
		}
;
	} while (0);
}
