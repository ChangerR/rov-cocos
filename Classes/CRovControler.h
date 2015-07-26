#ifndef __ROV_CONTROLER_
#define __ROV_CONTROLER_
#include "stringc.h"
#include "network/SocketIO.h"
#include <thread>
#include "CRovStatus.h"
#include "IControler.h"
#include "CCStreamVideoPlayer.h"
typedef float f32;

const f32 ROUNDING_ERROR_f32 = 0.000001f;

inline bool equals(const f32 a, const f32 b, const f32 tolerance = ROUNDING_ERROR_f32)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}

#define	FORWARD_VALUE  -1.0f
#define	BACK_VALUE  1.0f
#define	LEFT_VALUE  -1.0f
#define RIGHT_VALUE  1.0f
#define UP_VALUE  1.0f
#define DOWN_VALUE  -1.0f
#define SAFE_EMIT(client,args0,args1) do{ \
											if(!client)break;\
											client->emit(args0,args1);\
										}while (0)

class CRovControler :public IControler
{
public:


	class Position {
	public:
		Position():throttle(0.f),yaw(0.f),lift(0.f){}

		bool equals(const Position& p) const {
			return (::equals(p.lift,lift)&&::equals(p.throttle,throttle)&&::equals(p.yaw,yaw));
		}

		void assign(const Position& p) {
			this->lift = p.lift;
			this->throttle = p.throttle;
			this->yaw = p.yaw;
		}

		stringc toJSONString() const{
			stringc tmp("{\"throttle\":");
			append_f32(tmp,throttle);
			tmp.append(",\"yaw\":");
			append_f32(tmp,yaw);
			tmp.append(",\"lift\":");
			append_f32(tmp,lift);
			tmp.append("}");
			return tmp;
		}

	public:
		float throttle;
		float yaw;
		float lift;
	};

public:

	CRovControler(const stringc& url,int u_time);

	~CRovControler(void);

	static CRovControler* create( const stringc& url, int u_time) {
		CRovControler* joy = new (std::nothrow)CRovControler( url,u_time);
		if (joy&&joy->init())
		{
			joy->autorelease();
			return joy;
		}
		CC_SAFE_RELEASE(joy);
		return nullptr;
	}

	bool init() {
		if (!Node::init())
			return false;

		schedule(schedule_selector(CRovControler::control_update), getUpdateTime() / 1000.f);
		return true;
	}

	void setRovStatus(CRovStatus* status) {
		m_status = status;
		CC_SAFE_RETAIN(m_status);
	}

	void setCameraTilt(float value) {

		this->tilt=value;
		if (this->tilt > 1) this->tilt =1;
		if (this->tilt < -1) this->tilt = -1;
		stringc tmp("[");
		append_f32(tmp,tilt);
		tmp.append("]");
		SAFE_EMIT(client,"tilt_update", tmp);

	}

	void adjustCameraTilt(float value) {
		this->tilt+=value;
		this->setCameraTilt(this->tilt);
	}

	void setLights(float value) {

		this->lights = value;
		if (this->lights>1) this->lights = 1;
		if (this->lights<0) this->lights = 0;
		stringc tmp("[");
		append_f32(tmp,lights);
		tmp.append("]");
		SAFE_EMIT(client, "brightness_update", tmp);

	}

	void adjustLights(float value) {
		if (this->lights==0 && value<0){ //this code rounds the horn so to speak by jumping from zero to max and vise versa
			this->lights = 0; //disabled the round the horn feature
		} else if (this->lights==1 && value>0){
			this->lights = 1; //disabled the round the horn feature
		} else {
			this->lights += value;
		}
		this->setLights(this->lights);
	}

	Position& getPositions(){
		return pos;
	}


	void setThrust(int t, int y, int l) {
		if (t != INVALID_THRUST_ARG)
			pos.throttle = t * power_thru;

		if (y != INVALID_THRUST_ARG) {
			if (y == 0)
				pos.yaw = 0.f;
			else if(abs(pos.throttle) > 0.000001f)
				pos.yaw = y * yaws / abs(y);
			else
				pos.yaw = y * power_thru;
		}
		if (l != INVALID_THRUST_ARG)
			pos.lift = l * power_vertical;

	}

	void setPower(int f){
		power_level = f;
		
		if (power_level <= 0)
			power_level = 1;
		if (power_level >= 6)
			power_level = 5;
		CRovStatus::_power_level = power_level;
		switch (power_level)
		{
		case 1:
			power_thru = 0.214f;
			power_vertical = 0.1f;
			yaws = 0.001f;
			break;
		case 2:
			power_thru = 0.216f;
			power_vertical = 0.2f;
			yaws = 0.002f;
			break;
		case 3:
			power_thru = 0.22f;
			power_vertical = 0.24f;
			yaws = 0.003f;
			break;
		case 4:
			power_thru = 0.23f;
			power_vertical = 0.26f;
			yaws = 0.005f;
			break;
		case 5:
			power_thru = 0.25f;
			power_vertical = 0.30f;
			yaws = 0.01f;
			break;
		default:
			break;
		}
	}

	void addPower() {
		setPower(power_level + 1);
	}

	void decPower() {
		setPower(power_level - 1);
	}

	void stopAll() {
		vtrim = 0;
		ttrim = 0;
		pos.throttle = 0;
		pos.yaw = 0;
		pos.lift = 0;
	}

	int getUpdateTime() const{
		return update_time;
	}

	void setUpdateTime(int u) {
		update_time = u;
	}

	bool getRunningState() {
		return isRunning;
	}

	void control_update(float dt) {

		if (isRunning&&!old_pos.equals(pos))
		{
			old_pos.assign(pos);
			SAFE_EMIT(client,"control_update", pos.toJSONString());
		}
	}
	
	float getVtrim() const {
		return vtrim;
	}

	float getTtrim() const {
		return ttrim;
	}	

	virtual bool restartStream();

	void nav_handle(cocos2d::network::SIOClient* client, const stringc& s);

	void cape_handle(cocos2d::network::SIOClient* client, const stringc& s);

	void video_start(cocos2d::network::SIOClient* client, const stringc& s);

	void video_end(cocos2d::network::SIOClient* client, const stringc& s);

public:
	class ControlDelegate :
		public cocos2d::network::SocketIO::SIODelegate {
	public:
		ControlDelegate(CRovControler* control) :father(control){}

		virtual void onConnect(cocos2d::network::SIOClient* client);

		virtual void onMessage(cocos2d::network::SIOClient* client, const std::string& data) {

		}

		virtual void onClose(cocos2d::network::SIOClient* client);

		virtual void onError(cocos2d::network::SIOClient* client, const std::string& data) {

		}

		virtual void fireEventToScript(cocos2d::network::SIOClient* client, const std::string& eventName, const std::string& data){}
	private:
		CRovControler* father;
	};
	
private:
	cocos2d::network::SIOClient* client;
	Position pos;
	Position old_pos;
	int power_level;
	float power_thru,power_vertical,yaws; 
	float vtrim;
	float ttrim;
	float tilt;
	float lights;
	int update_time;
	bool isRunning;
	std::thread *hControl;
	ControlDelegate m_delegate;
	CRovStatus* m_status;
	CCStreamVideoPlayer* m_player;
	stringc _url;
};

#endif