#ifndef __ROV_CONTROLER_
#define __ROV_CONTROLER_
#include "stringc.h"
#include "network/SocketIO.h"
#include <thread>
#include "CRovStatus.h"

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

class CRovControler :public cocos2d::Ref
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
	CRovControler(cocos2d::Layer* container,const stringc& url,int u_time);
	~CRovControler(void);

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

	float getPower() const{
		return power;
	}

	void setPosition(float t,float y,float l) {
		pos.throttle = t * power;
		pos.yaw =  y * power;
		pos.lift = l * power;
	}

	void setPower(float f){
		power = f;
	}

	void addPower() {
		if (power >= 1.0f)
			return;
		power += 0.05f;
	}

	void decPower() {
		if (power <= 0.1f)
			return;
		power -= 0.05f;
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

	bool getRunningState() const{
		return isRunning;
	}

	void update() {

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
	private:
		CRovControler* father;
	};
	
private:
	cocos2d::network::SIOClient* client;
	Position pos;
	Position old_pos;
	float power; 
	float vtrim;
	float ttrim;
	float tilt;
	float lights;
	int update_time;
	bool isRunning;
	std::thread *hControl;
	ControlDelegate m_delegate;
	CRovStatus* m_status;
	stringc _url;
	cocos2d::Layer* _video_container;
};

#endif