//#========---
//#=- VALET
//#=====---
// Vapour Technology All-Purpose Library
// Copyright 2001 Vapour Technology Ltd.
//
// Add confidentiality notice

////////////////
//! file 		= "VALET/3d/material.cpp"
//! author 		= "James Smith"
//! date 		= "09/10/2001"
//! lib 		= libVALET3d
//! rcsid 		= "$Id: material.cpp,v 1.1 2001/10/09 23:19:54 vap-james Exp $"
//! userlevel 	        = Normal
//! docentry 	        = "VALET.3d"

#include "material.h"

// VALET includes

// Standard includes

namespace NValet {
  
  // Creates a surface material
  CMaterial::CMaterial() :
    m_cAmbient(0.4F, 0.4F, 0.4F, 1.0F),
    m_cDiffuse(0.9F, 0.9F, 0.9F, 1.0F),
    m_cSpecular(0.0F, 0.0F, 0.0F, 1.0F),
    m_cEmissive(0.0F, 0.0F, 0.0F, 0.0F),
    m_fShininess(0.0F)
  {
  }
  
  // Destructor
  CMaterial::~CMaterial() {
  }
  
}


