//#========---
//#=- VALET
//#=====---
// Vapour Technology All-Purpose Library
// Copyright 2001 Vapour Technology Ltd.
//
// Add confidentiality notice

////////////////
//! file 		= "VALET/core/loghandle.noarch.cpp"
//! author 		= "Warren Moore"
//! date 		= "23/09/2001"
//! lib 			= libVALETcore
//! rcsid 		= "$Id: loghandle.noarch.cpp,v 1.3 2001/10/21 14:38:33 vap-warren Exp $"

//#===--- Includes
#include "loghandle.h"

#include <stdio.h>
#include <string.h>

//#===--- CLogHandle

namespace NValet {

	CLogHandle::CLogHandle(const char *pcType) : 
		m_poLogFile(NULL) {
		ASSERT(pcType);
		// Create the log filename
		char pcName[m_uiStrLength] = "";
		strcpy(pcName, pcType);
		strcat(pcName, ".log");
		// Create the log file
		m_poLogFile = new ofstream(pcName, ios::out|ios::app);
		// TODO: Add time and date
		if (m_poLogFile) {
			*m_poLogFile << "---" << endl;
		}
	} // CLogHandle::CLogHandle

	CLogHandle::~CLogHandle() {
		// Delete the log file
		if (m_poLogFile) {
			m_poLogFile->close();
			delete m_poLogFile;
		}
	} // CLogHandle::~CLogHandle

	void CLogHandle::Trace(int iLevel, const char *pcFunction, const char *pcMessage) {
		ASSERT(iLevel >= 0);
		ASSERT(pcMessage);
		if (!m_poLogFile)
			return;
		*m_poLogFile << iLevel << " : " << pcFunction << " - " << pcMessage << endl;
	} // CLogHandle::Trace
	
}
