//=============---
// Cortona Base
//-------------
// Base application in which to develop specific Cortona functionality
// Copyright 2000 Vapour Technology Ltd.
//
// CortonaUtil.h
// 07/03/2002 - Warren Moore
//
// $Id: CortonaUtil.h,v 1.6 2002/03/21 23:03:14 vap-warren Exp $

#ifndef __CORTONAUTIL__
#define __CORTONAUTIL__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CortonaNode.h"
#include "CortonaField.h"

// Automation interfaces for Cortona
#include "shelley.h"

class CCortonaUtil {
//#===--- Construction/Destruction
public:
   CCortonaUtil(IEngine *pEngine);
   // Constructor must be called with a valid IEngine interface
   ~CCortonaUtil();

//#===--- Member Functions

   CCortonaNode *CreateVrmlFromString(const char *pcVrml);
   // Create a VRML node from a string, returns NULL if failed

   bool AddToScene(const CCortonaNode &oNode);
   // Add a node to the list of root nodes

   CCortonaNode *GetNode(const char *pcName);
   // Get a node by it's DEF'd name, returns NULL if failed

   bool AddRoute(const CCortonaNode &oSrcNode, const char *pcSrcField,
                 const CCortonaNode &oDstNode, const char *pcDstField);
   // Add a route from the src node's field to the destination node's field

   CCortonaField *CreateField(const char *pcType);
   // Create an empty field, returns NULL if failed

//#===--- Member Variables
protected:
   IEngine *m_pEngine;
   // Cortona Engine interface

};

#endif // __CORTONAUTIL__