//==========================================---
// VT Structure Visualisation File Converter
//------------------------------------------
// File converter for structure visualisation
// Copyright 2002 Vapour Technology Ltd.
//
// InputData.cpp
// 19/03/2002 - James Smith
//
// $Id: InputData.h,v 1.1 2002/03/26 00:51:54 vap-james Exp $

#ifndef __VTSTRUCVIS_INPUTDATA__
#define __VTSTRUCVIS_INPUTDATA__

#include <fstream>

class CInputData {

public:

   CInputData(const char* pcFilename);
   // Constructor

   ~CInputData();
   // Destructor

   bool NextSection(void);
   // Advances to the next section of the file

   const char* SectionType(void);
   // Section name string

   unsigned int LoadIncr(void);
   // Is there a load increment associated with this section?
   // Returns 0 if not, or the number if so.

   float GetFloat(void);
   // Fetch a floating-point value from the stream

   void UngetFloat(float fVal);
   // Put a floating-point value back on the stream

   unsigned int GetUInt(void);
   // Fetch an unsigned integer from the stream

   void UngetUInt(unsigned int iVal);
   // Put an unsigned integer back on the stream

   bool DataReady(void);
   // Returns true if there is still data left in this section

   bool Beginning(void) {return m_bBeginning;}
   // Returns true if no data has been read from this section

   void SetNumBeams(unsigned int iNumBeams) {m_iNumBeams = iNumBeams;}
   // Sets the number of beams in the input data

   void SetNumBeamGroups(unsigned int iNumBeamGroups) {m_iNumBeamGroups = iNumBeamGroups;}
   // Sets the number of beam groups in the input data

   unsigned int GetNumBeams(void) {return m_iNumBeams;}
   // Gets the number of beams in the input data

   unsigned int GetNumBeamGroups(void) {return m_iNumBeamGroups;}
   // Gets the number of beam groups in the input data

   void SetNumSlabs(unsigned int iNumSlabs) {m_iNumSlabs = iNumSlabs;}
   // Sets the number of slabs in the input data

   void SetNumSlabGroups(unsigned int iNumSlabGroups) {m_iNumSlabGroups = iNumSlabGroups;}
   // Sets the number of slab groups in the input data

   unsigned int GetNumSlabs(void) {return m_iNumSlabs;}
   // Gets the number of slabs in the input data

   unsigned int GetNumSlabGroups(void) {return m_iNumSlabGroups;}
   // Gets the number of slab groups in the input data

   unsigned int NumFrames(void) {return m_iNumFrames;}
   // Gets the number of frames in the input data

   float GetMinStress(void) {return m_fMinStress;}
   // Gets the minimum stress value in the input data

   float GetMaxStress(void) {return m_fMaxStress;}
   // Gets the maximum stress value in the input data

   void AddToStressRange(float fStress);
   // Sets the minimum and maximum stress value in the input data

protected:

   std::ifstream m_oInput;
   // Incoming file stream

   char m_pcHeader[128];
   // Header line storage

   int m_iLoadIncr;
   // Load increment (if appropriate)

   bool m_bBeginning;
   // Has any data been read from the section yet?
   // True if not.

   unsigned int m_iNumBeams;
   unsigned int m_iNumBeamGroups;
   unsigned int m_iNumSlabs;
   unsigned int m_iNumSlabGroups;
   unsigned int m_iNumFrames;
   float m_fMinStress;
   float m_fMaxStress;

};

#endif //__VTSTRUCVIS_INPUTDATA__