//#========---
//#=- VALET
//#=====---
// Vapour Technology All-Purpose Library
// Copyright 2001 Vapour Technology Ltd.
//
// Add confidentiality notice

////////////////
//! file 	= "VALET/math/eulerrotation.cpp"
//! author 	= "James Smith"
//! date	= "02/10/2001"
//! lib 	= libVALETmath
//! rcsid 	= "$Id: eulerrotation.cpp,v 1.3 2001/10/24 21:33:25 vap-james Exp $"
//! userlevel	= Normal
//! docentry	= "VALET.Math.Geometry"

#include "VALET/eulerrotation.h"

// VALET includes
#include "VALET/log.h"

// Standard includes
#include <math.h>
#include <float.h>

// defines
#define X_AXIS 0x0
#define Y_AXIS 0x1
#define Z_AXIS 0x2

namespace NVALET {

   CEulerRotation::CEulerRotation() {
      CLog("math","CEulerRotation::Constructor (default)",LL_OBJECT);
      m_tType.m_eID = XYZS;
   } //CEulerRotation()
  
   CEulerRotation::CEulerRotation(const TEulerType& tType) :
      m_tType(tType)
   {
      CLog("math","CEulerRotation::Constructor (TEulerType)",LL_OBJECT);
   } //CEulerRotation(const TEulerType& tType)
  
   CEulerRotation::CEulerRotation(const CVector3D& oAngles, const TEulerType& tType) :
      m_oAngles(oAngles),
      m_tType(tType)
   {
      CLog("math","CEulerRotation::Constructor (CVector3D,TEulerType)",LL_OBJECT);
   } //CEulerRotation(const CVector3D& oAngles, const TEulerType& tType)
  
   CEulerRotation::CEulerRotation(const CHomTransform & oTransform, const TEulerType& tType) : 
      m_tType(tType)
   {
      CLog("math","CEulerRotation::Constructor (CHomTransform,TEulerType)",LL_OBJECT);
      int NextAngle[] = {Y_AXIS, Z_AXIS, X_AXIS, Y_AXIS};
      int i = tType.m_tProperties.m_ucInnerAxis;
      int j = NextAngle[i + tType.m_tProperties.m_bOddParity];
      int k = NextAngle[i + 1 - tType.m_tProperties.m_bOddParity];	
      if (tType.m_tProperties.m_bRepetition) {
         double sy = sqrt(oTransform.Element(i,j)*oTransform.Element(i,j) + oTransform.Element(i,k)*oTransform.Element(i,k));
         if (sy > 16*FLT_EPSILON) {
            m_oAngles.X() = atan2(oTransform.Element(i,j),oTransform.Element(i,k));
            m_oAngles.Y() = atan2(sy, oTransform.Element(i,i));
            m_oAngles.Z() = atan2(oTransform.Element(j,i), -oTransform.Element(k,i));
         }
         else {
            m_oAngles.X() = atan2(-oTransform.Element(j,k), oTransform.Element(j,j));
            m_oAngles.Y() = atan2(sy, oTransform.Element(i,i));
            m_oAngles.Z() = 0;
         }
      }
      else {
         double cy = sqrt(oTransform.Element(i,i)*oTransform.Element(i,i) + oTransform.Element(j,i)*oTransform.Element(j,i));
         if (cy > 16*FLT_EPSILON) {
            m_oAngles.X() = atan2(oTransform.Element(k,j), oTransform.Element(k,k));
            m_oAngles.Y() = atan2(-oTransform.Element(k,i), cy);
            m_oAngles.Z() = atan2(oTransform.Element(j,i), oTransform.Element(i,i));
         }
         else {
            m_oAngles.X() = atan2(-oTransform.Element(j,k), oTransform.Element(j,j));
            m_oAngles.Y() = atan2(-oTransform.Element(k,i), cy);
            m_oAngles.Z() = 0;
         }
      }
      if (tType.m_tProperties.m_bOddParity) {
         m_oAngles.Invert();
      }
      if (tType.m_tProperties.m_bRotatingFrame) {
         double dTemp = m_oAngles.X();
         m_oAngles.X() = m_oAngles.Z();
         m_oAngles.Z() = dTemp;
      }
   } //CEulerRotation(const CHomTransform & oTransform, const TEulerType& tType)
  
   CEulerRotation::CEulerRotation(const CQuaternion& oQuat, const TEulerType& tType) {    
      CLog("math","CEulerRotation::Constructor (CQuaternion,TEulerType)",LL_OBJECT);
      CHomTransform oTransform(oQuat);
      *this = CEulerRotation(oTransform,tType);
   } //CEulerRotation(const CQuaternion& oQuat, const TEulerType& tType)
  
   CEulerRotation::~CEulerRotation() {
      CLog("math","CEulerRotation::Destructor",LL_OBJECT);
   } //~CEulerRotation()
  
   CEulerRotation& CEulerRotation::operator*=(const CVector3D& oVec) {
      CLog("math","CEulerRotation::operator*=",LL_FUNCTION);
      m_oAngles *= oVec;
      return *this;
   } //operator*=(const CVector3D& oVec)
  
   bool CEulerRotation::Limit(const CVector3D& oMax, const CVector3D& oMin) {
      CLog("math","CEulerRotation::Limit",LL_FUNCTION);
      bool bReturn = false;
      if (m_oAngles.X() > oMax.X()) {
         m_oAngles.X() = oMax.X(); 
         bReturn = true;
      }
      else if (m_oAngles.X() < oMin.X()) {
         m_oAngles.X() = oMin.X();
         bReturn = true;
      }
      if (m_oAngles.Y() > oMax.Y()) {
         m_oAngles.Y() = oMax.Y();
         bReturn = true;
      }
      else if (m_oAngles.Y() < oMin.Y()) {
         m_oAngles.Y() = oMin.Y();
         bReturn = true;
      }
      if (m_oAngles.Z() > oMax.Z()) {
         m_oAngles.Z() = oMax.Z();
         bReturn = true;
      }
      else if (m_oAngles.Z() < oMin.Z()) {
         m_oAngles.Z() = oMin.Z();
         bReturn = true;
      }
      return bReturn;
   } //Limit(const CVector3D& oMax, const CVector3D& oMin) 

   CVector3D CEulerRotation::Angles(void) const {
      CLog("math","CEulerRotation::Angles (const)",LL_FUNCTION);
      return m_oAngles;
   }
    
   CVector3D& CEulerRotation::Angles(void) {
      CLog("math","CEulerRotation::Angles",LL_FUNCTION);
      return m_oAngles;
   }
    
   CEulerRotation::TEulerType CEulerRotation::Type(void) const {
      CLog("math","CEulerRotation::Type (const)",LL_FUNCTION);
      return m_tType;
   }
    
   CEulerRotation::TEulerType& CEulerRotation::Type(void) {
      CLog("math","CEulerRotation::Type",LL_FUNCTION);
      return m_tType;
   }
 
}

