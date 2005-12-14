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
#include <assert.h>

#include "globals.h"
#include "image_aid.h"
#include "lesson.h"
#include "screen.h"
#include "SDL_aid.h"
#include "widget_image.h"
#include "widget_keyboard.h"
#include "widget_settings.h"

const char cBackspaceChar_c = (char)8;


CKeyboardWidget::CKeyboardWidget()
{
   m_pImageWidget = new CImageWidget;
}

CKeyboardWidget::~CKeyboardWidget()
{
   m_pImageWidget->SetImage(NULL);
   delete m_pImageWidget;
}

void CKeyboardWidget::LoadSettings(CKeyedValues& rSettings)
{
   m_pImageWidget->LoadRectSettings(rSettings);
   CHTString sImageMappings;
   if (rSettings.GetStringValue("ImageMappings", sImageMappings))
      SplitDelimitedString(sImageMappings, m_asImageFileMappings, '\n');
}

bool CKeyboardWidget::ValidateSettings(CHTString& rsElementaryError)
{
   if (m_asImageFileMappings.size() == 0)
   {
      rsElementaryError = "The 'ImageMappings' setting is missing or invalid.";
      return false;
   }
   for (char cCurChar = ' '; cCurChar <= '~'; cCurChar++)
   {
      if (!LoadImageForLetter(cCurChar, rsElementaryError))
         return false;
   }
   if (!LoadImageForLetter(cBackspaceChar_c, rsElementaryError)) return false;

   int iImageWidth = m_apKeyboardImages[0]->w;
   int iImageHeight = m_apKeyboardImages[0]->h;
   for (int iImage = 0; iImage < m_apKeyboardImages.size(); iImage++)
   {
      if (m_apKeyboardImages[iImage]->w != iImageWidth || m_apKeyboardImages[iImage]->h != iImageHeight)
      {
         rsElementaryError = "Image for key '" + m_asKeyboardKeys[iImage] + "' is not the same size as the other keyboard images.";
         return false;
      }
   }

   return true;
}

CHTString TranslateCharToImageName(char cLetter)
{
   CHTString sLetter = cLetter;
   if (cLetter == cBackspaceChar_c)
      sLetter = "bs";

   return sLetter;
}

void CKeyboardWidget::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   CHTString sCurKey = TranslateCharToImageName(GetCurKey(pEngine));

   for (int iImage = 0; iImage < m_asKeyboardKeys.size(); iImage++)
   {
      if (sCurKey.CompareWithCase(m_asKeyboardKeys[iImage]) == 0)
      {
         SDL_Surface* pCurImage = m_apKeyboardImages[iImage];
         m_pImageWidget->SetImage(pCurImage);
         m_pImageWidget->PaintWidget(pPaintingParms, pEngine);
      }
   }
}

bool CKeyboardWidget::WantsKeyboardEvents()
{
   return true;
}

void CKeyboardWidget::HandleKeyboardEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   PaintWidget(pPaintingParms, pEngine);
}

char CKeyboardWidget::GetCurKey(CScreenEngine* pEngine)
{
   int iTypedLength = pEngine->GetStringValue("TypedText").Length();
   CHTString sLessonText = pEngine->GetStringValue("LessonText");

   int iArrayOffset = 0;
   if (pEngine->GetBoolValue("LastKeyCorrect"))
      iArrayOffset = iTypedLength-1+1;
   else
   {
      EMistakeHandling eMistakeHandling;
      if (!TextToMistakeHandling(pEngine->GetStringValue("MistakeHandling"), eMistakeHandling))
         return '\0';

      switch(eMistakeHandling)
      {
         case EMistakeHandling_AutoCorrect:
            iArrayOffset = iTypedLength-1;
            break;

         case EMistakeHandling_MustCorrect:
            return cBackspaceChar_c;
         case EMistakeHandling_AllowMistakes:
            iArrayOffset = iTypedLength;
            break;
      }
   }

   return sLessonText[min(iArrayOffset, sLessonText.Length()-1)];
}

bool CKeyboardWidget::LoadImageForLetter(char cLetter, CHTString& rsError)
{
   CHTString sLetter = TranslateCharToImageName(cLetter);
   CHTString sKeyPrefix = sLetter + CHTString("=");
   CHTString sFileName;
   for (int iMapping = 0; iMapping < m_asImageFileMappings.size(); iMapping++)
   {
      if (m_asImageFileMappings[iMapping].StartsWith(sKeyPrefix))
      {
         sFileName = m_asImageFileMappings[iMapping];
         sFileName.RemoveAt(0, sKeyPrefix.Length());
         break;
      }
   }
   if (sFileName == "")
   {
      rsError = "Image file for key '" + sLetter + "' not specified.";
      return false;
   }

   SDL_Surface* pKeyboardLayout = GetOrLoadImage(sFileName, rsError);
   if (!pKeyboardLayout)
   {
      rsError = "Unable to load image for key '" + sLetter + "':\n" + rsError;
      return false;
   }

   m_asKeyboardKeys.Add(sLetter);
   m_apKeyboardImages.Add(pKeyboardLayout);
   return true;
}



