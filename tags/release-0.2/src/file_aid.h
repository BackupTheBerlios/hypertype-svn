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

#include "string_aid.h"

bool ExistFile(CHTString sPath);

class CTextFile
{
public:
   CTextFile();
   ~CTextFile();

   bool Open(CHTString sFilePath, bool bWriteAccess, bool bAppend, CHTString& rsError);
   bool IsOpen();
   bool Close(CHTString& rsError);

   // Only returns false on failure
   bool ReadLine(CHTString& rsLine, CHTString& rsError);
   bool SlurpFile(CHTStringArray& rasLines, CHTString& rsError);

   bool AppendLine(CHTString sLine, CHTString& rsError);

   bool EndOfFile();
private:
   bool InternalRead(void* pBuffer, int& riAmt, CHTString& rsError);
   FILE *m_pFile;
   char* m_pBuffer;
   int m_iBufferSize;
   int m_iBufferOffset;
   int m_iValidBufferSize; // buffer size that actually has data
   bool m_bEOF;
   CHTString m_sFilePath;
};

bool GetDirectoryListing(CHTString sDirectory, CHTStringArray& rasEntries, CHTString& rsError);
