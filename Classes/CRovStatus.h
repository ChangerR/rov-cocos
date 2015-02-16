#ifndef __CROV_STATUS__
#define __CROV_STATUS__
#include "cocos2d.h"
#include "stringc.h"
#include "ui/UIText.h"

struct NVGcontext;
class CRovStatus :
	public cocos2d::Node
{
public:
	class navdata{
	public:
		navdata() :HDGD(0.0),DEAP(0.f),ROLL(0.f),PITC(0.f),YAW(0.f),FTHR(0.f){}

		const navdata& operator = (const navdata& nav) {
			HDGD = nav.HDGD;
			DEAP = nav.DEAP;
			ROLL = nav.ROLL;
			PITC = nav.PITC;
			YAW = nav.YAW;
			FTHR = nav.FTHR;
			return *this;
		}

		double HDGD; //Compass heading in degrees
		double DEAP; //Depth in meters
		double ROLL; //Roll in degrees
		double PITC; //Pitch in degrees
		double YAW; //Yaw in degrees
		double FTHR; //% of power in forward thrust
	};

	class capedata {
	public:
		capedata():PRES(0.f),TEMP(0.f),FMEM(0.),VOUT(0.),IOUT(0.),ATMP(0.),VER(),UTIM(0.),MATC(false),
				CMNT(0),CMTG(0),CMPD(){}
		const capedata& operator = (const capedata& cape) {
			PRES = cape.PRES; TEMP = cape.TEMP; FMEM = cape.FMEM; VOUT = cape.VOUT;
			IOUT = cape.IOUT; ATMP = cape.ATMP; VER = cape.VER; UTIM = cape.UTIM;
			MATC = cape.MATC; CMNT = cape.CMNT; CMTG = cape.CMTG; CMPD = cape.CMPD;
			return *this;
		}

		double PRES; //Pressure in millibars
		double TEMP; //Temperature in C
		double FMEM; //Free memory on the Arduino in bytes
		double VOUT; //Voltage as meassed at the cape in milli-volts
		double IOUT; //Current measured in to the cape in milli-amps.
		double ATMP; //Arduino internal temp in Cellcius (should not get above ~86)
		stringc VER;  //version number of the OpenRov firmware
		double UTIM; //up-time since Arduino was started in milliseconds
		bool MATC; //Motors on-line indicator
		int CMNT; //Camera Mount X Rotation in milliseconds
		int CMTG; //Camera Mount X Target Rotation in milliseconds
		stringc CMPD;
	};

	CRovStatus();

	virtual ~CRovStatus();

	void updateNavData(navdata& nav);

	void updateCapeDate(capedata& cape);

	bool init();

	void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags);

	void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags);

	void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);

	static CRovStatus* create();

private:
	navdata _navdata;
	capedata _capedate;
	cocos2d::ui::Text* _status;
	cocos2d::CustomCommand _customCommand;
	NVGcontext* _vg;
	int _font_face;
};

#endif