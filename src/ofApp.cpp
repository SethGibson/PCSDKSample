#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowShape(640,480);
	ofSetFrameRate(30);

	mPXC.EnableImage(PXCImage::COLOR_FORMAT_RGB24);
	mPXC.EnableImage(PXCImage::COLOR_FORMAT_DEPTH);
	mPXC.EnableGesture();
	mPXC.EnableSegmentation();
	mPXC.Init();

	mDimsRgb = new pxcU32[2];
	mDimsDepth = new pxcU32[2];
	mPXC.QueryImageSize(PXCImage::IMAGE_TYPE_COLOR, mDimsRgb[0], mDimsRgb[1]);
	mPXC.QueryImageSize(PXCImage::IMAGE_TYPE_DEPTH, mDimsDepth[0], mDimsDepth[1]);

	mTexRgb.allocate(mDimsRgb[0], mDimsRgb[1], GL_RGB);
	mTexDepth.allocate(mDimsDepth[0],mDimsDepth[1],GL_RGB);
	mTexSeg.allocate(mDimsDepth[0],mDimsDepth[1],GL_LUMINANCE);
	mTexLabel.allocate(mDimsDepth[0],mDimsDepth[1],GL_LUMINANCE);
	mDepthBuffer = new pxcU16[mDimsDepth[0]*mDimsDepth[1]];
	mDepthPixels = new unsigned char[mDimsDepth[0]*mDimsDepth[1]*3];
}

//--------------------------------------------------------------
void ofApp::update()
{
	if(mPXC.AcquireFrame(true))
	{
		//Structures for image data retrieval
		PXCImage *cImage = mPXC.QueryImage(PXCImage::IMAGE_TYPE_COLOR);
		PXCImage::ImageData cData;

		if(cImage->AcquireAccess(PXCImage::ACCESS_READ, &cData)>=PXC_STATUS_NO_ERROR)
		{
			mTexRgb.loadData(cData.planes[0],mDimsRgb[0],mDimsRgb[1],GL_BGR);
			cImage->ReleaseAccess(&cData);
		}

		cImage = mPXC.QueryImage(PXCImage::IMAGE_TYPE_DEPTH);
		if(cImage->AcquireAccess(PXCImage::ACCESS_READ, &cData)>=PXC_STATUS_NO_ERROR)
		{
			memcpy(mDepthBuffer, cData.planes[0], (size_t)(mDimsDepth[0]*mDimsDepth[1]*sizeof(pxcU16)));
			cImage->ReleaseAccess(&cData);
		}
		
		cImage = mPXC.QuerySegmentationImage();
		if(cImage->AcquireAccess(PXCImage::ACCESS_READ, &cData)>=PXC_STATUS_NO_ERROR)
		{
			mTexSeg.loadData(cData.planes[0],mDimsDepth[0],mDimsDepth[1],GL_LUMINANCE);
			cImage->ReleaseAccess(&cData);
		}

		PXCGesture *cGesture = mPXC.QueryGesture();
		if(cGesture->QueryBlobImage(PXCGesture::Blob::LABEL_SCENE,0,&cImage)>=PXC_STATUS_NO_ERROR)
		{
			if(cImage->AcquireAccess(PXCImage::ACCESS_READ, &cData)>=PXC_STATUS_NO_ERROR)
			{
				mTexLabel.loadData(cData.planes[0],mDimsDepth[0],mDimsDepth[1],GL_LUMINANCE);
				cImage->ReleaseAccess(&cData);
			}
		}
		mPXC.ReleaseFrame();
	}
	getDepthMap();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	mTexRgb.draw(0,0,320,240);
	mTexDepth.draw(320,0,320,240);
	mTexLabel.draw(0,240,320,240);
	mTexSeg.draw(320,240,320,240);
}

//--------------------------------------------------------------
void ofApp::getDepthMap()
{
	for(int i=0;i<mDimsDepth[0]*mDimsDepth[1];++i)
	{
		float cVal = (float)mDepthBuffer[i];
		mDepthPixels[i*3]=0;
		mDepthPixels[i*3+1]=0;
		mDepthPixels[i*3+2]=0;
		if(cVal<32000)
		{
			char cColor = (char)ofMap(cVal,0,2000,255,0);
			mDepthPixels[i*3]=cColor;
			mDepthPixels[i*3+1]=cColor;
			mDepthPixels[i*3+2]=cColor;
		}
	}
	mTexDepth.loadData(mDepthPixels,mDimsDepth[0],mDimsDepth[1],GL_RGB);
}

//--------------------------------------------------------------
void ofApp::exit()
{
	mPXC.Close();
	delete [] mDepthBuffer;
	delete [] mDepthPixels;
	delete [] mDimsDepth;
	delete [] mDimsRgb;
}
