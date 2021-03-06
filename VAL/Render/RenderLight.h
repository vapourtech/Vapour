//====---
// VAL
//----
// Vapour Technology All-Purpose Library
// Copyright 2000 Vapour Technology Ltd.
//
// RenderLight.h - 30/07/2000 - Warren Moore
//	Light render object
//
// $Id: RenderLight.h,v 1.2 2000/11/25 22:35:05 waz Exp $
//

#ifndef _VAL_RENDERLIGHT_
#define _VAL_RENDERLIGHT_

#pragma once

//#===--- Includes
#include "VAL.h"
#include "RenderObject.h"

//#===--- Defines

/////////////////
// CRenderLight

class CRenderLight : public CRenderObject {
public:
	CRenderLight(CRenderContext *poContext);
	virtual ~CRenderLight();

	//#===--- Initialisation/Shutdown

	//#===--- Settings

	//#===--- Render
	// Executes the object function
	// NB: Resets the projeciton matrix
	void Execute();

//#===--- Internal Functions
protected:

//#===--- Internal Data
protected:
	unsigned int m_uMode;
};

//#===--- Inline Functions

#endif // _VAL_RENDERLIGHT_
