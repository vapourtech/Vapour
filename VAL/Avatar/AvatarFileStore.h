//====---
// VAL
//----
// Vapour Technology All-Purpose Library
// Copyright 2000 Vapour Technology Ltd.
//
// AvatarFileStore.h - 16/03/2000 - James Smith
//	Avatar import/export filter speciality store header
//
// $Id: AvatarFileStore.h,v 1.2 2000/06/17 10:42:10 waz Exp $
//

#pragma once

#ifndef _VAL_AVATARFILESTORE_
#define _VAL_AVATARFILESTORE_

#include <stdio.h>
#include <vector>

#include "AvatarFile.h"

// DLL import/export definitions
#ifndef DLL_IMP_EXP
	#ifdef _EXP_VAPOUR_COMMON_DLL_
		#define DLL_IMP_EXP __declspec(dllexport)
	#endif
	#ifdef _IMP_VAPOUR_COMMON_DLL_
		#define DLL_IMP_EXP __declspec(dllimport)
	#endif
	#ifndef DLL_IMP_EXP
		#define DLL_IMP_EXP
	#endif
#endif

// Predeclare CAvatarFileProxyBase - do not include here
class CAvatarFileProxyBase;

// Vector and Iterator Typedef
typedef std::vector<const CAvatarFileProxyBase*> AvatarFileVector;
typedef std::vector<const CAvatarFileProxyBase*>::iterator AvatarFileIterator;

// Predicate function for comparing filter names. Returns true if pFilterOne < pFilterTwo.
bool CompareFilters(const CAvatarFileProxyBase* pFilterOne ,const CAvatarFileProxyBase* pFilterTwo);

////////////////////
// CAvatarFileStore

class DLL_IMP_EXP CAvatarFileStore {

private:
	void Init();

	AvatarFileVector* m_pAvatarFiles;

   static const double m_dMaxFilterVersion;

   CAvatarFileStore* m_pParentStore;

public:
	CAvatarFileStore() {m_pParentStore=NULL;};
	~CAvatarFileStore();

	void Register(const CAvatarFileProxyBase* pProxy);

   void Override(CAvatarFileStore &oAvatarFileStore);

	bool CheckForExtension(const char* pszExtension);
	CAvatarFile* CreateByExtension(const char* pszExtension);	

	int GetCount() const;
	const CAvatarFileProxyBase* GetAt(int i) const;

};

#endif // _VAL_AVATARFILESTORE_
