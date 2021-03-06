//====---
// VAL
//----
// Vapour Technology All-Purpose Library
// Copyright 2000 Vapour Technology Ltd.
//
// BodyPart.cpp - 12/06/2000 - James Smith
//	Body Part information structures and types
//
// $Id: BodyPart.cpp,v 1.3 2000/12/02 07:32:17 warren Exp $
//

#include "stdafx.h"

#include "BodyPart.h"
#include "MathConstants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SBodyPart::SBodyPart() :
   m_bDirtyAngle(false),
	m_bDirtyShape(false),
	m_iNumVertices(0),
   m_pliVertices(NULL),
	m_piVertices(NULL),
	m_iNumFaces(0),
   m_pliFaces(NULL),
	m_piFaces(NULL),
	m_dMaxX(V_PI),
	m_dMaxY(V_PI),
	m_dMaxZ(V_PI),
	m_dMinX(V_MINUS_PI),
	m_dMinY(V_MINUS_PI),
	m_dMinZ(V_MINUS_PI),
	m_dDampX(1),
	m_dDampY(1),
	m_dDampZ(1),
   m_bpParent(unknown),
	m_iMaterialIndex(-1)
{
   m_bpChildren[0] = unknown;
   m_bpChildren[1] = unknown;
   m_bpChildren[2] = unknown;
	return;
} //SBodyPart()

// Destructor
SBodyPart::~SBodyPart() {
	if (m_piVertices != NULL) delete [] m_piVertices;
	if (m_piFaces != NULL) delete [] m_piFaces;
} //~SBodyPart()

const char* SBodyPart::Names[] = {
	"root",
	"sacroiliac",
	"l_hip",
	"l_knee",
	"l_ankle",
	"l_subtalar",
	"l_midtarsal",
	"l_metatarsal",
	"r_hip",
	"r_knee",
	"r_ankle",
	"r_subtalar",
	"r_midtarsal",
	"r_metatarsal",
	"vl5",
	"vl4",
	"vl3",
	"vl2",
	"vl1",
	"vt12",
	"vt11",
	"vt10",
	"vt9",
	"vt8",
	"vt7",
	"vt6",
	"vt5",
	"vt4",
	"vt3",
	"vt2",
	"vt1",
	"vc7",
	"vc6",
	"vc5",
	"vc4",
	"vc3",
	"vc2",
	"vc1",
	"skullbase",
	"l_eyeball_joint",
	"r_eyeball_joint",
	"l_sternoclavicular",
	"l_acromioclavicular",
	"l_shoulder",
	"l_elbow",
	"l_wrist",
	"l_thumb1",
	"l_thumb2",
	"l_thumb3",
	"l_index0",
	"l_index1",
	"l_index2",
	"l_index3",
	"l_middle0",
	"l_middle1",
	"l_middle2",
	"l_middle3",
	"l_ring0",
	"l_ring1",
	"l_ring2",
	"l_ring3",
	"l_pinky0",
	"l_pinky1",
	"l_pinky2",
	"l_pinky3",
	"r_sternoclavicular",
	"r_acromioclavicular",
	"r_shoulder",
	"r_elbow",
	"r_wrist",
	"r_thumb1",
	"r_thumb2",
	"r_thumb3",
	"r_index0",
	"r_index1",
	"r_index2",
	"r_index3",
	"r_middle0",
	"r_middle1",
	"r_middle2",
	"r_middle3",
	"r_ring0",
	"r_ring1",
	"r_ring2",
	"r_ring3",
	"r_pinky0",
	"r_pinky1",
	"r_pinky2",
	"r_pinky3",
   "l_foot_tip",
   "r_foot_tip",
   "skull_tip",
   "l_hand_tip",
   "r_hand_tip"
};