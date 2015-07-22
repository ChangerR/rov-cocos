#include "CRovControler.h"
#include "json/document.h"
#include "CCStreamVideoPlayer.h"
#define __CCMJPEG_VIDEO_NAME "__CCMJPEG_X_JSSX_"

#ifdef ROV_USE_THREAD_TIMER_

unsigned int __stdcall control_update(CRovControler* p) {
	
	while(true) {
	std::this_thread::sleep_for(std::chrono::milliseconds(p->getUpdateTime()));
		if (!p->getRunningState())
			break;
		p->update();
	}
	CCLOG("control update return\n");
	return 1;
}
#endif

CRovControler::CRovControler(cocos2d::Layer* container, const stringc& url, int u_time) :update_time(u_time), m_delegate(this)
{
	power = 0.1f; 
	vtrim = 0;
	ttrim = 0;
	tilt = 0;
	lights = 0;
	_url = url;
	client = cocos2d::network::SocketIO::connect(url, m_delegate);
	isRunning = true;
	hControl= NULL;
	m_status = NULL;
	_video_container = container;
	CC_SAFE_RETAIN(container);
}

CRovControler::~CRovControler( void )
{
	isRunning = false;
	if (hControl)
		hControl->join();
	CC_SAFE_DELETE(hControl);
	if (client)
		client->disconnect();
	CC_SAFE_RELEASE(client);
	CC_SAFE_RELEASE(_video_container);
	CC_SAFE_RELEASE(m_status);
}

void CRovControler::nav_handle(cocos2d::network::SIOClient* client, const stringc& s) {
	if (!m_status)
		return;

	do {
		rapidjson::Document _doc;
		CRovStatus::navdata _nav;
		_doc.Parse<0>(s.c_str());
		CC_BREAK_IF(_doc.HasParseError());
		CC_BREAK_IF(!_doc.IsObject());
		CC_BREAK_IF(!_doc.HasMember("name"));
		CC_BREAK_IF(!_doc.HasMember("args"));

		CC_BREAK_IF(strcmp(_doc["name"].GetString(), "navdata"));
		const rapidjson::Value& pArgs = _doc["args"];
		CC_BREAK_IF(!pArgs.IsArray());

		for (int i = 0; i < pArgs.Capacity(); i++) {

			const rapidjson::Value& args = pArgs[i];

			if (args.HasMember("pitch")) {
				
				if (args["pitch"].IsString())
					_nav.PITC = atof(args["pitch"].GetString());
				else
					_nav.PITC = args["pitch"].GetDouble();
			}
				
			if (args.HasMember("roll")) {

				if (args["roll"].IsString())
					_nav.ROLL = atof(args["roll"].GetString());
				else
					_nav.ROLL = args["roll"].GetDouble();
			}
			
			if (args.HasMember("yaw")) {

				if (args["yaw"].IsString())
					_nav.YAW = atof(args["yaw"].GetString());
				else
					_nav.YAW = args["yaw"].GetDouble();
			}

			if (args.HasMember("thrust")) {

				if (args["thrust"].IsString())
					_nav.FTHR = atof(args["thrust"].GetString());
				else
					_nav.FTHR = args["thrust"].GetDouble();
			}

			if (args.HasMember("deapth")) {

				if (args["deapth"].IsString())
					_nav.DEAP = atof(args["deapth"].GetString());
				else
					_nav.DEAP = args["deapth"].GetDouble();
			}

			if (args.HasMember("hdgd")) {

				if (args["hdgd"].IsString())
					_nav.HDGD = atof(args["hdgd"].GetString());
				else
					_nav.HDGD = args["hdgd"].GetDouble();
			}
		}

		m_status->updateNavData(_nav);
	} while (0);

}

void CRovControler::cape_handle(cocos2d::network::SIOClient* client, const stringc& s)
{
	if (!m_status)
		return;
	do {
		rapidjson::Document _doc;
		CRovStatus::capedata _cape;
		_doc.Parse<0>(s.c_str());
		CC_BREAK_IF(_doc.HasParseError());
		CC_BREAK_IF(!_doc.IsObject());
		CC_BREAK_IF(!_doc.HasMember("name"));
		CC_BREAK_IF(!_doc.HasMember("args"));

		CC_BREAK_IF(strcmp(_doc["name"].GetString(), "status"));
		const rapidjson::Value& pArgs = _doc["args"];
		CC_BREAK_IF(!pArgs.IsArray());

		const rapidjson::Value& args = pArgs[rapidjson::SizeType(0)];

		if (args.HasMember("vout")) {

			if (args["vout"].IsString())
				_cape.VOUT = atof(args["vout"].GetString());
			else
				_cape.VOUT = args["vout"].GetDouble();
		}

		if (args.HasMember("iout")) {

			if (args["iout"].IsString())
				_cape.IOUT = atof(args["iout"].GetString());
			else
				_cape.IOUT = args["iout"].GetDouble();
		}

		
		m_status->updateCapeDate(_cape);
	} while (0);

}
#define _USE_VIDEO_ 1 

void CRovControler::video_start(cocos2d::network::SIOClient* client, const stringc& s)
{
	if (!_video_container)
		return;
	const char* p = _url.c_str() + 7;
	char buf[64]="http://", *p_buf = buf + 7;
	while (*p && *p != ':'&&*p != '/')*p_buf++ = *p++;
	
	strcpy(p_buf, ":8090/?action=stream");
#if _USE_VIDEO_ > 0
	auto node = CCStreamVideoPlayer::create(buf, Color4B(255, 255, 255, 255));
#else
	CCStreamVideoPlayer* node = nullptr;
#endif
	if (node) {
		CC_SAFE_RETAIN(node);
		node->setName(__CCMJPEG_VIDEO_NAME);
		node->setPosition(cocos2d::Director::getInstance()->getVisibleOrigin());
		_video_container->addChild(node, 0);
	}
}

void CRovControler::video_end(cocos2d::network::SIOClient* client, const stringc& s)
{
	if (!_video_container)
		return;
	auto node = _video_container->getChildByName(__CCMJPEG_VIDEO_NAME);
	if (node) {
		_video_container->removeChildByName(__CCMJPEG_VIDEO_NAME);
		CC_SAFE_RELEASE(node);
	}
}

void CRovControler::ControlDelegate::onConnect(cocos2d::network::SIOClient* client)
{
	father->isRunning = true;
	client->on("navdata", CC_CALLBACK_2(CRovControler::nav_handle,father));
	client->on("status", CC_CALLBACK_2(CRovControler::cape_handle, father));
	client->on("videoStarted",CC_CALLBACK_2(CRovControler::video_start,father));
	client->on("VideoStopped",CC_CALLBACK_2(CRovControler::video_end,father));
#ifdef ROV_USE_THREAD_TIMER_
	father->hControl = new std::thread(control_update, father);
#endif
}

void CRovControler::ControlDelegate::onClose(cocos2d::network::SIOClient* client)
{
	father->isRunning = false;
#ifdef ROV_USE_THREAD_TIMER_
	if (father->hControl)
		father->hControl->join();
	CC_SAFE_DELETE(father->hControl);
#endif
}
