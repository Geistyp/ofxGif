#include "ofxGif.h"

#include "FreeImage.h"

namespace ofxGIF
{
	/// Pointer to a multi-page file stream
	FIMULTIBITMAP *gif = NULL;

	//----------------------------------------------------------
	// static variable for freeImage initialization:
	void ofInitFreeImage(bool deinit = false){
		// need a new bool to avoid c++ "deinitialization order fiasco":
		// http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.15
		static bool	* bFreeImageInited = new bool(false);
		if (!*bFreeImageInited && !deinit){
			FreeImage_Initialise();
			*bFreeImageInited = true;
		}
		if (*bFreeImageInited && deinit){
			FreeImage_DeInitialise();
		}
	}

	/*
	 *	Load file from disk
	 *		: not support png file right
	 */
	FIBITMAP* loadImage(string fileName)
	{
		ofxGIF::ofInitFreeImage();

		fileName = ofToDataPath(fileName);
		bool bLoaded = false;
		FIBITMAP * bmp = NULL;

		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		fif = FreeImage_GetFileType(fileName.c_str(), 0);
		if (fif == FIF_UNKNOWN) {
			// or guess via filename
			fif = FreeImage_GetFIFFromFilename(fileName.c_str());
		}
		if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
			bmp = FreeImage_Load(fif, fileName.c_str(), 0);

			if (bmp != NULL){
				bLoaded = true;
			}
		}

		// this will create a 256-color palette from the image for gif
		FIBITMAP* convertedBmp;
		convertedBmp = FreeImage_ColorQuantize(bmp, FIQ_NNQUANT);

		FreeImage_Unload(bmp);

		return convertedBmp;
	}

	//----------------------------------------------------
	FIBITMAP* getBmpFromPixels(ofPixels &pix){

		pix.swapRgb();			// should add this

		unsigned char* pixels = pix.getPixels();
		unsigned int width = pix.getWidth();
		unsigned int height = pix.getHeight();
		unsigned int bpp = pix.getBitsPerPixel();

		FREE_IMAGE_TYPE freeImageType = FIT_BITMAP;		// not need to check, just use unsigned char
		FIBITMAP* bmp = FreeImage_AllocateT(freeImageType, width, height, bpp);
		unsigned char* bmpBits = FreeImage_GetBits(bmp);
		if (bmpBits != NULL) {
			int srcStride = width * pix.getBytesPerPixel();
			int dstStride = FreeImage_GetPitch(bmp);
			unsigned char* src = (unsigned char*)pixels;
			unsigned char* dst = bmpBits;
			for (int i = 0; i < (int)height; i++) {
				memcpy(dst, src, srcStride);
				src += srcStride;
				dst += dstStride;
			}
		}
		else {
			ofLogError("ofImage") << "getBmpFromPixels(): unable to get FIBITMAP from ofPixels";
		}

		// ofPixels are top left, FIBITMAP is bottom left
		FreeImage_FlipVertical(bmp);

		// this will create a 256-color palette from the image for gif
		FIBITMAP* convertedBmp;
		convertedBmp = FreeImage_ColorQuantize(bmp, FIQ_NNQUANT);

		FreeImage_Unload(bmp);

		return convertedBmp;
	}

	//----------------------------------------------------
	void putBmpIntoPixels(FIBITMAP * bmp, ofPixels &pix, bool swapForLittleEndian = true) {
		// some images use a palette, or <8 bpp, so convert them to raster 8-bit channels
		FIBITMAP* bmpConverted = NULL;
		if (FreeImage_GetColorType(bmp) == FIC_PALETTE || FreeImage_GetBPP(bmp) < 8) {
			if (FreeImage_IsTransparent(bmp)) {
				bmpConverted = FreeImage_ConvertTo32Bits(bmp);
			}
			else {
				bmpConverted = FreeImage_ConvertTo24Bits(bmp);
			}
			bmp = bmpConverted;
		}

		unsigned int width = FreeImage_GetWidth(bmp);
		unsigned int height = FreeImage_GetHeight(bmp);
		unsigned int bpp = FreeImage_GetBPP(bmp);
		unsigned int channels = (bpp / sizeof(unsigned char)) / 8;
		unsigned int pitch = FreeImage_GetPitch(bmp);

		// ofPixels are top left, FIBITMAP is bottom left
		FreeImage_FlipVertical(bmp);

		unsigned char* bmpBits = FreeImage_GetBits(bmp);
		if (bmpBits != NULL) {
			pix.setFromAlignedPixels((unsigned char*)bmpBits, width, height, channels, pitch);
		}
		else {
			ofLogError("ofImage") << "putBmpIntoPixels(): unable to set ofPixels from FIBITMAP";
		}

		if (bmpConverted != NULL) {
			FreeImage_Unload(bmpConverted);
		}

#ifdef TARGET_LITTLE_ENDIAN
		if (swapForLittleEndian && sizeof(unsigned char) == 1) {
			pix.swapRgb();
		}
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	void fiGifSaver::create(string filename)
	{
		//filename = ofToDataPath(filename);
		FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename.c_str());
		gif = FreeImage_OpenMultiBitmap(fif, filename.c_str(), TRUE, FALSE, TRUE);
	}

	void fiGifSaver::save()
	{
		if (gif)
		{
			FreeImage_CloseMultiBitmap(gif);
			gif = NULL;
		}
	}

	void fiGifSaver::append(string filename)
	{
		if (gif)
		{
			FIBITMAP* page = ofxGIF::loadImage(filename);
			FreeImage_AppendPage(gif, page);
			FreeImage_Unload(page);
		}
	}

	void fiGifSaver::append(ofPixels& pixels)
	{
		if (gif)
		{
			FIBITMAP* page = ofxGIF::getBmpFromPixels(pixels);
			FreeImage_AppendPage(gif, page);
			FreeImage_Unload(page);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	void fiGifLoader::load(string filename)
	{
		ofxGIF::ofInitFreeImage();
		filename = ofToDataPath(filename);
		FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename.c_str());
		gif = FreeImage_OpenMultiBitmap(fif, filename.c_str(), FALSE, TRUE, TRUE);

		// Get src page count   
		int count = FreeImage_GetPageCount(gif);

		// Clone src to dst   
		for (int page = 0; page < count; page++) {
			// Load the bitmap at position 'page'   
			FIBITMAP *dib = FreeImage_LockPage(gif, page);
			if (dib) {
				
				pages.push_back(ofImage());
				ofxGIF::putBmpIntoPixels(dib, pages.back().getPixelsRef());
				// Unload the bitmap (do not apply any change to src)   
				FreeImage_UnlockPage(gif, dib, FALSE);
			}
		}

		// Close src   
		FreeImage_CloseMultiBitmap(gif, 0);

		// wired, sometimes last page get empty, check all
		for (int i = 0; i < pages.size(); i++)
		{
			if (pages[i].getWidth() == 0) pages.erase(pages.begin() + i--); 
		}
	}
};



