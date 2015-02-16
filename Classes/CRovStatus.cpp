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
	
	nvgBeginFrame(_vg, _contentSize.width, _contentSize.height, 1);

	nvgTranslate(_vg, _contentSize.width / 2, _contentSize.height / 2);

	drawHorizon();

	drawZero();

	drawRoll();

	drawSpeed();

	drawAltitude();
	///////////////////////////////////////////////////
	nvgEndFrame(_vg);
	cocos2d::GL::bindTexture2D(0);
	cocos2d::GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_NONE);

	cocos2d::GL::useProgram(0);
}
const static  float zeroWidth = 100.f, zeroGap = 10.f, radialRadius = 178.f, radialLimit = 60.f,
tickRadius = 10.f, zeroPadding = 100.f, speedIndicatorHeight = 250.f, speedIndicatorWidth = 60.f,
speedAltOpacity = 0.2f, pixelsPer10Kmph = 50.f, yellowBoundarySpeed = 100.f, speedWarningWidth = 10.f, redBoundarySpeed = 130.f,
majorTickWidth = 10.f, minorTicksPer10Kmph = 5.f, minorTickWidth = 5.f, minorTicksPer100Ft = 5.f,
altIndicatorHeight = 250.f, altIndicatorWidth = 50.f, pixelsPer100Ft = 50.f, majorWidth = 100, minorWidth = 60, frontCameraFovY = 70.f;

void CRovStatus::drawZero()
{
	//draw zero
	nvgSave(_vg);
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
	nvgArc(_vg, 0, 0, radialRadius, -M_PI / 2 - M_PI * radialLimit / 180, -M_PI / 2 + M_PI * radialLimit / 180, NVG_CW);
	nvgStroke(_vg);

	nvgBeginPath(_vg);
	for (int i = -4; i <= 4; i++) {
		nvgMoveTo(_vg, (radialRadius - tickRadius) * cosf(CC_DEGREES_TO_RADIANS(-90 + i * 15)), (radialRadius - tickRadius) * sinf(CC_DEGREES_TO_RADIANS(-90 + i * 15)));
		nvgLineTo(_vg, radialRadius * cosf(CC_DEGREES_TO_RADIANS(-90 + i * 15)), radialRadius * sinf(CC_DEGREES_TO_RADIANS(-90 + i * 15)));
	}
	nvgStroke(_vg);
	nvgRestore(_vg);
}

void CRovStatus::drawRoll()
{
	//draw roll
	nvgSave(_vg);
	nvgRotate(_vg, float(-_navdata.ROLL));
	nvgFillColor(_vg, nvgRGB(255, 255, 255));
	nvgBeginPath(_vg);
	nvgMoveTo(_vg, 0, -radialRadius);
	nvgLineTo(_vg, -5, -radialRadius + 10);
	nvgLineTo(_vg, +5, -radialRadius + 10);
	nvgClosePath(_vg);
	nvgFill(_vg);

	int readableRollAngle = int(round(float(_navdata.ROLL))) % 360;
	if (readableRollAngle > 180) {
		readableRollAngle = readableRollAngle - 360;
	}

	nvgBeginPath(_vg);
	nvgRect(_vg, -20, -radialRadius + 9, 40, 16);
	nvgFill(_vg);

	nvgFontFaceId(_vg, _font_face);
	nvgFontSize(_vg, 12);
	nvgTextAlign(_vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgFontBlur(_vg, 0);
	nvgFillColor(_vg, nvgRGB(0, 0, 0));
	char buf[16]{0};
	nvgText(_vg, 0, -radialRadius + 18, _itoa(readableRollAngle, buf, 10), NULL);
	nvgRestore(_vg);
}

void CRovStatus::drawSpeed()
{
	float yellowBoundaryY, redBoundaryY, yOffset, from, to;
	int i, j;
	nvgSave(_vg);
	
	nvgTranslate(_vg,-zeroWidth  - zeroPadding - speedIndicatorWidth, 0);
	nvgFillColor(_vg,nvgRGBAf(.0f,0,0,speedAltOpacity));
	nvgStrokeColor(_vg,nvgRGB(255,255,255));
	nvgStrokeWidth(_vg,2);
	nvgBeginPath(_vg);
	nvgRect(_vg,0, -speedIndicatorHeight / 2, speedIndicatorWidth, speedIndicatorHeight);
	nvgStroke(_vg);
	nvgFill(_vg);
	nvgRestore(_vg);
	//nvgRect(_vg,0, -speedIndicatorHeight / 2, speedIndicatorWidth, speedIndicatorHeight);

	nvgSave(_vg);
	nvgTranslate(_vg,-zeroWidth - zeroPadding - speedIndicatorWidth, 0);
	nvgScissor(_vg,0, -speedIndicatorHeight / 2, speedIndicatorWidth, speedIndicatorHeight);
	yellowBoundaryY = -(-_navdata.FTHR + yellowBoundarySpeed) / 10 * pixelsPer10Kmph;
	redBoundaryY = -(-_navdata.FTHR + redBoundarySpeed) / 10 * pixelsPer10Kmph;

	nvgFillColor(_vg, nvgRGB(255, 255, 0)); // 'yellow';
	nvgBeginPath(_vg);
	nvgRect(_vg,speedIndicatorWidth - speedWarningWidth, yellowBoundaryY, speedWarningWidth, redBoundaryY - yellowBoundaryY);
	nvgFill(_vg);

	nvgFillColor(_vg, nvgRGB(255, 0, 0)); // 'yellow';
	nvgBeginPath(_vg);
	nvgRect(_vg,speedIndicatorWidth - speedWarningWidth, redBoundaryY, speedWarningWidth, -speedIndicatorHeight / 2 - redBoundaryY);
	nvgFill(_vg);

	nvgFillColor(_vg, nvgRGB(0, 255, 0));
	nvgBeginPath(_vg);
	nvgRect(_vg,speedIndicatorWidth - speedWarningWidth, yellowBoundaryY, speedWarningWidth, +speedIndicatorHeight / 2 - yellowBoundaryY);
	nvgFill(_vg);

	yOffset = _navdata.FTHR / 10 * pixelsPer10Kmph;
	// The unclipped ticks to be rendered.
	// We render 100kmph either side of the center to be safe
	nvgFontFaceId(_vg, _font_face);
	nvgFontSize(_vg, 12);
	nvgTextAlign(_vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);
	nvgFontBlur(_vg, 0);
	nvgFillColor(_vg, nvgRGB(255, 255, 255));

	from = -floor(_navdata.FTHR / 10) - 10;
	to = ceil(_navdata.FTHR / 10) + 10;
	nvgStrokeColor(_vg,nvgRGB(255,255,255));
	nvgStrokeWidth(_vg,2);
	for (i = from; i < to; ++i) {
		nvgBeginPath(_vg);
		nvgMoveTo(_vg,speedIndicatorWidth - speedWarningWidth, -i * pixelsPer10Kmph + yOffset);
		nvgLineTo(_vg,speedIndicatorWidth - speedWarningWidth - majorTickWidth, -i * pixelsPer10Kmph + yOffset);
		for (j = 1; j < minorTicksPer10Kmph; ++j) {
			nvgMoveTo(_vg,speedIndicatorWidth - speedWarningWidth, -i * pixelsPer10Kmph - j * pixelsPer10Kmph / minorTicksPer10Kmph + yOffset);
			nvgLineTo(_vg,speedIndicatorWidth - speedWarningWidth - minorTickWidth, -i * pixelsPer10Kmph - j * pixelsPer10Kmph / minorTicksPer10Kmph + yOffset);
		}
		nvgStroke(_vg);

		//nvgBeginPath(_vg);
		
		char buf[16]{0};
		nvgText(_vg, 20, -i * pixelsPer10Kmph + yOffset + 4,_itoa(i*10,buf,10),NULL);
	}

	nvgBeginPath(_vg);
	nvgMoveTo(_vg,speedIndicatorWidth - speedWarningWidth - minorTickWidth, 0);
	nvgLineTo(_vg,speedIndicatorWidth - speedWarningWidth - minorTickWidth * 2, -5);
	nvgLineTo(_vg,speedIndicatorWidth - speedWarningWidth - minorTickWidth * 2, -10);
	nvgLineTo(_vg,0, -10);
	nvgLineTo(_vg,0, 10);
	nvgLineTo(_vg,speedIndicatorWidth - speedWarningWidth - minorTickWidth * 2, 10);
	nvgLineTo(_vg,speedIndicatorWidth - speedWarningWidth - minorTickWidth * 2, 5);
	nvgClosePath(_vg);
	nvgFill(_vg);

	nvgFillColor(_vg,nvgRGB(0,0,0));
	char fl[32]{0};
	sprintf_s<32>(fl, "%.f", round(_navdata.FTHR * 100) / 100);
	nvgText(_vg, 15, 4.5,fl,NULL);
	nvgRestore(_vg);
}

void CRovStatus::drawAltitude()
{
	float yOffset = 0, from, to;
	int i, j;
	nvgSave(_vg);

	nvgTranslate(_vg,zeroWidth + zeroPadding, 0);
	nvgBeginPath(_vg);
	nvgFillColor(_vg, nvgRGBAf(0, 0, 0, speedAltOpacity));
	nvgStrokeColor(_vg, nvgRGB(255, 255, 255));
	nvgStrokeWidth(_vg, 2);
	nvgRect(_vg,0, -altIndicatorHeight / 2, altIndicatorWidth, altIndicatorHeight);
	nvgStroke(_vg);
	nvgFill(_vg);
	nvgRestore(_vg);
	nvgSave(_vg);
	nvgTranslate(_vg,zeroWidth + zeroPadding, 0);
	nvgScissor(_vg,0, -altIndicatorHeight / 2, altIndicatorWidth, altIndicatorHeight);

	nvgStrokeColor(_vg, nvgRGB(255, 255, 255));
	nvgStrokeWidth(_vg, 2);

	yOffset = _navdata.DEAP / 1 * pixelsPer100Ft;
	// The unclipped ticks to be rendered. We render 500ft either side of
	// the center to be safe


	nvgFontFaceId(_vg, _font_face);
	nvgFontSize(_vg, 12);
	nvgTextAlign(_vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);
	nvgFontBlur(_vg, 0);
	nvgFillColor(_vg, nvgRGB(255, 255, 255));

	from = floor(_navdata.DEAP / 1) - 5;
	to = ceil(_navdata.DEAP / 1) + 5;
	for (i = from; i < to; ++i) {
		nvgBeginPath(_vg);
		nvgMoveTo(_vg,0, -i * pixelsPer100Ft + yOffset);
		nvgLineTo(_vg,majorTickWidth, -i * pixelsPer100Ft + yOffset);
		for (j = 1; j < minorTicksPer100Ft; ++j) {
			nvgMoveTo(_vg,0, -i * pixelsPer100Ft - j * pixelsPer100Ft / minorTicksPer100Ft + yOffset);
			nvgLineTo(_vg,minorTickWidth, -i * pixelsPer100Ft - j * pixelsPer100Ft / minorTicksPer100Ft + yOffset);
		}
		nvgStroke(_vg);

		char buf[16]{0};
		nvgText(_vg, 15, -i * pixelsPer100Ft + yOffset + 4,_itoa(i,buf,10),NULL);
	}

	nvgRestore(_vg);
	nvgSave(_vg);
	nvgTranslate(_vg,zeroWidth + zeroPadding, 0);
	nvgStrokeColor(_vg, nvgRGB(255, 255, 255));
	nvgStrokeWidth(_vg, 2);
	nvgFillColor(_vg, nvgRGB(255, 255, 255));
	
	nvgBeginPath(_vg);
	nvgMoveTo(_vg,minorTickWidth, 0);
	nvgLineTo(_vg,minorTickWidth * 2, -5);
	nvgLineTo(_vg,minorTickWidth * 2, -10);
	nvgLineTo(_vg,altIndicatorWidth, -10);
	nvgLineTo(_vg,altIndicatorWidth, 10);
	nvgLineTo(_vg,minorTickWidth * 2, 10);
	nvgLineTo(_vg,minorTickWidth * 2, 5);
	nvgClosePath(_vg);
	nvgFill(_vg);
	
	nvgFillColor(_vg, nvgRGB(0, 0, 0));
	char fl[32]{0};
	sprintf_s<32>(fl, "%.f", round(_navdata.DEAP * 100) / 100);
	nvgText(_vg, 15, 4.5, fl,NULL);
	nvgRestore(_vg);
}

void CRovStatus::drawHorizon()
{
	float pitchPixels, pixelsPerDeg = _contentSize.height / (frontCameraFovY / 2);
	nvgSave(_vg);
	nvgRotate(_vg,-_navdata.ROLL);
	pitchPixels = _navdata.PITC / (M_PI * 2) * 360 * pixelsPerDeg;
	nvgTranslate(_vg,0, pitchPixels);
	/*
	nvgfillStyle = skyColor;
	nvgfillRect(-10000, -10000, 20000, 10000);
	nvgfillStyle = earthColor;
	nvgfillRect(-10000, 0, 20000, 10000);
	*/
	// horizon
	nvgStrokeColor(_vg, nvgRGB(255, 0xff, 255));
	nvgFillColor(_vg, nvgRGB(255, 255, 255));
	nvgStrokeWidth(_vg,2);
	nvgBeginPath(_vg);
	nvgMoveTo(_vg,-_contentSize.width , 0);
	nvgLineTo(_vg,_contentSize.width, 0);
	nvgStroke(_vg);

	nvgScissor(_vg, -_contentSize.width / 2, -_contentSize.height / 2, _contentSize.width, _contentSize.height);

	nvgFontFaceId(_vg, _font_face);
	nvgFontSize(_vg, 12);
	nvgTextAlign(_vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);
	nvgFontBlur(_vg, 0);
		
	char buf[16]{0};
	for (int i = -18; i <= 18; ++i) {
		int pitchAngle = int(i / 2.f * 10);
		if (i != 0) {
			if (i % 2 == 0) {
				nvgBeginPath(_vg);
				nvgMoveTo(_vg,-majorWidth / 2, -pixelsPerDeg * pitchAngle);
				nvgLineTo(_vg,+majorWidth / 2, -pixelsPerDeg * pitchAngle);
				nvgStroke(_vg);
				nvgText(_vg,-majorWidth / 2 - 20, -pixelsPerDeg * 10 / 2 * i,_itoa(pitchAngle,buf,10),NULL);
				nvgText(_vg, majorWidth / 2 + 10, -pixelsPerDeg * 10 / 2 * i, _itoa(pitchAngle, buf, 10), NULL);
			}
			else {
				nvgBeginPath(_vg);
				nvgMoveTo(_vg,-minorWidth / 2, -pixelsPerDeg * pitchAngle);
				nvgLineTo(_vg,+minorWidth / 2, -pixelsPerDeg * pitchAngle);
				nvgStroke(_vg);
			}
		}
	}
	nvgRestore(_vg);
}
