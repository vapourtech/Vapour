//===================================---
// VT Structure Visualisation Control
//-----------------------------------
// ActiveX Control for structure visualisation
// Copyright 2002 Vapour Technology Ltd.
//
//! docentry  = "VTStrucVis.Visualisation Control.ActiveX Control"
//! userlevel =  Normal
//! file      = "Control/UIDataPath.cpp"
//! author    = "Warren Moore"
//! date      = "27/3/2002"
//! rcsid     = "$Id: UIDataPath.cpp,v 1.5 2002/04/04 11:01:37 vap-warren Exp $"

#include "stdafx.h"
#include "vtstrucvis.h"
#include "UIDataPath.h"

#include "VTStrucVisCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////
// CUIDataPath

CUIDataPath::CUIDataPath() :
   m_uiDataRead(0) {
}

CUIDataPath::~CUIDataPath() {
}

#if 0
BEGIN_MESSAGE_MAP(CUIDataPath, CDataPathProperty)
	//{{AFX_MSG_MAP(CUIDataPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////
// Message Handlers

void CUIDataPath::OnDataAvailable(DWORD dwSize, DWORD bscfFlag) {
   // First call, so signal file loading
   if (bscfFlag & BSCF_FIRSTDATANOTIFICATION) {
      ((CVTStrucVisCtl*)GetControl())->UILoading();
      m_uiDataRead = 0;
   }

   // Sort out streaming data
   DWORD uiArriving = dwSize - m_uiDataRead;
   if (uiArriving > 0) {
      // Update read so far
      m_uiDataRead = dwSize;
   }

   // Last call, so signal file loaded
   if (bscfFlag & BSCF_LASTDATANOTIFICATION) {
      ((CVTStrucVisCtl*)GetControl())->UILoaded();
   }
	
	CDataPathProperty::OnDataAvailable(dwSize, bscfFlag);
}

