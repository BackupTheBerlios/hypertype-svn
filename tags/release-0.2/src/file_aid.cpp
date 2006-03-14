/***************************************************************************
 *   Copyright (C) 2004 by Josh Nisly                                      *
 *   hypertype@joshnisly.com                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "file_aid.h"

const int iInitialBufferSize_c = 1024;

bool ExistFile(CHTString sPath)
{
   assert(sPath.Length() > 0);
   struct stat tmp;
   return (stat(sPath, &tmp) != -1 || errno != ENOENT);
}


CTextFile::CTextFile()
{
   m_pFile = NULL;
   m_pBuffer = (char*)malloc(iInitialBufferSize_c);
   m_iBufferSize = iInitialBufferSize_c;
   m_iBufferOffset = 0;
   m_iValidBufferSize = 0;
   m_bEOF = false;
}

CTextFile::~CTextFile()
{
   if (IsOpen())
   {
      assert(false);
      CHTString sIgnored;
      (void)Close(sIgnored); // There's nothing we can do if it fails
   }
   free(m_pBuffer);
}

bool CTextFile::Open(CHTString sFilePath, bool bWriteAccess, bool bAppend, CHTString& rsError)
{
   assert(!IsOpen());
   m_sFilePath = sFilePath;
   CHTString sOpenOptions;
   if (bWriteAccess)
   {
      if (bAppend)
         sOpenOptions = "a";
      else
         sOpenOptions = "w";
   }
   else
      sOpenOptions = "r";

   m_pFile = fopen(m_sFilePath, sOpenOptions);
   if (!m_pFile)
   {
      rsError = CHTString("Unable to open ")+m_sFilePath+": "+strerror(errno);
      return false;
   }

   return true;
}

bool CTextFile::IsOpen()
{
   return (m_pFile != NULL);
}

bool CTextFile::Close(CHTString& rsError)
{
   assert(IsOpen());
   int iResult = fclose(m_pFile);
   m_pFile = NULL;
   if (iResult != 0)
   {
      rsError = "Unable to write to "+m_sFilePath+": "+strerror(errno);
      return false;
   }

   return true;
}

bool CTextFile::ReadLine(CHTString& rsLine, CHTString& rsError)
{
   assert(!m_bEOF);
   int iRemainingBufferSize = m_iValidBufferSize-m_iBufferOffset;
   char* pRemainingBuffer = m_pBuffer+m_iBufferOffset;

   // If we find a newline in the buffer we have, return it.
   char* pEOL = (char*)memchr(pRemainingBuffer, '\n', iRemainingBufferSize);
   if (pEOL)
   {
      *pEOL = '\0';
      rsLine = pRemainingBuffer;
      m_iBufferOffset = pEOL-m_pBuffer+1;
      return true;
   }
   // Check for EOF.  If we're there, give back remaining file.
   // (The check for buffer sizes is so we can stomp in a final NULL)
   if (m_iValidBufferSize < m_iBufferSize && feof(m_pFile))
   {
      *(m_pBuffer+m_iValidBufferSize) = 0;
      rsLine = pRemainingBuffer;
      m_bEOF = true;
      return true;
   }

   // We can't find a newline char.  Move the remaining buffer to the start, read
   // in more to fill the buffer, and try again.
   if (m_iBufferOffset != 0)
   {
      // If no line ending found in unused buffer, but we have space remaining.
      // Move unused buffer to start, and read in to fill remaining buffer.
      memmove(m_pBuffer, pRemainingBuffer, iRemainingBufferSize);
      int iAmount = m_iBufferOffset;
      if (!InternalRead(m_pBuffer+iRemainingBufferSize, iAmount, rsError))
         return false;
      m_iValidBufferSize = iRemainingBufferSize+iAmount;
      m_iBufferOffset = 0;
      return ReadLine(rsLine, rsError);
   }

   // The entire buffer is full, but we still can't find a linefeed.  Keep
   // enlarging the buffer until we find one or hit EOF.
   bool bEnlargeBuffer = (m_iValidBufferSize == m_iBufferSize); // They won't match on initial read. (See constructor)
   if (bEnlargeBuffer)
   {
      m_pBuffer = (char*)realloc(m_pBuffer, m_iBufferSize+iInitialBufferSize_c);
      m_iBufferSize += iInitialBufferSize_c;
   }
   int iAmount = iInitialBufferSize_c;
   if (!InternalRead(m_pBuffer+m_iValidBufferSize, iAmount, rsError))
      return false;
   m_iValidBufferSize += iAmount;
   return ReadLine(rsLine, rsError);
}

bool CTextFile::SlurpFile(CHTStringArray& rasLines, CHTString& rsError)
{
   assert(IsOpen());
   while(!EndOfFile())
   {
      CHTString sLine;
      if (!ReadLine(sLine, rsError))
         return false;

      rasLines.Add(sLine);
   }

   return true;
}

bool CTextFile::AppendLine(CHTString sLine, CHTString& rsError)
{
   sLine += "\n"; // TODO: could make smart to only add if necessary
   if (fseek(m_pFile, 0, SEEK_END) != 0 || (int)fwrite(sLine, 1, sLine.Length(), m_pFile) != sLine.Length())
   {
      rsError = CHTString("Unable to write to file: ") + strerror(errno);
      return false;
   }

   return true;
}

bool CTextFile::EndOfFile()
{
   return m_bEOF;
}

bool CTextFile::InternalRead(void* pBuffer, int& riAmt, CHTString& rsError)
{
   int iRead = fread(pBuffer, 1, riAmt, m_pFile);
   if (iRead != riAmt)
   {
      if (ferror(m_pFile))
      {
         rsError = "Unable to read from "+m_sFilePath+": "+strerror(errno);
         return false;
      }
      assert(feof(m_pFile));
      // don't set m_bEOF here, bc we haven't actually returned the buffer yet
   }
   riAmt = iRead;
   return true;
}

bool GetDirectoryListing(CHTString sDirectory, CHTStringArray& rasEntries, CHTString& rsError)
{
   struct dirent **namelist;
   int iNumEntries = scandir(sDirectory, &namelist, 0, alphasort);
   if (iNumEntries < 0)
   {
      rsError = CHTString("Unable to read directory: ") + strerror(errno);
      return false;
   }

   for (int iEntry = 0; iEntry < iNumEntries; iEntry++)
   {
      rasEntries.Add(namelist[iEntry]->d_name);
      free(namelist[iEntry]);
   }
   free(namelist);

   return true;
}
