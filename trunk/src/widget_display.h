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

class CWidgetManager;

// TODO: consider moving class into .cpp and interfacing single function

// Handles actual screen drawing.  Does little more then telling widgets to paint and
// handling user input (keypresses, mouse clicks, etc.)  Not overridden.
class CWidgetDisplay
{
public:
   CWidgetDisplay();
   ~CWidgetDisplay();

   void Init(CScreenEngine* pEngine);

   bool LoadFromXml(xmlNode* pXml, CHTString& rsError);

// Any user action will be stored in the engine, keyed on "action"
   void Display(SDL_Surface* pEntireScreen);

protected:
   bool InternalLoadFromXml(xmlNode* pSettingsXml, CHTString& rsError);

   // General painting functions
   void PaintAllWidgets(CPaintingParms* pParms, CScreenEngine* pEngine);
   void ClearSurface(SDL_Surface* pSurface);

   // Event handling
   void HandleEvent(SDL_Event vEvent, CPaintingParms* pPaintingParms);
   void OnKey(SDL_Event vEvent, CPaintingParms* pPaintingParms);
   void OnMouse(SDL_Event vEvent, CPaintingParms* pPaintingParms);
   void OnTimer(CPaintingParms* pPaintingParms);

   CScreenEngine* m_pEngine;
   CWidgetManager* m_pWidgetManager;
   bool m_bClearBackground;
};

