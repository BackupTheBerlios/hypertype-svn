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

#ifndef STRING_AID_H
#define STRING_AID_H

#include <iostream>

#include "array_aid.h"

using namespace std;

class CHTString
{
public:
   inline CHTString() {}
   inline CHTString(const CHTString &rOther) { m_sData.assign(rOther.m_sData); }
   inline CHTString(const char* pszOther) { m_sData = pszOther; }
   inline CHTString(char cOther) { m_sData += cOther; }
   ~CHTString() {}

   const char* GetString() const { return m_sData.c_str(); }
   operator const char*() const { return GetString(); }

   char& operator [] (int iIndex)
   {
      return m_sData[iIndex];
   }

   CHTString& operator = (const CHTString& rOther)
   {
      m_sData.assign(rOther.m_sData);
      return *this;
   }
   CHTString& operator += (const CHTString& rOther)
   {
      m_sData.append(rOther.m_sData);
      return *this;
   }
   CHTString& operator += (char cChar)
   {
      m_sData.append(1, cChar);
      return *this;
   }

   // Properties
   int Length() const { return strlen(m_sData.c_str()); }

   // Comparison
   int CompareWithCase(CHTString sOther) const { return strcmp(GetString(), sOther.GetString()); }
   bool IsSame(CHTString sOther) const { return (CompareWithCase(sOther) == 0); }
   bool StartsWith(CHTString sOther) const;
   bool StartsWithNoCase(CHTString sOther) const;

   // Retrieval
   CHTString Left(int iChars) const;
   CHTString Mid(int iStartChar/*0-based*/, int iNumChars=-1);
   CHTString Right(int iNumChars);
   bool Find(CHTString sSearchText, int& iFoundOffset);
   bool Find(CHTString sSearchText, int iSearchStart, int& riFoundOffset);

   // Operations
   int Replace(CHTString sOldText, CHTString sNewText);
   void Clear() { m_sData.clear(); }
   bool RemovePrefixNoCase(CHTString sPrefix); // Removes sPrefix if it exists.  Returns whether it was removed.
   void RemoveAt(int iIndex, int iCharsToRemove);
   void InsertAt(int iIndex, CHTString sTextToInsert);

   void TrimLeft(char chCharToTrim)
   {
      while (m_sData[0] == chCharToTrim)
         RemoveAt(0, 1);
   }
   void TrimRight(char chCharToTrim)
   {
      while (m_sData[Length()-1] == chCharToTrim)
         RemoveAt(Length()-1, 1);
   }

   // More fringe use stuff
   void UpCase();

private:
   string m_sData;
};

#define CHTStringArray CHTArray<CHTString>

bool operator == (const CHTString& rs1, const CHTString& rs2);
bool operator == (const char* s1, const CHTString& rs2);
bool operator == (const CHTString& rs1, const char* s2);

bool operator != (const CHTString& rs1, const CHTString& rs2);
bool operator != (const char* s1, const CHTString& rs2);
bool operator != (const CHTString& rs1, const char* s2);

CHTString operator + (const CHTString& rs1, const CHTString& rs2);
CHTString operator + (const char* s1, const CHTString& rs2);
CHTString operator + (const CHTString& rs1, const char* s2);

int CompareNoCase(CHTString s1, CHTString s2);
bool IsSameNoCase(CHTString s1, CHTString s2);

// These functions return false if the number is out of range, or if it is not a valid version of the data type
bool StringToInt(CHTString sString, int& riNum);
bool StringToDouble(CHTString sString, double& rdNum);
bool StringToBool(CHTString sString, bool& rbBool);

CHTString IntToHTString(int iNum);
CHTString TimeToString(time_t);
CHTString DoubleToString(double dNum, int iNumDecimals = 1);
CHTString BoolToString(bool bBool);

CHTString FormatString(const char* szFormat, ...);

void SplitDelimitedString(CHTString sString, CHTStringArray& rasSplitStrings, CHTString sDelimiter);

int CalcNumberDifferentChars(CHTString s1, CHTString s2, int iLength);

void TestCHTString();

#endif
