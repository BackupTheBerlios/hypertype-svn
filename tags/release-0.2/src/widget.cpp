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
#include <libxml/parser.h>

#include "game.h"
#include "screen.h"
#include "string_aid.h"
#include "widget.h"
#include "widget_accuracy.h"
#include "widget_button.h"
#include "widget_completion.h"
#include "widget_image.h"
#include "widget_keyboard.h"
#include "widget_lesson_progress.h"
#include "widget_newkeys.h"
#include "widget_practice.h"
#include "widget_settings.h"
#include "widget_speed.h"
#include "widget_text.h"
#include "xml_aid.h"


CWidgetManager::CWidgetManager()
{
   Reset();
}

CWidgetManager::~CWidgetManager()
{
   Reset();
}

void CWidgetManager::Reset()
{
   for (int iWidget = 0; iWidget < m_apWidgets.size(); iWidget++)
   {
      delete m_apWidgets[iWidget];
   }
   m_apWidgets.clear();
}

bool CWidgetManager::LoadWidgetSettings(xmlNode* pSettingsXml, CScreenEngine* pEngine, CHTString& rsElementaryError)
{
   AdvanceToElementNode(pSettingsXml);
   CKeyedValues vWidgetSettings;
   vWidgetSettings.LoadFromXml(pSettingsXml);

   CHTString sWidgetName;
   if (!vWidgetSettings.GetStringValue("name", sWidgetName, rsElementaryError))
      return false;

   if (sWidgetName == "")
   {
      rsElementaryError = "No widget name was specified.";
      return false;
   }

   int iWidgetPos = 0;
   if (FindWidgetPos(sWidgetName, iWidgetPos))
   {
      CWidget* pWidget = GetWidget(iWidgetPos);
      pWidget->LoadSettings(vWidgetSettings);
   }
   else
   {
      CWidget* pNewWidget = CWidget::AllocateAndLoadNewWidget(pSettingsXml, pEngine, rsElementaryError);
      if (!pNewWidget) return false;

      m_apWidgets.Add(pNewWidget);
   }

   return true;
}

bool CWidgetManager::ValidateAllWidgetSettings(CHTString& rsElementaryError)
{
   for (int iWidget = 0; iWidget < m_apWidgets.size(); iWidget++)
   {
      if (!GetWidget(iWidget)->ValidateSettings(rsElementaryError))
         return false;
   }

   return true;
}

int CWidgetManager::GetNumWidgets()
{
   return m_apWidgets.size();
}

CWidget* CWidgetManager::GetWidget(int iWidget)
{
   if (iWidget >= GetNumWidgets())
   {
      assert(false);
      return NULL;
   }

   return m_apWidgets[iWidget];
}

bool CWidgetManager::FindWidgetPos(CHTString sWidgetName, int& riWidgetPos)
{
   for (int iWidget = 0; iWidget < m_apWidgets.size(); iWidget++)
   {
      if (IsSameNoCase(m_apWidgets[iWidget]->GetWidgetName(), sWidgetName))
      {
         riWidgetPos = iWidget;
         return true;
      }
   }

   riWidgetPos = -1;
   return false;
}


CWidget::CWidget()
{
   m_iWidgetTop = -1;
   m_iWidgetLeft = -1;
   m_iWidgetHeight = -1;
   m_iWidgetWidth = -1;
   m_pBackground = NULL;
   m_bSavedBackground = false;
}

CWidget::~ CWidget()
{
   if (m_pBackground)
      SDL_FreeSurface(m_pBackground);
}

CWidget* CWidget::AllocateAndLoadNewWidget(xmlNodePtr pSettingsXml, CScreenEngine* pEngine, CHTString& rsElementaryError)
{
   AdvanceToElementNode(pSettingsXml);
   CKeyedValues vSettings;
   vSettings.LoadFromXml(pSettingsXml);

   CWidget* pNewWidget = NULL;
   CHTString sWidgetType;
   if (vSettings.GetStringValue("type", sWidgetType, rsElementaryError))
   {
      if (IsSameNoCase(sWidgetType, "button"))
         pNewWidget = new CButtonWidget;
      else if (IsSameNoCase(sWidgetType, "image"))
         pNewWidget = new CImageWidget;
      else if (IsSameNoCase(sWidgetType, "text"))
         pNewWidget = new CTextWidget;
      else if (IsSameNoCase(sWidgetType, "newkeys"))
         pNewWidget = new CNewKeysWidget;
      else if (IsSameNoCase(sWidgetType, "practice"))
         pNewWidget = new CPracticeWidget;
      else if (IsSameNoCase(sWidgetType, "keyboard"))
         pNewWidget = new CKeyboardWidget;
      else if (IsSameNoCase(sWidgetType, "statistic"))
         pNewWidget = new CStatWidget;
      else if (IsSameNoCase(sWidgetType, "lesson_progress"))
         pNewWidget = new CLessonProgressWidget;
      else if (IsSameNoCase(sWidgetType, "sprite"))
         pNewWidget = GetCollidingSpriteWidget();
      else
         rsElementaryError = "Unknown widget type: " + sWidgetType + "\n";
   }
   else
      rsElementaryError = "You must specify a widget type.";

   CHTString sTmp;
   CHTString sWidgetName;
   if (vSettings.GetStringValue("name", sWidgetName, sTmp))
      pNewWidget->m_sWidgetName = sWidgetName;

   // Load widget
   if (pNewWidget)
   {
      pNewWidget->LoadSettings(vSettings);
   }
   else
   {
      assert(rsElementaryError != "");
      rsElementaryError = "Unable to load widget \"" + sWidgetType + "\": " + rsElementaryError;
   }
   return pNewWidget;
}

void CWidget::OnPaintedBackground(SDL_Surface* pScreen)
{
   if (!m_pBackground || m_pBackground->w != GetWidgetRect().w || m_pBackground->h != GetWidgetRect().h)
   {
      if (m_pBackground)
         SDL_FreeSurface(m_pBackground);
      m_pBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, GetWidgetRect().w, GetWidgetRect().h, 24, 0, 0, 0, 0);
   }

   SDL_Rect vBlitRect = GetWidgetRect();
   SDL_BlitSurface(pScreen, &vBlitRect, m_pBackground, NULL);
}

void CWidget::OnPaintWidget(SDL_Surface* pScreen)
{
   if (!m_bSavedBackground)
   {
      OnPaintedBackground(pScreen);
      m_bSavedBackground = true;
   }
}


void CWidget::RepaintBackground(CPaintingParms* pParms)
{
   if (m_pBackground)
   {
      SDL_Rect vBlitRect = GetWidgetRect();
      SDL_BlitSurface(m_pBackground, NULL, pParms->m_pDrawingSurface, &vBlitRect);
      pParms->m_aRectsToUpdate.Add(vBlitRect);
   }
}

void CWidget::ClearStoredBackground()
{
   SDL_FreeSurface(m_pBackground);
   m_pBackground = NULL;
}

SDL_Rect CWidget::GetWidgetRect()
{
   SDL_Rect vRect;
   vRect.x = m_iWidgetLeft;
   vRect.y = m_iWidgetTop;
   vRect.w = m_iWidgetWidth;
   vRect.h = m_iWidgetHeight;
   return vRect;
}

void CWidget::LoadRectSettings(CKeyedValues& rSettings)
{
   CHTString sIgnoredError;
   (void)rSettings.GetIntValue("top", m_iWidgetTop, sIgnoredError);

   (void)rSettings.GetIntValue("left", m_iWidgetLeft, sIgnoredError);

   (void)rSettings.GetIntValue("height", m_iWidgetHeight, sIgnoredError);

   (void)rSettings.GetIntValue("width", m_iWidgetWidth, sIgnoredError);
}

bool CWidget::ValidateRectSettings(bool bValidateWidthHeight, CHTString& rsElementaryError)
{
   if (m_iWidgetTop == -1)
   {
      rsElementaryError = "The \"widget top\" setting was not specified.";
      return false;
   }
   if (m_iWidgetLeft == -1)
   {
      rsElementaryError = "The \"widget left\" setting was not specified.";
      return false;
   }
   if (bValidateWidthHeight)
   {
      if (m_iWidgetWidth == -1)
      {
         rsElementaryError = "The \"widget width\" setting was not specified.";
         return false;
      }
      if (m_iWidgetHeight == -1)
      {
         rsElementaryError = "The \"widget height\" setting was not specified.";
         return false;
      }
   }

   return true;
}
