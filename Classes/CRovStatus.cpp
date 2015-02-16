#include "CRovStatus.h"

#include "nanovg.h"
#define NANOVG_GLES2_IMPLEMENTATION
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"
#include <math.h>

CRovStatus::CRovStatus() :_status(0), _vg(0)
{
}


CRovStatus::~CRovStatus()
{
	CC_SAFE_RELEASE(_status);
}

void CRovStatus::updateNavData(navdata& nav)
{
	_navdata = nav;
}

void CRovStatus::updateCapeDate(capedata& cape)
{
	_capedate = cape;
}

bool CRovStatus::init()
{
	_vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	_font_face = nvgCreateFont(_vg, "arial", "fonts/arial.ttf");
	if (!Node::init())
	{
		return false;
	}
	
	//_status = cocos2d::ui::Text::create("V:0 I:0", "Arial", 20);
	//CC_SAFE_RETAIN(_status);
	//_status->setAnchorPoint(cocos2d::Vec2(0.f, 0.f));
	//_status->setPosition(cocos2d::Vec2(0.f, 0.f));
	//addChild(_status, 1);

	return true;
}

CRovStatus* CRovStatus::create()
{
	CRovStatus *pRet = new CRovStatus();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

void CRovStatus::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags)
{
	char tmp[64]{0};
	sprintf(tmp, "V:%.2f I:%.2f", float(_capedate.VOUT), float(_capedate.IOUT));
	if (_status)
		_status->setString(tmp);

	Node::visit(renderer, parentTransform, parentFlags);
}

void CRovStatus::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(CRovStatus::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}

void CRovStatus::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{
	const float zeroWidth = 100.f, zeroGap = 10.f, radialRadius = 178.f, radialLimit = 60.f,
		tickRadius = 10.f;
	nvgBeginFrame(_vg, _contentSize.width, _contentSize.height, 1);

	nvgTranslate(_vg, _contentSize.width / 2, _contentSize.height / 2);

	//draw zero
	nvgBeginPath(_vg);
	nvgMoveTo(_vg, -zeroWidth, 0.0f);
	nvgLineTo(_vg, -zeroGap, 0.f);
	nvgMoveTo(_vg, zeroWidth, 0.0f);
	nvgLineTo(_vg, zeroGap, 0.f);
	nvgMoveTo(_vg, -zeroGap, zeroGap);
	nvgLineTo(_vg, 0.f, 0.f);
	nvgMoveTo(_vg, zeroGap, zeroGap);
	nvgLineTo(_vg, 0.f, 0.f);
	nvgStrokeColor(_vg, nvgRGB(255, 255, 0));
	nvgStrokeWidth(_vg, 2);
	nvgStroke(_vg);

	nvgBeginPath(_vg);
	nvgArc(_vg, 0, 0, radialRadius, - M_PI / 2 - M_PI * radialLimit / 180, -M_PI / 2 + M_PI * radialLimit / 180, NVG_CW);
	nvgStroke(_vg);

	nvgBeginPath(_vg);	
	for(int i = -4; i <= 4; i++) {
		nvgMoveTo(_vg, (radialRadius - tickRadius) * cosf(CC_DEGREES_TO_RADIANS(-90 + i * 15)), (radialRadius - tickRadius) * sinf(CC_DEGREES_TO_RADIANS(-90 + i * 15)));
		nvgLineTo(_vg, radialRadius * cosf(CC_DEGREES_TO_RADIANS(-90 + i * 15)), radialRadius * sinf(CC_DEGREES_TO_RADIANS(-90 + i * 15)));
	}
	nvgStroke(_vg);

	//draw roll
	nvgRotate(_vg, float(_navdata.ROLL));
	nvgFillColor(_vg,nvgRGB(255,255,255));
	nvgBeginPath(_vg);
	nvgMoveTo(_vg,0, -radialRadius);
	nvgLineTo(_vg,-5, -radialRadius + 10);
	nvgLineTo(_vg,+5, -radialRadius + 10);
	nvgClosePath(_vg);
	nvgFill(_vg);

	float readableRollAngle = int(round(float(_navdata.ROLL))) % 360;
	if (readableRollAngle > 180) {
		readableRollAngle = readableRollAngle - 360;
	}

	nvgBeginPath(_vg);
	nvgRect(_vg,-20, -radialRadius + 9, 40, 16);
	nvgFill(_vg);

	nvgBeginPath(_vg);	
	nvgFontFaceId(_vg, _font_face);
	nvgFontSize(_vg, 12);
	nvgTextAlign(_vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgFontBlur(_vg, 0);
	nvgFillColor(_vg, nvgRGB(0, 0, 0));
	char buf[16]{0};
	nvgText(_vg, 0, -radialRadius + 18, itoa(readableRollAngle,buf,16), NULL);

	///////////////////////////////////////////////////
	nvgEndFrame(_vg);
	cocos2d::GL::bindTexture2D(0);
	cocos2d::GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_NONE);

	cocos2d::GL::useProgram(0);
}
