//====---
// VAL
//----
// Vapour Technology All-Purpose Library
// Copyright 2000 Vapour Technology Ltd.
//
// BoxFilter.h - 29/03/2000 - Warren Moore
//	Box filter class declartion
//
// $Id: BoxFilter.h,v 1.1 2000/06/16 21:59:41 waz Exp $
//

#ifndef _BOXFILTER_
#define _BOXFILTER_

#pragma once

#include "BaseFilter.h"

#include <math.h>

///////////////
// CBoxFilter

class CBoxFilter : public CBaseFilter {
//#===--- Functions
public:
	CBoxFilter(double dWidth = 0.5) : CBaseFilter(dWidth) {};
	~CBoxFilter() {};

	double Filter(double dVal);
};

//#===--- Inline Functions

inline double CBoxFilter::Filter(double dVal) {
	return (fabs(dVal) <= m_dWidth ? 1.0 : 0.0);
} // Filter

#endif // _BOXFILTER_

