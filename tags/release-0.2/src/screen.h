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

#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "SDL_aid.h"

class SDL_Surface;
class CWidgetDisplay;
class CScreenEngine;
class CKeyedValues;

class CPaintingParms
{
public:
   SDL_Surface* m_pDrawingSurface;
   SDL_Rect m_vClippingRect;
   CRectArray m_aRectsToUpdate;
};
// Simple class.  Overridden to provide lesson traversal logic.
class CScreenIterator
{
public:
   CScreenIterator();
   virtual ~CScreenIterator();

   bool Init(SDL_Surface* pRootSurface, CHTString sSettingsFile, CHTString& rsError);

   // Usually returns false if load failed
   bool DisplayIteration(CHTString& rsError);
protected:

   // LoadAndInit will allocate and load a new engine and widget display.  Caller is responsible to delete.
   CWidgetDisplay* LoadDisplay(CHTString sScreen, CScreenEngine* pEngine, CHTString& rsError);

   virtual bool Iterator_Init(CHTString sSettingsFile, CHTString& rsError)=0;
   virtual CScreenEngine* Iterator_GetFirstScreen(OUT CHTString& rsDisplayFile, CHTString& rsError)=0;
   virtual CScreenEngine* Iterator_GetNextScreen(CScreenEngine* pPrevScreen, OUT CHTString& rsDisplayFile, CHTString& rsError)=0;
   virtual bool Iterator_OnScreenEnd(CScreenEngine* pScreen) { return true; }

   bool IteratorHandlesAction(CHTString sAction);
   // Returns false if we should quit the iteration
   bool HandleAction(CHTString sAction);

   CHTString GetFileForScreen(CHTString sScreenName);
   SDL_Surface* m_pRootSurface;
};

// Overridable base class.
// Overrides contain all business logic.
// Is notified of user input.
// Is responsible for telling the iterator what screen to display next.
class CScreenEngine
{
public:
   CScreenEngine();
   virtual ~CScreenEngine();

   virtual bool Load(CKeyedValues* pSettings, CHTString& rsError)=0;

   bool HasStringValue(CHTString sKey);
   bool HasDoubleValue(CHTString sKey);
   bool HasIntValue(CHTString sKey);
   bool HasBoolValue(CHTString sKey);

   CHTString GetStringValue(CHTString sKey, bool bMustExist = true);
   double GetDoubleValue(CHTString sKey, bool bMustExist = true);
   int GetIntValue(CHTString sKey, bool bMustExist = true);
   bool GetBoolValue(CHTString sKey, bool bMustExist = true);

   virtual void OnKey(SDL_keysym vKey)=0;
   virtual void OnMouse(SDL_Event vMouse)=0;
   virtual void OnTimer()=0;

   void SetStringValue(CHTString sKey, CHTString sValue);
   void SetDoubleValue(CHTString sKey, double dValue);
   void SetIntValue(CHTString sKey, int iValue);
   void SetBoolValue(CHTString sKey, bool bValue);

   virtual void OnScreenStart(SDL_Surface* pScreen, bool& rbScreenNeedsRepaint) { rbScreenNeedsRepaint = false; }
   virtual void OnScreenEnd(SDL_Surface* pScreen) {}

protected:
   struct
   {
      CHTStringArray m_asKeys;
      CHTStringArray m_asValues;
   } m_vStringDict;

   struct
   {
      CHTStringArray m_asKeys;
      CHTArray<double> m_adValues;
   } m_vDoubleDict;

   struct
   {
      CHTStringArray m_asKeys;
      CHTArray<int> m_aiValues;
   } m_vIntDict;

   struct
   {
      CHTStringArray m_asKeys;
      CHTArray<bool> m_abValues;
   } m_vBoolDict;
};

extern bool g_bWindowedMode;

#endif
