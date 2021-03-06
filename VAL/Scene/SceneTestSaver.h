//====---
// VAL
//----
// Vapour Technology All-Purpose Library
// Copyright 2000 Vapour Technology Ltd.
//
// SceneAvatarSaver.h - 27/11/2000 - Warren Moore
//	  Scene rendering class for avatar screen saver
//
// $Id: SceneTestSaver.h,v 1.2 2000/12/03 13:24:25 warren Exp $
//

#ifndef _VAL_SCENEAVATARSAVER_
#define _VAL_SCENEAVATARSAVER_

#pragma once

#include "VAL.h"

#include "Scene.h"
#include "Image.h"
#include "Avatar.h"
#include "Wedgie.h"

//#===--- Predeclared classes
class CRenderAvatar;
class CRenderLight;
class CRenderCamera;
class CAvatarPose;

//#===--- Defines

#define SC_MAX_POSES			50

//#===--- Data types

////////////////////
// CSceneTestSaver

class CSceneTestSaver : public CScene {
public:
	CSceneTestSaver(CDisplayContext *poDisplay);
	virtual ~CSceneTestSaver();

//#===--- External Functions
	//#===--- Display Management
	// Creates the scene
	SCRESULT Create();
	// Destroys the scene
	SCRESULT Destroy();
	// Creates the image from the supplied avatar
	SCRESULT Render();
	// Returns an image of the previously rendered avatar
	SCRESULT Snapshot(CImage *&poImage);
	// Sets the window size
	SCRESULT SetSize(int iWidth, int iHeight);
	// Sets the window size with offset
	SCRESULT SetSize(int iWidth, int iHeight, int iWidthOffset, int iHeightOffset);

	//#===--- Data Import/Export
	// Imports an avatar (reference used so calling function must delete avatar)
	SCRESULT ImportAvatar(CAvatar *poAvatar, bool bGeneric = true);
	// Imports a pose for animation
	SCRESULT ImportPose(CAvatarPose &oPose);

	//#===--- Control

protected:
//#===--- Internal Structures

//#===--- Internal Functions

//#===--- Internal Data
	CAvatar *m_poAvatar;								// Avatar pointer
	bool m_bGeneric;									// Generic avatar indicator

	//#===--- Render Objects
	CRenderAvatar *m_poRAvatar;					// Avatar renderer
	CRenderLight *m_poRLight;						// Light renderer
	CRenderCamera *m_poRCamera;					// Renderer camera

	//#===--- State variables
	int m_iPoses;										// Number of poses imported
	int m_iCurPose;									// Current pose
	CAvatarPose *m_ppPose[SC_MAX_POSES];		// Array of imported poses
};

//#===--- Inline Functions

#endif // _VAL_SCENEAVATARSAVER_
