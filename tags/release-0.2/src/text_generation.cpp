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

#include "precompiled.h"

#include "file_aid.h"
#include "text_generation.h"

CHTString GenerateTextFromCharacters(CHTString sCharacters, int iTargetLength, bool bRepeat, bool bUseSpaces)
{
   // Generate text, sans spaces
   CHTString sGeneratedText;
   while (sGeneratedText.Length() < iTargetLength)
   {
      int iIndex = rand() % sCharacters.Length();
      char cCurChar = sCharacters[iIndex];

      if (cCurChar == sGeneratedText[sGeneratedText.Length()-1])
         continue;

      CHTString sNewString;
      if (bRepeat)
      {
         int iNumRepeats = (rand() % 3) + 2;
         for (int iCnt = 0; iCnt < iNumRepeats; iCnt++)
            sNewString += cCurChar;
      }
      else
         sNewString = cCurChar;

      sGeneratedText += sNewString;
   }

   // Insert spaces, if requested
   if (bUseSpaces)
   {
      const int iMaxWordLength_c = 7;
      int iWordStart = 0;
      while (true)
      {
         int iWordLength = bRepeat ? (rand() % iMaxWordLength_c)+2 : (rand() % iMaxWordLength_c+1)+1;
         int iWordEnd = iWordStart + iWordLength;
         if (iWordEnd >= sGeneratedText.Length())
            break;

         sGeneratedText.InsertAt(iWordEnd, " ");
         iWordStart = iWordEnd+1;
      }
   }

   // Trim generated text back to requested length
   sGeneratedText.RemoveAt(iTargetLength, sGeneratedText.Length()-iTargetLength);
   return sGeneratedText;
}

bool GenerateTextFromStrings(CHTString sFileWithStrings, CHTString sPossibleCharacters, int iTargetLength, bool bRandom, OUT CHTString& rsText, CHTString& rsError)
{
   CTextFile vFile;
   CHTStringArray asLines;
   if (!vFile.Open(sFileWithStrings, false, false, rsError) || !vFile.SlurpFile(asLines, rsError) || !vFile.Close(rsError))
      return false;

   rsText = GenerateTextFromStrings(asLines, sPossibleCharacters, iTargetLength, bRandom);
   return true;
}

bool ContainsOnlyChars(CHTString sStringToTest, CHTString sPossibleCharacters)
{
   for (int iChar = 0; iChar < sStringToTest.Length(); iChar++)
   {
      int iIgnored;
      if (sStringToTest[iChar] != ' ' && !sPossibleCharacters.Find(sStringToTest[iChar], iIgnored))
         return false;
   }

   return true;
}

CHTString GenerateTextFromStrings(CHTStringArray asStrings, CHTString sPossibleCharacters, int iTargetLength, bool bRandom)
{
   CHTString sGeneratedText;
   while (sGeneratedText.Length() < iTargetLength)
   {
      int iCurArrayPos = rand() % asStrings.size();
      if (sPossibleCharacters != "" && !ContainsOnlyChars(asStrings[iCurArrayPos], sPossibleCharacters))
         continue;

      sGeneratedText += asStrings[iCurArrayPos] + " ";
   }

   sGeneratedText.RemoveAt(sGeneratedText.Length()-1, 1); // remove last space
   return sGeneratedText;
}
