//===================================---
// VT Structure Visualisation Control
//-----------------------------------
// ActiveX Control for structure visualisation
// Copyright 2000 Vapour Technology Ltd.
//
// SlabElement.cpp
// 19/03/2002 - James Smith
//
// $Id: SlabElement.cpp,v 1.28 2002/03/28 13:06:16 vap-james Exp $

#include "stdafx.h"
#include "SlabElement.h"

#include <strstrea.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////
// CBeamElement

//const unsigned int CSlabElement::m_uiNumFields = 4;

CSlabElement::CSlabElement(CCortonaUtil *poCortona, CNodeSet* poNodeSet) : 
   CElement(poCortona, poNodeSet),
   m_fThickness(1)
{
   memset(m_pfStresses, 0, 9*sizeof(float));
   memset(m_piNodes, 0, 9*sizeof(unsigned int));
   memset(m_pcCracks, 0, 9*sizeof(unsigned char));
   memset(m_ppoField, 0, 4*sizeof(CCortonaField*));
}

CSlabElement::~CSlabElement() {
   for (int i = 0; i < 4; i++)
      if (m_ppoField[i]) {
         m_ppoField[i]->Release();
         delete m_ppoField[i];
      }
}

// Beginning of SlabElement node
const char pcSlabStart[] = " \
   EXTERNPROTO SlabElement [ \
      eventIn MFColor  set_colours \
      eventIn MFInt32  set_cracks \
      eventIn MFVec3f  set_nodes \
      eventIn SFBool   set_visible \
      field   MFColor  colours \
      field   MFInt32  cracks \
      field   MFString description \
      field   MFVec3f  nodes \
      field   SFFloat  thickness \
      eventOut MFString description_changed \
   ] \
   [ \
      \"";

bool CSlabElement::Display(const char* pcURL) const {
   // Calculate colours
   float pfColours[27];
   CalculateColours(pfColours);
   // Calculate node positions
   float pfNodes[27];
   CalculateNodePositions(pfNodes);
   // If the slab isn't there yet
   if (m_poNodePtr == NULL) {
      // Create a slab and add it to the scene
      // Create string buffer for VRML text
      char pcBuffer[2048];
      memset(pcBuffer,0, 2048);
      ostrstream strSlab(pcBuffer, 2048);
      // Add the basic SlabElement syntax
      strSlab << pcSlabStart;
      // Add the EXTERNPROTO URL
      if (pcURL != NULL) strSlab << pcURL;
      // Add the rest of the preamble;
      strSlab << "SlabElement.wrl\" ] SlabElement { ";
      // Set the description
      strSlab << " description [\"Slab: " << m_iElement << "\" ";
      strSlab << " \"Group: " << m_iGroup << "\" ";
      strSlab << " \"Temp: " << m_fTemp << "\"] ";
      // Set the size
      strSlab << " thickness " << m_fThickness;
      // Set colours
      strSlab << " colours [ ";
      for (int c=0; c<27; c++) strSlab << " " << pfColours[c];
      strSlab << " ] ";
      // Setup node positions
      strSlab << " nodes [ ";
      for (int n=0; n<27; n++) strSlab << " " << pfNodes[n];
      strSlab << " ] ";
      // Setup cracks
      strSlab << " cracks [ ";
      for (int cr=0; cr<9; cr++) strSlab << " " << static_cast<int>(m_pcCracks[cr]);
      strSlab << " ] ";
      // Close the SlabElement
      strSlab << "}";
      // Create VRML nodes from the buffer
      if (m_poNodePtr = m_poCortona->CreateVrmlFromString(pcBuffer)) {
         m_poCortona->AddToScene(*m_poNodePtr);
         // Create the field caches
         m_ppoField[0] = m_poCortona->CreateField("MFVec3f");
         if (m_ppoField[0])
            m_ppoField[0]->SetMFCount(9);
         else
            return false;
         m_ppoField[1] = m_poCortona->CreateField("MFInt32");
         if (m_ppoField[1])
            m_ppoField[1]->SetMFCount(9);
         else
            return false;
         m_ppoField[2] = m_poCortona->CreateField("MFColor");
         if (m_ppoField[2])
            m_ppoField[2]->SetMFCount(9);
         else
            return false;
         m_ppoField[3] = m_poCortona->CreateField("SFBool");
         return ((m_ppoField[0]->GetMFCount() == 9) &&
                 (m_ppoField[1]->GetMFCount() == 9) && 
                 (m_ppoField[2]->GetMFCount() == 9) && 
                  m_ppoField[3]);
      }
      else
         return false;
   }
   else {
      bool bOK = true;
      //#===--- Update node positions
      // Set values
      if (m_ppoField[0]) {
         for (int i=0; i<9 && bOK; i++) {
            bOK = m_ppoField[0]->SetMFVec3f(i, pfNodes[(i*3)], pfNodes[(i*3)+1], pfNodes[(i*3)+2]);
         }      
         // Send event
         if (bOK && !m_poNodePtr->AssignEventIn("set_nodes", *(m_ppoField[0])))
            bOK = false;
      }

      //#===--- Update cracks
      // Set values
      if (m_ppoField[1]) {
         for (int i=0; i<9 && bOK; i++) {
            bOK = m_ppoField[1]->SetMFInt32(i, m_pcCracks[i]);
         }      
         // Send event
         if (bOK && !m_poNodePtr->AssignEventIn("set_cracks", *(m_ppoField[1])))
            bOK = false;
      }

      //#===--- Update colours
      // Set values
      if (m_ppoField[2]) {
         for (int i=0; i<9 && bOK; i++) {
            bOK = m_ppoField[2]->SetMFColor(i, pfColours[(i*3)], pfColours[(i*3)+1], pfColours[(i*3)+2]);
         }      
         // Send event
         if (bOK && !m_poNodePtr->AssignEventIn("set_colours", *(m_ppoField[2])))
            bOK = false;
      }

      // Done
      return bOK;
   }
}

void CSlabElement::SetNodes(const unsigned int* piNodes) {
   memcpy(m_piNodes, piNodes, 9*sizeof(int));
   return;
}

void CSlabElement::SetCracks(unsigned int iLayer, const unsigned char* pcCracks) const {
   memcpy(m_pcCracks, pcCracks, 9*sizeof(char));
   return;
}

void CSlabElement::SetStresses (const float* pfStresses) const {
   memcpy(m_pfStresses, pfStresses, 9*sizeof(float));
   return;
}

void CSlabElement::SetTemp(float fTemp) const {
   m_fTemp = fTemp;
   // Update description
   if (m_poNodePtr) {
      CCortonaField* poString = m_poNodePtr->GetField("description");
      char pcBuffer[32];
      sprintf(pcBuffer,"Temp: %.2f",m_fTemp);
      poString->SetMFString(2,pcBuffer);
      poString->Release();
      delete poString;
   }
}

void CSlabElement::SetSize(float fThickness) {
   m_fThickness = fThickness;
}

void CSlabElement::CalculateColours(float* pfColours) const {
   switch (m_oColourScheme) {
   case COLOUR_GROUP:
      {
         for (int i=0; i<9; i++) {
            pfColours[(i*3) + 0] = m_pfColour[0];
            pfColours[(i*3) + 1] = m_pfColour[1];
            pfColours[(i*3) + 2] = m_pfColour[2];
         }
      }
      break;
   case COLOUR_STRESS:
      {
         for (int i=0; i<9; i++) {
            float fStress = (m_pfStresses[i] - m_fMinStress) / (m_fMaxStress - m_fMinStress);
            pfColours[(i*3) + 0] = fStress * 2;
            pfColours[(i*3) + 1] = 2 - (fStress * 2);
            pfColours[(i*3) + 2] = 0;
            // Clip to 0..1
            if (pfColours[(i*3) + 0] > 1) pfColours[(i*3) + 0] = 1;
            if (pfColours[(i*3) + 1] > 1) pfColours[(i*3) + 1] = 1;
         }
      }
      break;
   }
   return;
}

void CSlabElement::CalculateNodePositions(float* pfNodes) const {
   for (int i=0; i<9; i++) {
      const float* pNode = m_poNodeSet->Node(m_piNodes[i]);
      pfNodes[(i*3) + 0] = pNode[0];
      pfNodes[(i*3) + 1] = pNode[1];
      pfNodes[(i*3) + 2] = pNode[2];
   }
   return;
}

bool CSlabElement::SetVisible(bool bVisible) const {
   if (m_poNodePtr && m_ppoField[3]) {
      // Set value
      m_ppoField[3]->SetSFBool(bVisible);
      // Send event
      if (m_poNodePtr->AssignEventIn("set_visible", *(m_ppoField[3])))
         return true;
   }
   return false;
}
