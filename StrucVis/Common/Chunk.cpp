//==========================================---
// VT Structure Visualisation File Converter
//------------------------------------------
// File converter for structure visualisation
// Copyright 2002 Vapour Technology Ltd.
//
//! docentry  = "File Converter.VSV File IO"
//! userlevel =  Normal
//! file      = "Convert/Chunk.cpp"
//! author    = "James Smith"
//! date      = "19/3/2002"
//! rcsid     = "$Id: Chunk.cpp,v 1.1 2002/04/03 12:31:24 vap-james Exp $"

//#ifndef __NOT_USING_MFC__
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Chunk.h"

#include <math.h>
#include <memory.h>
#include <vector>

#define SCALE_FACTOR 0.01;

CChunk::CChunk(TChunkType oType) :
   m_iBufferLength(0),
   m_iDataSize(0),
   m_iDataProcessed(0),
   m_iChunkLength(0),
   m_pcData(NULL),
   m_oType(oType),
   m_pTempSubChunk(NULL)
{
}

CChunk::CChunk(const CChunk& oChunk) :
   m_iBufferLength(oChunk.m_iBufferLength),
   m_iDataSize(oChunk.m_iDataSize),
   m_iDataProcessed(oChunk.m_iDataProcessed),
   m_iChunkLength(oChunk.m_iChunkLength),
   m_pcData(NULL),
   m_oType(oChunk.m_oType),
   m_pTempSubChunk(oChunk.m_pTempSubChunk),
   m_oTOC(oChunk.m_oTOC)
{
   // Alloocate memory for copied data
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return;
   // Copy data
   memcpy(m_pcData,oChunk.m_pcData,m_iChunkLength);
}

CChunk::~CChunk() {
   if (m_pcData) delete [] m_pcData;
   if (m_pTempSubChunk) delete m_pTempSubChunk;
}

bool CChunk::Read(CInputData& oInput) {
   // Check that there isn't already data in this chunk.
   if (m_oType != CHUNK_NONE) 
      return false;

   // Read frame number
   m_iFrame = oInput.LoadIncr();

   // Process chunk
   switch (TranslateType(oInput.SectionType())) {
   case CHUNK_NONE:
      return false;
   case CHUNK_NODES:
      return ReadNodes(oInput);
   case CHUNK_BEAMSIZE:
      return ReadBeamSizes(oInput);
   case CHUNK_SLABSIZE:
      return ReadSlabSizes(oInput);
   case CHUNK_BEAMS:
      if (oInput.Beginning()) return ReadBeams(oInput);
      else return ReadSlabs(oInput);
   case CHUNK_TEMP:
      return ReadTemps(oInput);
   case CHUNK_NODEDISP:
      return ReadNodeDisplacements(oInput);
   case CHUNK_BEAMFORC:
      return ReadBeamForces(oInput);
   case CHUNK_SLABFORC:
      return ReadSlabForces(oInput);
   case CHUNK_CRACKS:
      return ReadCracks(oInput);
   default:
      return false;
   }

}

TChunkType CChunk::TranslateType(const char* pcType) {
   if (strcmp(pcType,"HEADER")                            == 0) return CHUNK_NONE;
   if (strcmp(pcType,"NODAL GEOMETRY")                    == 0) return CHUNK_NODES;
   if (strcmp(pcType,"SECTION SIZES")                     == 0) return CHUNK_BEAMSIZE;
   if (strcmp(pcType,"MATERIAL PROPERTIES")               == 0) return CHUNK_NONE;
   if (strcmp(pcType,"RESIDUAL STRESSES")                 == 0) return CHUNK_NONE;
   if (strcmp(pcType,"LAYERED SLAB")                      == 0) return CHUNK_SLABSIZE;
   if (strcmp(pcType,"MEMBER DATA")                       == 0) return CHUNK_BEAMS;
   if (strcmp(pcType,"BOUNDARY CONDITIONS")               == 0) return CHUNK_NONE;
   if (strcmp(pcType,"JOINT LOADS")                       == 0) return CHUNK_NONE;
   if (strcmp(pcType,"TEMPERATURES")                      == 0) return CHUNK_TEMP;
   if (strcmp(pcType,"SLAB TEMPERATURES")                 == 0) return CHUNK_TEMP;
   if (strcmp(pcType,"NODAL DISPLACEMENTS")               == 0) return CHUNK_NODEDISP;
   if (strcmp(pcType,"INTERNAL FORCES")                   == 0) return CHUNK_BEAMFORC;
   if (strcmp(pcType,"9_NODED_SLAB_CRACKS")               == 0) return CHUNK_CRACKS;
   if (strcmp(pcType,"9_NODED_PRINCIPAL_MEMBRANE_FORCES") == 0) return CHUNK_SLABFORC;
   return CHUNK_NONE;
}

bool CChunk::ReadNodes(CInputData& oInput) {   
   // Extract data
   std::vector<float*> oList;
   while (oInput.DataReady()) {
      // Read and discard node number
      oInput.GetUInt();
      // Create new node
      float* pEntry = NULL;
      pEntry = new float[3];
      if (pEntry == NULL) return false;
      // Load positions
      pEntry[2] = oInput.GetFloat() * SCALE_FACTOR;
      pEntry[1] = oInput.GetFloat() * SCALE_FACTOR;
      pEntry[0] = oInput.GetFloat() * SCALE_FACTOR;
      // Load onto list
      oList.push_back(pEntry);
   }

   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + oList.size()*3*sizeof(float);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_NODES);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of nodes
   *reinterpret_cast<unsigned int*>(pcCurrent) = oList.size();
   pcCurrent += sizeof(unsigned int);
   // Set chunk data
   float* pfCurrent = reinterpret_cast<float*>(pcCurrent);
   for (std::vector<float*>::iterator pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      *pfCurrent++ = (*pEntry)[0];
      *pfCurrent++ = (*pEntry)[1];
      *pfCurrent++ = (*pEntry)[2];
   }

   // Dump temporary data
   for (pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      delete *pEntry;      
   }
   oList.empty();
   // Done
   m_oType = CHUNK_NODES;
   return true;
}

bool CChunk::ReadBeamSizes(CInputData& oInput) {
   // Extract data
   std::vector<float*> oList;
   while (oInput.DataReady()) {
      // Read and discard initial number
      oInput.GetUInt();
      // Create new size data block
      float* pEntry = NULL;
      pEntry = new float[4];
      if (pEntry == NULL) return false;
      // Load positions
      pEntry[0] = oInput.GetFloat() * SCALE_FACTOR;
      pEntry[1] = oInput.GetFloat() * SCALE_FACTOR;
      pEntry[2] = oInput.GetFloat() * SCALE_FACTOR;
      pEntry[3] = oInput.GetFloat() * SCALE_FACTOR;
      // Load onto list
      oList.push_back(pEntry);
   }

   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + oList.size()*4*sizeof(float);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_BEAMSIZE);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of size sets
   *reinterpret_cast<unsigned int*>(pcCurrent) = oList.size();
   pcCurrent += sizeof(unsigned int);
   // Set chunk data
   float* pfCurrent = reinterpret_cast<float*>(pcCurrent);
   for (std::vector<float*>::iterator pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      *pfCurrent++ = (*pEntry)[0];
      *pfCurrent++ = (*pEntry)[1];
      *pfCurrent++ = (*pEntry)[2];
      *pfCurrent++ = (*pEntry)[3];
   }

   // Dump temporary data
   for (pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      delete *pEntry;      
   }
   oList.empty();
   // Done
   m_oType = CHUNK_BEAMSIZE;
   return true;
}

bool CChunk::ReadSlabSizes(CInputData& oInput) {
   // Extract data
   std::vector<float*> oList;
   // Skip initial 18 numbers that we don't need
   for (int i=0; i<18; i++) oInput.GetFloat();
   // Get per-group data
   while (oInput.DataReady()) {
      // Read number of layers
      unsigned int iNumLayers = oInput.GetUInt();
      // Skip 15 numbers
      for (i=0; i<15; i++) oInput.GetFloat();
      // Create new size data block
      float* pEntry = NULL;
      pEntry = new float[1];
      if (pEntry == NULL) return false;
      // Load thicknesses
      float fTop, fBottom;
      fBottom = oInput.GetFloat() * SCALE_FACTOR;
      for (i=0; i<iNumLayers-1; i++) oInput.GetFloat();
      fTop = oInput.GetFloat() * SCALE_FACTOR;
      pEntry[0] = fTop - fBottom;
      // Load onto list
      oList.push_back(pEntry);
   }

   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + oList.size()*1*sizeof(float);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_SLABSIZE);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of size sets
   *reinterpret_cast<unsigned int*>(pcCurrent) = oList.size();
   pcCurrent += sizeof(unsigned int);
   // Set chunk data
   float* pfCurrent = reinterpret_cast<float*>(pcCurrent);
   for (std::vector<float*>::iterator pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      *pfCurrent++ = (*pEntry)[0];
   }

   // Dump temporary data
   for (pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      delete *pEntry;      
   }
   oList.empty();
   // Done
   m_oType = CHUNK_SLABSIZE;
   return true;
}

bool CChunk::ReadBeams(CInputData& oInput) {
   // Extract data
   std::vector<unsigned int*> oList;
   // Get per-beam data
   while (oInput.DataReady()) {
      // Read beam ID
      unsigned int iBeam = oInput.GetUInt();
      // Read mystery number
      unsigned int iType = oInput.GetUInt();
      // If it's not 1, we've finished with the beams
      if (iType != 1) {
         // Put read data back onto stream
         oInput.UngetUInt(iType);
         oInput.UngetUInt(iBeam);
         break;
      }
      // Skip 3 numbers
      for (int i=0; i<3; i++) oInput.GetFloat();
      // Create new data block
      unsigned int* pEntry = NULL;
      pEntry = new unsigned int[3];
      if (pEntry == NULL) return false;
      // Load node data
      for (i=0; i<2; i++)
         pEntry[i+1] = oInput.GetUInt();
      // Skip 4 numbers
      for (i=0; i<4; i++) oInput.GetFloat();
      // Load group number
      pEntry[0] = oInput.GetUInt();
      if (pEntry[0] > oInput.GetNumBeamGroups()) oInput.SetNumBeamGroups(pEntry[0]);
      // Skip 1 number
      oInput.GetFloat();
      // Load onto list
      oList.push_back(pEntry);
   }

   // Store number of beams
   oInput.SetNumBeams(oList.size());

   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + oList.size()*3*sizeof(unsigned int);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_BEAMS);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of beams
   *reinterpret_cast<unsigned int*>(pcCurrent) = oList.size();
   pcCurrent += sizeof(unsigned int);
   // Set chunk data
   unsigned int* piCurrent = reinterpret_cast<unsigned int*>(pcCurrent);
   for (std::vector<unsigned int*>::iterator pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      *piCurrent++ = (*pEntry)[0];
      *piCurrent++ = (*pEntry)[1];
      *piCurrent++ = (*pEntry)[2];
   }

   // Dump temporary data
   for (pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      delete *pEntry;      
   }
   oList.empty();
   // Done
   m_oType = CHUNK_BEAMS;
   return true;
}

bool CChunk::ReadSlabs(CInputData& oInput) {
   // Extract data
   std::vector<unsigned int*> oList;
   // Get per-beam data
   while (oInput.DataReady()) {
      // Read beam ID
      oInput.GetUInt();
      // Skip 3 numbers
      for (int i=0; i<4; i++) oInput.GetFloat();
      // Create new data block
      unsigned int* pEntry = NULL;
      pEntry = new unsigned int[10];
      if (pEntry == NULL) return false;
      // Load node data
      for (i=1; i<10; i++)
         pEntry[i] = oInput.GetUInt();
      // Load group number
      unsigned int iGroup = oInput.GetUInt();
      if (iGroup > oInput.GetNumSlabGroups()) oInput.SetNumSlabGroups(iGroup);
      pEntry[0] = iGroup + oInput.GetNumBeamGroups();
      // Skip 2 numbers
      oInput.GetFloat(); oInput.GetFloat();
      // Load onto list
      oList.push_back(pEntry);
   }

   // Store number of slabs
   oInput.SetNumSlabs(oList.size());

   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + oList.size()*10*sizeof(unsigned int);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_SLABS);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of beams
   *reinterpret_cast<unsigned int*>(pcCurrent) = oList.size();
   pcCurrent += sizeof(unsigned int);
   // Set chunk data
   unsigned int* piCurrent = reinterpret_cast<unsigned int*>(pcCurrent);
   for (std::vector<unsigned int*>::iterator pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      for (int i=0; i<10; i++)
         *piCurrent++ = (*pEntry)[i];
   }

   // Dump temporary data
   for (pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      delete *pEntry;      
   }
   oList.empty();
   // Done
   m_oType = CHUNK_SLABS;
   return true;
}

bool CChunk::ReadTemps(CInputData& oInput) {
   // Extract data
   std::vector<float> oList;
   // Get beam group data
   int iCount = 0;
   while (oInput.DataReady() && iCount++ < oInput.GetNumBeamGroups()) {
      // Read group ID
      oInput.GetUInt();
      // Load temperature data
      oList.push_back(oInput.GetFloat());
      // Skip 12 numbers
      for (int i=0; i<12; i++) oInput.GetFloat();
   }
   oInput.NextSection();
   if (TranslateType(oInput.SectionType()) != CHUNK_TEMP) return false;
   // Get slab group data
   iCount = 0;
   while (oInput.DataReady() && iCount++ < oInput.GetNumSlabGroups()) {
      // Read group ID
      oInput.GetUInt();
      // Load temperature data
      oList.push_back(oInput.GetFloat());
      // Skip 12 numbers
      for (int i=0; i<12; i++) oInput.GetFloat();
   }

   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + oList.size()*sizeof(float);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_TEMP);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of temperatures
   *reinterpret_cast<unsigned int*>(pcCurrent) = oList.size();
   pcCurrent += sizeof(unsigned int);
   // Set chunk data
   float* pfCurrent = reinterpret_cast<float*>(pcCurrent);
   for (std::vector<float>::iterator pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      *pfCurrent++ = *pEntry;
   }

   // Done
   m_oType = CHUNK_TEMP;
   return true;
}

bool CChunk::ReadNodeDisplacements(CInputData& oInput) {
   // Extract data
   std::vector<float*> oList;
   while (oInput.DataReady()) {
      // Read and discard node number
      oInput.GetUInt();
      // Create new node
      float* pEntry = NULL;
      pEntry = new float[3];
      if (pEntry == NULL) return false;
      // Load positions
      pEntry[2] = oInput.GetFloat() * SCALE_FACTOR;
      pEntry[1] = oInput.GetFloat() * SCALE_FACTOR;
      pEntry[0] = oInput.GetFloat() * SCALE_FACTOR;
      // Discard next 8 numbers
      for (int i=0; i<8; i++) oInput.GetFloat();
      // Load onto list
      oList.push_back(pEntry);
   }

   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + oList.size()*3*sizeof(float);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_NODEDISP);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of nodes
   *reinterpret_cast<unsigned int*>(pcCurrent) = oList.size();
   pcCurrent += sizeof(unsigned int);
   // Set chunk data
   float* pfCurrent = reinterpret_cast<float*>(pcCurrent);
   for (std::vector<float*>::iterator pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      *pfCurrent++ = (*pEntry)[0];
      *pfCurrent++ = (*pEntry)[1];
      *pfCurrent++ = (*pEntry)[2];
   }

   // Dump temporary data
   for (pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      delete *pEntry;      
   }
   oList.empty();
   // Done
   m_oType = CHUNK_NODEDISP;
   return true;
}

bool CChunk::ReadBeamForces(CInputData& oInput) {
   // Extract data
   std::vector<float*> oList;
   int iCount = 0;
   while (oInput.DataReady() && iCount < oInput.GetNumBeams()) {
      // Read and discard beam number
      oInput.GetUInt();
      // Read and discard node number
      oInput.GetUInt();
      // Create new force set
      float* pEntry = NULL;
      pEntry = new float[6];
      if (pEntry == NULL) return false;
      // Load forces
      pEntry[2] = fabs(oInput.GetFloat());
      pEntry[1] = fabs(oInput.GetFloat());
      pEntry[0] = fabs(oInput.GetFloat());
      // Skip 9 numbers
      for (int j=0; j<9; j++) oInput.GetFloat();
      pEntry[5] = fabs(oInput.GetFloat());
      pEntry[4] = fabs(oInput.GetFloat());
      pEntry[3] = fabs(oInput.GetFloat());
      // Skip 8 numbers
      for (j=0; j<8; j++) oInput.GetFloat();
      // Set minimum and maximum stress values
      for (j=0; j<6; j++)
         oInput.AddToStressRange(pEntry[j]);
      // Load onto list
      oList.push_back(pEntry);
      iCount++;
   }

   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + oList.size()*6*sizeof(float);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_BEAMFORC);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of force sets
   *reinterpret_cast<unsigned int*>(pcCurrent) = oList.size();
   pcCurrent += sizeof(unsigned int);
   // Set chunk data
   float* pfCurrent = reinterpret_cast<float*>(pcCurrent);
   for (std::vector<float*>::iterator pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      for (int i=0; i<6; i++)
         *pfCurrent++ = (*pEntry)[i];
   }

   // Dump temporary data
   for (pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      delete *pEntry;      
   }
   oList.empty();
   // Done
   m_oType = CHUNK_BEAMFORC;
   return true;
}

bool CChunk::ReadSlabForces(CInputData& oInput) {
   // Extract data
   std::vector<float*> oList;
   while (oInput.DataReady()) {
      // Read and discard node number
      oInput.GetUInt();
      // Create new force set
      float* pEntry = NULL;
      pEntry = new float[27];
      if (pEntry == NULL) return false;
      // Load forces
      for (int i=0; i<9; i++) {
         pEntry[(i*3) + 2] = fabs(oInput.GetFloat());
         pEntry[(i*3) + 1] = fabs(oInput.GetFloat());
         pEntry[(i*3) + 0] = fabs(oInput.GetFloat());
      }
      // Set minimum and maximum stress values
      for (int j=0; j<27; j++)
         oInput.AddToStressRange(pEntry[j]);
      // Load onto list
      oList.push_back(pEntry);
   }

   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + oList.size()*27*sizeof(float);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_SLABFORC);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of force sets
   *reinterpret_cast<unsigned int*>(pcCurrent) = oList.size();
   pcCurrent += sizeof(unsigned int);
   // Set chunk data
   float* pfCurrent = reinterpret_cast<float*>(pcCurrent);
   for (std::vector<float*>::iterator pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      for (int i=0; i<27; i++)
         *pfCurrent++ = (*pEntry)[i];
   }

   // Dump temporary data
   for (pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      delete *pEntry;      
   }
   oList.empty();
   // Done
   m_oType = CHUNK_SLABFORC;
   return true;
}

bool CChunk::ReadCracks(CInputData& oInput) {
   // Extract data
   std::vector<unsigned char*> oList;
   while (oInput.DataReady()) {
      // Read and discard node number
      oInput.GetUInt();
      // We're only dealing with cracks on the first layer
      if (oInput.GetUInt() == 1) {
         // Create new crack set
         unsigned char* pEntry = NULL;
         pEntry = new unsigned char[9];
         if (pEntry == NULL) return false;
         // Load positions
         for (int i=0; i<9; i++) {
            pEntry[i] = static_cast<unsigned char>(oInput.GetUInt());
            oInput.GetFloat();
         }
         // Load onto list
         oList.push_back(pEntry);
      }
      else {
         for (int i=0; i<18; i++) oInput.GetFloat();
      }
   }

   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + oList.size()*9*sizeof(unsigned char);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_CRACKS);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of cracks
   *reinterpret_cast<unsigned int*>(pcCurrent) = oList.size();
   pcCurrent += sizeof(unsigned int);
   // Set chunk data
   for (std::vector<unsigned char*>::iterator pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      for (int i=0; i<9; i++) 
         *pcCurrent++ = (*pEntry)[i];
   }

   // Dump temporary data
   for (pEntry = oList.begin(); pEntry != oList.end(); pEntry++) {
      delete *pEntry;      
   }
   oList.empty();
   // Done
   m_oType = CHUNK_CRACKS;
   return true;
}

bool CChunk::CreateFrameInfoChunk(CInputData& oInput) {
   // Create chunk
   m_iChunkLength = 2 + (2*sizeof(unsigned int));
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_FRAMEINF);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of frames
   *reinterpret_cast<unsigned int*>(pcCurrent) = oInput.NumFrames();
   // Done
   m_oType = CHUNK_FRAMEINF;
   return true;
}

bool CChunk::CreateGroupChunk(CInputData& oInput) {
   // Create chunk
   int iNumGroups = oInput.GetNumBeamGroups() + oInput.GetNumSlabGroups();
   m_iChunkLength = 2 + (2*sizeof(unsigned int)) + iNumGroups*sizeof(unsigned char);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_GROUPS);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set number of cracks
   *reinterpret_cast<unsigned int*>(pcCurrent) = iNumGroups;
   pcCurrent += sizeof(unsigned int);
   // Set group data
   for (int i=0; i<oInput.GetNumBeamGroups(); i++)
      *pcCurrent++ = static_cast<unsigned char>(0x00);
   for (i=0; i<oInput.GetNumSlabGroups(); i++)
      *pcCurrent++ = static_cast<unsigned char>(0x01);
   // Done
   m_oType = CHUNK_GROUPS;
   return true;
}

bool CChunk::CreateStressChunk(CInputData& oInput) {
   // Create chunk
   m_iChunkLength = 2 + sizeof(unsigned int) + 2*sizeof(float);
   m_pcData = new unsigned char[m_iChunkLength];
   if (m_pcData == NULL) return false;
   unsigned char* pcCurrent = m_pcData;
   // Set chunk type
   *pcCurrent++ = static_cast<unsigned char>(CHUNK_STRESSR);
   // Set length
   *reinterpret_cast<unsigned int*>(pcCurrent) = m_iChunkLength;
   pcCurrent += sizeof(unsigned int);
   // Set number of subchunks
   *pcCurrent++ = static_cast<unsigned char>(0xFF);
   // Set minimum
   *reinterpret_cast<float*>(pcCurrent) = oInput.GetMinStress();
   pcCurrent += sizeof(float);
   // Set maximum
   *reinterpret_cast<float*>(pcCurrent) = oInput.GetMaxStress();
   // Done
   m_oType = CHUNK_STRESSR;
   return true;
}

bool CChunk::AddSubChunk(CChunk* pSubChunk) {
   if (m_iChunkLength == 0) {
      m_iChunkLength = 2 + sizeof(unsigned int);
   }
   m_iChunkLength += pSubChunk->Length();
   m_iChunkLength += 1;
   m_iChunkLength += sizeof(unsigned int);
   CTOCEntry oEntry;
   oEntry.m_iLength = pSubChunk->m_iChunkLength;
   oEntry.m_oType = pSubChunk->m_oType;
   oEntry.m_pChunk = pSubChunk;
   m_oTOC.push_back(oEntry);
   return true;
}

bool CChunk::Write(ofstream& oOutput) const {
   const char* pcData = NULL;
   bool bOK = true;
   if (m_oTOC.size() > 0) {
      // Calculate sizes and write header
      oOutput << static_cast<unsigned char>(m_oType);
      pcData = reinterpret_cast<const char*>(&m_iChunkLength);
      oOutput.write(pcData,sizeof(unsigned int));
      oOutput << static_cast<unsigned char>(m_oTOC.size());
      // Write TOC
      unsigned int iOffset = 2 + sizeof(unsigned int) + (m_oTOC.size() * (1 + sizeof(unsigned int)));
      pcData = reinterpret_cast<const char*>(&iOffset);
      for (std::vector<CTOCEntry>::const_iterator pSubChunk = m_oTOC.begin(); pSubChunk != m_oTOC.end(); pSubChunk++) {
         oOutput << static_cast<unsigned char>(pSubChunk->m_oType);
         oOutput.write(pcData,sizeof(unsigned int));
         iOffset += pSubChunk->m_iLength;
      }
      // Write subchunks
      for (pSubChunk = m_oTOC.begin(); pSubChunk != m_oTOC.end(); pSubChunk++)
         bOK &= pSubChunk->m_pChunk->Write(oOutput);
      return bOK;
   }
   else {
      // No subchunks - just write the chunk data to the stream
      oOutput.write(reinterpret_cast<const char *>(m_pcData),m_iChunkLength);
      return true;
   }
}

const unsigned char* CChunk::Data(void) const {
   return m_pcData + 6;
}

const CChunk* CChunk::SubChunk(TChunkType oType) const {
      for (std::vector<CTOCEntry>::const_iterator pEntry=m_oTOC.begin(); pEntry!=m_oTOC.end(); pEntry++) {
      if (pEntry->m_oType == oType) return pEntry->m_pChunk;
   }
   return NULL;
}

void CChunk::AddDisplacements(float* pfDisplacements) {
   // Check chunk type
   if (m_oType != CHUNK_NODEDISP) return;
   // Get pointer to displacement data
   unsigned char* pcData = m_pcData + 2 + sizeof(unsigned int);
   // Get number of displacements
   unsigned int iNumValues = *reinterpret_cast<unsigned int*>(pcData) * 3;
   pcData += sizeof(unsigned int);
   // Get displacement pointer
   float* pfData = reinterpret_cast<float*>(pcData);
   // Add displacements one by one
   for (unsigned int i=0; i<iNumValues; i++) {
      *pfData += *pfDisplacements;
      *pfDisplacements = *pfData;
      pfData++;
      pfDisplacements++;
   }
   // Done
   return;
}

unsigned int CChunk::NumNodes(void) const {
   // Check chunk type
   if (m_oType != CHUNK_NODES) return 0;
   // Get pointer to displacement data
   unsigned char* pcData = m_pcData + 2 + sizeof(unsigned int);
   // Get number of displacements
   return *reinterpret_cast<unsigned int*>(pcData);
}

void CChunk::ZeroDisplacements(void) {
   // Check chunk type
   if (m_oType != CHUNK_NODEDISP) return;
   // Get pointer to displacement data
   unsigned char* pcData = m_pcData + 2 + sizeof(unsigned int);
   // Get number of displacements
   unsigned int iNumValues = *reinterpret_cast<unsigned int*>(pcData) * 3;
   pcData += sizeof(unsigned int);
   // Get displacement pointer
   float* pfData = reinterpret_cast<float*>(pcData);
   // Zero displacements one by one
   for (unsigned int i=0; i<iNumValues; i++) {
      *pfData = 0.0f;
      pfData++;
   }
   // Done
   return;
}

bool CChunk::CreateChunk(const unsigned char* pcData, unsigned int iLength, unsigned int& iUsed, bool bLoadSubChunks) {

   iUsed = 0;

   // If we've not finished loading the TOC 
   if (m_oTOC.empty()) {
      // Is this the first lump of data?
      if (m_pcData == NULL) {
         // Create a minimum buffer space
         m_pcData = static_cast<unsigned char*>(malloc(iLength));
         if (m_pcData == NULL) return false;
         m_iBufferLength = iLength;
      }
      // Make sure buffer is big enough to take the data
      if (iLength + m_iDataSize > m_iBufferLength) {
         m_pcData = static_cast<unsigned char*>(realloc(m_pcData,(iLength + m_iDataSize) * 2));
         if (m_pcData == NULL) return false;
         m_iBufferLength = (iLength + m_iDataSize) * 2;
      }
      // Append data to buffer
      memcpy(m_pcData+m_iDataSize,pcData,iLength);
      m_iDataSize += iLength;
      iUsed = iLength;

      // Read type from data chunk
      if (m_iDataSize < 1) return false;
      m_oType = static_cast<TChunkType>(m_pcData[0]);
   
      // Read chunk size
      if (m_iDataSize < 5) return false;
      m_iChunkLength = *reinterpret_cast<unsigned int*>(m_pcData+1);

      // Read number of TOC entries?
      if (m_iDataSize < 6) return false;
      int iTOCLength = 6 + m_pcData[5] * (1 + sizeof(unsigned int));

      if (m_pcData[5] == 0xFF) {
         // have we loaded the entire chunk?
         if (m_iDataSize < m_iChunkLength) {
            iUsed = iLength;
            return false;   
         }
         iUsed = iLength - (m_iDataSize - m_iChunkLength);
         return true;
      }
      else {
         // Check if we have all TOC data
         if (m_iDataSize < iTOCLength) {
            iUsed = iLength;
            return false;   
         }
         iUsed = iLength - (m_iDataSize - iTOCLength);

         // Create TOC
         unsigned char* pcCurrent = m_pcData + 6;
         for (int i=0; i<m_pcData[5]; i++) {
            CTOCEntry oEntry;
            oEntry.m_oType = static_cast<TChunkType>(*pcCurrent++);
            oEntry.m_iOffset = *reinterpret_cast<unsigned int*>(pcCurrent);
            pcCurrent += sizeof(unsigned int);
            if (i > 0) {
               m_oTOC.back().m_iLength = oEntry.m_iOffset - m_oTOC.back().m_iOffset;
               if (i == m_pcData[5]-1) {
                  oEntry.m_iLength = *reinterpret_cast<unsigned int*>(m_pcData+1) - oEntry.m_iOffset;
               }
            }
            m_oTOC.push_back(oEntry);
         }
         // We've finished creating the TOC
         m_iDataProcessed = iTOCLength;
         // Dump stored data
         if (m_pcData) delete [] m_pcData;
         m_pcData = NULL;
         m_iDataSize = 0;
         m_iBufferLength = 0;         
      }
   }

   // Load subchunks if necessary
   if (bLoadSubChunks) {
      // Update data pointers
      pcData += iUsed;
      iLength -= iUsed;
      // While we have data
      while (iLength != 0 && m_iDataProcessed < m_iChunkLength) {
         // Create new subchunk if we don't have one
         if (m_pTempSubChunk == NULL) {
            m_pTempSubChunk = new CChunk;
         }
         // Add data to subchunk
         if (m_pTempSubChunk->CreateChunk(pcData,iLength,iUsed)) {
            // Store chunk if it's finished loading
            for (std::vector<CTOCEntry>::iterator pEntry=m_oTOC.begin(); pEntry!=m_oTOC.end(); pEntry++) {
               if (pEntry->m_pChunk == NULL) {
                  pEntry->m_pChunk = m_pTempSubChunk;
                  m_pTempSubChunk = NULL;
                  break;
               }
            }
         }
         // Update data pointers
         pcData += iUsed;
         iLength -= iUsed;
         m_iDataProcessed += iUsed;
      }
      if (m_iDataProcessed < m_iChunkLength) return false;
   }

   return true;
}