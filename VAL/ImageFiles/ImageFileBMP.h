//====---
// VAL
//----
// Vapour Technology All-Purpose Library
// Copyright 1999 Vapour Technology Ltd.
//
// ImageFileBMP.h - 21/12/1999 - Warren Moore
//	BMP file format image
//
// $Id: ImageFileBMP.h,v 1.4 2000/07/31 14:45:03 waz Exp $
//

#ifndef _VAL_IMAGEFILEBMP_
#define _VAL_IMAGEFILEBMP_

#pragma once

#include "ImageFileStore.h"

#include <fstream.h>

//////////////////
// CImageFileBMP

class CImageFileBMP : public CImageFile {
public:
	CImageFileBMP();
	~CImageFileBMP();

// Exported Functions
	const char *GetFileExtension() const;
	const char *GetFileTitle() const;
	FRESULT GetImageType(const char *pFname, IMAGETYPE &eImgType);
	FRESULT Load(const char *pFname, int &x, int &y, unsigned char *&pData);
	bool CheckSaveType(IMAGETYPE eImgType) const;
	void SetSaveType(IMAGETYPE eImgType);
	void SetPalette(CImagePalette *pPalette);
	FRESULT Save(const char *pFname, const int x, const int y,
		const unsigned char *pData) const;

//#===--- Internal Functions
private:
	FRESULT LoadHeaders();

//#===--- Internal Data
private:
	fstream m_oFile;
	bool m_bTopDown;
	int m_iWidth, m_iHeight;
	IMAGETYPE m_eSaveType;
	CImagePalette *m_pPalette;

// ImageFileProxy Functions
public:
	static const char *GetTitle() {
		return ("Windows Bitmap");
	}

	static const char *GetExtension() {
		return ("bmp");
	}

};

#endif // _VAL_IMAGEFILEBMP_