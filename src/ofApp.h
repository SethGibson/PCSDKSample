#ifndef __OFAPP_H__
#define __OFAPP_H__

#include "ofMain.h"
#include "util_pipeline.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void getDepthMap();
	void draw();
	void exit();

private:
	ofTexture mTexSeg, mTexLabel, mTexDepth, mTexRgb;
	pxcU16 *mDepthBuffer;
	unsigned char *mDepthPixels;

	UtilPipeline mPXC;
	pxcU32 *mDimsRgb;
	pxcU32 *mDimsDepth;
};

#endif