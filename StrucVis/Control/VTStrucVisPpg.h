//===================================---
// VT Structure Visualisation Control
//-----------------------------------
// ActiveX Control for structure visualisation
// Copyright 2002 Vapour Technology Ltd.
//
//! docentry  = "VTStrucVis.Visualisation Control.ActiveX Control"
//! userlevel =  Normal
//! file      = "Control/VTStrucPpg.h"
//! author    = "Warren Moore"
//! date      = "5/3/2002"
//! rcsid     = "$Id: VTStrucVisPpg.h,v 1.4 2002/04/04 11:01:39 vap-warren Exp $"

#ifndef __VTSTRUCTVIS_PROPPAGE__
#define __VTSTRUCTVIS_PROPPAGE__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////
// CVTStrucVisPropPage

class CVTStrucVisPropPage : public COlePropertyPage {
// Dynamic construction
	DECLARE_DYNCREATE(CVTStrucVisPropPage)
	DECLARE_OLECREATE_EX(CVTStrucVisPropPage)
public:
	CVTStrucVisPropPage();

//#===--- Windows Mappings

// Dialog Data
	//{{AFX_DATA(CVTStrucVisPropPage)
	enum { IDD = IDD_PROPPAGE_VTSTRUCVIS };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTStrucVisPropPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}

#endif // __VTSTRUCTVIS_PROPPAGE__
