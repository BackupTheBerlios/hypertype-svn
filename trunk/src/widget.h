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

#ifndef WIDGET_H
#define WIDGET_H

#include <assert.h>
#include <libxml/tree.h>
#include <SDL_events.h>

#include "array_aid.h"
#include "string_aid.h"

class CLessonData;
class CPaintingParms;
class CKeyedValues;
struct SDL_Surface;
class CWidget;
class CScreenEngine;

class CWidgetManager
{
public:
   CWidgetManager();
   ~CWidgetManager();

   void Reset();

   bool LoadWidgetSettings(xmlNodePtr pSettingsXml, CScreenEngine* pEngine, CHTString& rsElementaryError);

   bool ValidateAllWidgetSettings(CHTString& rsElementaryError);

   int GetNumWidgets();
   CWidget* GetWidget(int iWidget);

protected:
   bool FindWidgetPos(CHTString sWidgetName, int& riWidgetPos);
   CHTArray<CWidget*> m_apWidgets;
};

class CWidget
{
public:
   CWidget();
   virtual ~CWidget();

   // May return NULL if invalid XML or settings
   static CWidget* AllocateAndLoadNewWidget(xmlNodePtr pSettingsXml, CScreenEngine* pEngine, CHTString& rsElementaryError);
   void LoadRectSettings(CKeyedValues& rSettings);
   bool ValidateRectSettings(bool bValidateWidthHeight, CHTString& rsElementaryError);
   virtual void LoadSettings(CKeyedValues& rSettings)=0;
   virtual bool ValidateSettings(CHTString& rsElementaryError)=0;

   // widget properties
   SDL_Rect GetWidgetRect();
   CHTString GetWidgetName() { return m_sWidgetName; }
   void SetTop(int iTop) { m_iWidgetTop = iTop; }
   void SetLeft(int iLeft) { m_iWidgetLeft = iLeft; }
   void SetWidth(int iWidth) { m_iWidgetWidth = iWidth; }
   void SetHeight(int iHeight) { m_iWidgetHeight = iHeight; }

   virtual void PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)=0;
   void OnPaintWidget(SDL_Surface* pScreen);

   // event handling
   virtual bool WantsMouseEvents() { return false; }
   virtual void HandleMouseEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine) { assert(false); }
   virtual bool WantsKeyboardEvents() { return false; }
   virtual void HandleKeyboardEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine) { assert(false); }
   virtual bool WantsTimerEvents() { return false; }
   virtual void HandleTimerEvent(CPaintingParms* pPaintingParms, CScreenEngine* pEngine) { assert(false); }

   void RepaintBackground(CPaintingParms* pParms);
   void ClearStoredBackground();

protected:
   int m_iWidgetTop;
   int m_iWidgetLeft;
   int m_iWidgetHeight;
   int m_iWidgetWidth;

   CHTString m_sWidgetName;

   SDL_Surface* m_pBackground;
private:
   void OnPaintedBackground(SDL_Surface* pScreen);
   bool m_bSavedBackground;
};

#endif

