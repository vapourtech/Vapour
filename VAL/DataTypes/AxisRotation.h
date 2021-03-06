//====---
// VAL
//----
// Vapour Technology All-Purpose Library
// Copyright 2000 Vapour Technology Ltd.
//
// AxisRotation.h - 12/04/2000 - James Smith
//	Axis-Angle rotation header
//
// $Id: AxisRotation.h,v 1.4 2000/11/21 16:38:24 waz Exp $
//

#ifndef _VAL_AXISROTATION_
#define _VAL_AXISROTATION_

#pragma once

class CAxisRotation;

// libVapour includes
#include "Vector3D.h"
#include "EulerRotation.h"
#include "Quaternion.h"

// DLL import/export definitions
#ifndef DLL
	#ifdef VAL_DLL_EXPORT
		#define DLL __declspec(dllexport)
	#endif
	#ifdef VAL_DLL_IMPORT
		#define DLL __declspec(dllimport)
	#endif
	#ifndef DLL
		#define DLL
	#endif
#endif

class DLL CAxisRotation {
public:
	// Construction & Destruction functions ///////////////////////////////

	// Constructor - creates an null rotation
	CAxisRotation();

	// Constructor - creates a rotation from double values
	CAxisRotation(const double & dX, const double & dY, const double & dZ, const double & dR);

	// Constructor - creates a rotation from a vector and a double rotation value
	CAxisRotation(const CVector3D & oVec, const double & dRot);

	// Constructor - creates a rotation from a quaternion
	CAxisRotation(const CQuaternion & oQuat);

	// Constructor - creates a rotation from an Euler rotation
	CAxisRotation(const CEulerRotation & oEuler);

   // Copy Constructor
	CAxisRotation(const CAxisRotation & oRot);

	// Operator functions /////////////////////////////////////////////////

	// Assignment
	CAxisRotation& operator=(const CAxisRotation& oRot);

	// Equality
   bool operator ==(const CAxisRotation& oRot) const;

	// Geometric functions ////////////////////////////////////////////////

	// Multiplies two rotations together
	CAxisRotation MergeInside(CAxisRotation & oRot) const;

	// Multiplies two rotations together - in the opposite order
	CAxisRotation MergeOutside(CAxisRotation & oRot) const;

	// Normalises the vector of rotation
	CAxisRotation Normalise(void) const;

	// Input functions ///////////////////////////////////////////////////.

	// Sets the rotation from a set of double values.
   void FromDouble(double dX, double dY, double dZ, double dR);

	// Sets the rotation angle
   void SetAngle(double dR);

   // Parses a string anc converts it into an axis rotation
	// The string must be only numbers in the format <X> <Y> <Z> <R>
	bool ParseString(const char* strInput, int* used = NULL);

	// Output functions ///////////////////////////////////////////////////

	// Converts the rotation to a string with maximum precision specified by <precision>
	char* ToString(int precision) const;

	// Converts the rotation to a set of double values
	void ToDouble(double& dX, double& dY, double& dZ, double& dR) const;

	// Access functions ///////////////////////////////////////////////////

	// Returns the rotation axis
   CVector3D GetAxis(void) const {return m_oAxis;}

	// Returns the rotation angle
	double GetAngle(void) const {return m_dRotation;}

protected:

 	// The angle to rotate
	double m_dRotation;
	
	// The axis about which to rotate
	CVector3D m_oAxis;

};


#endif // _VAL_AXISROTATION_
