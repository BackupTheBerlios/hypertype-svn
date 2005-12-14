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
#include <errno.h>
#include <stdarg.h>
#include "string_aid.h"


bool CHTString::StartsWith(CHTString sOther) const
{
   return strncmp(GetString(), sOther.GetString(), sOther.Length()) == 0;
}

bool CHTString::StartsWithNoCase(CHTString sOther) const
{
   return strncasecmp(GetString(), sOther.GetString(), sOther.Length()) == 0;
}

CHTString CHTString::Left(int iChars) const
{
   if (*this == "") return "";
   if (iChars >= Length()) return *this;

   CHTString sLeftCopy = *this;
   sLeftCopy.RemoveAt(iChars, Length()-iChars);
   return sLeftCopy;
}

CHTString CHTString::Mid(int iStartChar, int iNumChars)
{
   CHTString sReturnString = Right(Length()-iStartChar);
   if (iNumChars != -1 && sReturnString.Length() > iNumChars)
      sReturnString = sReturnString.Left(iNumChars);

   return sReturnString;
}

CHTString CHTString::Right(int iNumChars)
{
   CHTString vString;
   vString.m_sData.append(m_sData, Length()-iNumChars, iNumChars);
   return vString;
}

bool CHTString::Find(CHTString sSearchText, int& riFoundOffset)
{
   return Find(sSearchText, 0, riFoundOffset);
}

bool CHTString::Find(CHTString sSearchText, int iSearchStart, int& riFoundOffset)
{
   int iResult = m_sData.find(sSearchText, iSearchStart);
   if (iResult == -1)
      return false;

   riFoundOffset = iResult;
   return true;
}

int CHTString::Replace(CHTString sOldText, CHTString sNewText)
{
   int iMatches = 0;
   int iFindTarget = 0;
   while((iFindTarget = m_sData.find(sOldText, iFindTarget)) != -1)
   {
      m_sData.replace(iFindTarget, sOldText.Length(), sNewText, sNewText.Length());
      iFindTarget += sNewText.Length();
      iMatches++;
   }

   return iMatches;
}

void CHTString::UpCase()
{
   for (int iChar = 0; iChar < Length(); iChar++)
   {
      m_sData[iChar] = toupper(m_sData[iChar]);
   }
}

bool CHTString::RemovePrefixNoCase(CHTString sPrefix)
{
   if (Length() < sPrefix.Length())
      return false;

   CHTString sTmpString = *this;
   sTmpString.UpCase();
   sPrefix.UpCase();
   if (strncmp(sPrefix, sTmpString, sPrefix.Length())==0)
   {
      if (sPrefix.Length() == sTmpString.Length())
         m_sData.assign("");
      else
         RemoveAt(0, sPrefix.Length());
      return true;
   }

   return false;
}

void CHTString::RemoveAt(int iIndex, int iCharsToRemove)
{
   assert(iIndex + iCharsToRemove <= Length());

   if (iIndex == 0 && iCharsToRemove == Length())
   {
      m_sData.assign("");
      return;
   }

   int iOldLength = Length();
   m_sData.erase(iIndex, iCharsToRemove);
   assert(iOldLength-iCharsToRemove == Length());
}

void CHTString::InsertAt(int iIndex, CHTString sTextToInsert)
{
   m_sData.insert(iIndex, sTextToInsert);
}

bool operator == (const CHTString& rs1, const CHTString& rs2)
{
   return (CompareNoCase(rs1, rs2) == 0);
}

bool operator == (const char* s1, const CHTString& rs2)
{
   return (CompareNoCase(s1, rs2) == 0);
}

bool operator == (const CHTString& rs1, const char* s2)
{
   return (CompareNoCase(rs1, s2) == 0);
}

bool operator != (const CHTString& rs1, const CHTString& rs2)
{
   return !(rs1 == rs2);
}

bool operator != (const char* s1, const CHTString& rs2)
{
   return !(s1 == rs2);
}

bool operator != (const CHTString& rs1, const char* s2)
{
   return !(rs1 == s2);
}

CHTString operator + (const CHTString& rs1, const CHTString& rs2)
{
   CHTString vNewString = rs1;
   vNewString += rs2;
   return vNewString;
}

CHTString operator + (const char* s1, const CHTString& rs2)
{
   CHTString vNewString = s1;
   vNewString += rs2;
   return vNewString;
}

CHTString operator + (const CHTString& rs1, const char* s2)
{
   CHTString vNewString = s2;
   return rs1 + vNewString;
}

int CompareNoCase(CHTString s1, CHTString s2)
{
   return strcasecmp(s1, s2);
}

bool IsSameNoCase(CHTString s1, CHTString s2)
{
   return (CompareNoCase(s1, s2) == 0);
}

CHTString IntToHTString(int iNum)
{
   char achBuffer[100];
   int iNumChars = snprintf(achBuffer, 100, "%d", iNum);
   assert(iNumChars < 100);
   return achBuffer;
}

CHTString TimeToString(time_t vTime)
{
   tm *pLocalTime = localtime(&vTime);
   char achBuffer[100];
   int iNumChars = snprintf(achBuffer, 100, "%4d-%02d-%02d %02d:%02d:%02d", 1900+pLocalTime->tm_year, pLocalTime->tm_mon, pLocalTime->tm_mday, pLocalTime->tm_hour, pLocalTime->tm_min, pLocalTime->tm_sec);
   assert(iNumChars < 100);
   return achBuffer;
}

bool StringToInt(CHTString sString, int& riNum)
{
   long int liResult = strtoll(sString, (char**)NULL, 10);

   if (errno == EINVAL || errno == ERANGE || liResult > INT_MAX || liResult < INT_MIN)
   {
      return false;
   }

   riNum = liResult;
   return true;
}

bool StringToDouble(CHTString sString, double& rdNum)
{
   char* pErrorDetection = NULL;
   double dResult = strtof(sString, &pErrorDetection);
   if (dResult == 0 && (pErrorDetection == (const char*)sString || errno == ERANGE))
      return false;

   rdNum = dResult;
   return true;
}

bool StringToBool(CHTString sString, bool& rbBool)
{
   if (sString == "T" ||
       sString == "Y" ||
       sString == "true" ||
       sString == "yes")
   {
      rbBool = true;
      return true;
   }
   if (sString == "F" ||
       sString == "N" ||
       sString == "false" ||
       sString == "no")
   {
      rbBool = false;
      return true;
   }

   return false;
}

CHTString DoubleToString(double dNum, int iNumDecimals)
{
   char chBuffer[1024];
   sprintf(chBuffer, "%*.*f", iNumDecimals, iNumDecimals, dNum);
   return chBuffer;
}

CHTString BoolToString(bool bBool)
{
   return bBool ? "true" : "false";
}

CHTString FormatString(const char* szFormat, ...)
{
   va_list argList;
   va_start(argList, szFormat);

   int iAttemptSize = 100;
   bool bBufferLargeEnough = false;
   CHTString sReturnString;
   while (!bBufferLargeEnough)
   {
      char* pchBuffer = (char*) new char[iAttemptSize];
      int iNeededSize = vsnprintf(pchBuffer, iAttemptSize, szFormat, argList);
      if (iNeededSize < iAttemptSize)
      {
         bBufferLargeEnough = true;
         sReturnString = pchBuffer;
      }

      delete [] pchBuffer;
   }

   va_end(argList);

   return sReturnString;
}

void SplitDelimitedString(CHTString sString, CHTStringArray& rasSplitStrings, CHTString sDelimiter)
{
   rasSplitStrings.clear();

   CHTString sStringToSplit = sString;
   int iPos = 0;
   while (sStringToSplit.Find(sDelimiter, iPos))
   {
      rasSplitStrings.Add(sStringToSplit.Left(iPos));
      sStringToSplit = sStringToSplit.Mid(iPos+sDelimiter.Length());
   }
   rasSplitStrings.Add(sStringToSplit);
}

int CalcNumberDifferentChars(CHTString s1, CHTString s2, int iLength)
{
   int iDifferences = 0;
   for (int iChar = 0; iChar < iLength; iChar++)
   {
      if (iChar >= s1.Length() || iChar >= s2.Length())
      {
         iDifferences++;
      }
      else
      {
         if (s1[iChar] != s2[iChar])
            iDifferences++;
      }
   }

   return iDifferences;
}


void TestCHTString()
{
   CHTString sStartingText = "123456";
   assert(sStartingText.Left(2) == "12");
   assert(sStartingText.Mid(2) == "3456");
   assert(sStartingText.Mid(2, 2) == "34");
   assert(sStartingText.Right(2) == "56");
   int iPos;
   assert(sStartingText.Find("3", iPos) && iPos == 2);
   assert(sStartingText.Find("34", iPos) && iPos == 2);
   assert(sStartingText.Right(2) == "56");

   sStartingText.RemoveAt(2, 2);
   assert(sStartingText == "1256");
   // TODO: test case insensitivity on == operators
}

