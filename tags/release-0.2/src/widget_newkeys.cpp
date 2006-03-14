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

#include "globals.h"
#include "lesson.h"
#include "SDL_aid.h"
#include "screen.h"
#include "widget_newkeys.h"
#include "widget_settings.h"

CNewKeysWidget::CNewKeysWidget() : CWidget()
{
}

CNewKeysWidget::~CNewKeysWidget()
{
}

void CNewKeysWidget::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   CWidget::OnPaintWidget(pPaintingParms->m_pDrawingSurface);
   SDL_Color vTextColor;
   vTextColor.r = ShowRedText(pEngine) ? 255 : 0;
   vTextColor.g = 0;
   vTextColor.b = 0;
   char cNextChar = GetCurrentKey(pEngine);
   m_vKeyTextWidget.SetText("'"+CHTString(cNextChar)+"'");
   m_vKeyTextWidget.SetTextColor(vTextColor);
   m_vKeyTextWidget.PaintWidget(pPaintingParms, pEngine);

   m_vKeyDescWidget.SetText(pEngine->GetStringValue(CHTString("KeyDesc")+TranslateUpperCharIfNecessary(cNextChar)));
   m_vKeyDescWidget.SetTextColor(vTextColor);
   m_vKeyDescWidget.PaintWidget(pPaintingParms, pEngine);

   pPaintingParms->m_aRectsToUpdate.Add(GetWidgetRect());
}

bool CNewKeysWidget::WantsKeyboardEvents()
{
   return true;
}

void CNewKeysWidget::HandleKeyboardEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   RepaintBackground(pPaintingParms);
   PaintWidget(pPaintingParms, pEngine);
}

void CNewKeysWidget::LoadSettings(CKeyedValues& rSettings)
{
   LoadRectSettings(rSettings);
}

bool CNewKeysWidget::ValidateSettings(CHTString& rsElementaryError)
{
   ValidateRectSettings(true, rsElementaryError);
   if (m_iWidgetHeight < 250)
   {
      rsElementaryError = "New keys widgets must be at least 250 pixels high.";
      return false;
   }

   if (m_iWidgetWidth < 250)
   {
      rsElementaryError = "New keys widgets must be at least 250 pixels wide.";
      return false;
   }

   const int iKeyTextHeight_c = 50;
   m_vKeyTextWidget.SetTop(m_iWidgetTop+iKeyTextHeight_c);
   m_vKeyTextWidget.SetLeft(m_iWidgetLeft);
   m_vKeyTextWidget.SetWidth(m_iWidgetWidth);
   m_vKeyTextWidget.SetHeight(iKeyTextHeight_c);
   m_vKeyTextWidget.SetShouldWrap(false);
   m_vKeyTextWidget.SetHAlign(ETextHAlign_Center);
   m_vKeyTextWidget.SetVAlign(ETextVAlign_Top);
   if (!m_vKeyTextWidget.SetFont("fonts/menu.ttf", 40, rsElementaryError))
      return false;

   const int iTextSpacing_c = 70;
   const int iKeyDescHeight_c = 200;
   m_vKeyDescWidget.SetTop(m_iWidgetTop+iKeyTextHeight_c+iTextSpacing_c);
   m_vKeyDescWidget.SetLeft(m_iWidgetLeft);
   m_vKeyDescWidget.SetWidth(m_iWidgetWidth);
   m_vKeyDescWidget.SetHeight(iKeyDescHeight_c);
   m_vKeyDescWidget.SetShouldWrap(true);
   m_vKeyDescWidget.SetHAlign(ETextHAlign_Center);
   m_vKeyDescWidget.SetVAlign(ETextVAlign_Top);
   if (!m_vKeyDescWidget.SetFont("fonts/menu.ttf", 20, rsElementaryError))
      return false;

   return true;
}

bool CNewKeysWidget::ShowRedText(CScreenEngine* pEngine)
{
   return !pEngine->GetBoolValue("LastKeyCorrect");
}

char CNewKeysWidget::GetCurrentKey(CScreenEngine* pEngine)
{
   int iTypedLength = pEngine->GetStringValue("TypedText").Length();
   CHTString sLessonText = pEngine->GetStringValue("LessonText");

   int iArrayOffset = 0;
   if (pEngine->GetBoolValue("LastKeyCorrect"))
      iArrayOffset = iTypedLength-1+1;
   else
      iArrayOffset = iTypedLength-1;

   return sLessonText[min(iArrayOffset, sLessonText.Length()-1)];
}

