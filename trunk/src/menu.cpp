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

#include "game.h"
#include "lesson.h"
#include "screen.h"

class CMenuEngine : public CScreenEngine
{
public:
   CMenuEngine();
   ~CMenuEngine();

   virtual bool Load(CKeyedValues* pSettings, CHTString& rsError);

   virtual void OnKey(SDL_keysym vKey);
   virtual void OnMouse(SDL_Event vMouse);
   virtual void OnTimer() {}
};

class CMenuIterator : public CScreenIterator
{
public:
   CMenuIterator();
   virtual ~CMenuIterator();

protected:
   virtual bool Iterator_Init(CHTString sSettingsFile, CHTString& rsError);
   virtual CScreenEngine* Iterator_GetFirstScreen(OUT CHTString& rsDisplayFile, CHTString& rsError);
   virtual CScreenEngine* Iterator_GetNextScreen(CScreenEngine* pPrevScreen, OUT CHTString& rsDisplayFile, CHTString& rsError);
};

// END HEADER
//////////////////////////////

//////////////////////////////
// BEGIN CPP


CMenuEngine::CMenuEngine()
{
}

CMenuEngine::~CMenuEngine()
{
}

bool CMenuEngine::Load(CKeyedValues* pSettings, CHTString& rsError)
{
   // Nothing to load
   return true;
}

void CMenuEngine::OnKey(SDL_keysym vKey)
{
   // Nothing to do
}

void CMenuEngine::OnMouse(SDL_Event vMouse)
{
   // Nothing to do
}

CMenuIterator::CMenuIterator()
{
}

CMenuIterator::~CMenuIterator()
{
}

bool CMenuIterator::Iterator_Init(CHTString sSettingsFile, CHTString& rsError)
{
   // TODO: possibly validate that sSettingsFile == "" ?
   return true;
}

CScreenEngine* CMenuIterator::Iterator_GetFirstScreen(OUT CHTString& rsDisplayFile, CHTString& rsError)
{
   rsDisplayFile = "main";
   CScreenEngine* pEngine = new CMenuEngine;
   pEngine->SetStringValue("name", "main");
   return pEngine;
}

CScreenEngine* CMenuIterator::Iterator_GetNextScreen(CScreenEngine* pPrevScreen, OUT CHTString& rsDisplayFile, CHTString& rsError)
{
   CHTString sAction = pPrevScreen->GetStringValue("action");
   if (sAction.RemovePrefixNoCase("screen:"))
      rsDisplayFile = sAction;

   // Check for lesson:path/to/lesson
   bool bLesson = sAction.RemovePrefixNoCase("lessons:");
   bool bBookLesson = false;
   if (!bLesson) bBookLesson = sAction.RemovePrefixNoCase("book:");
   if (bLesson || bBookLesson)
   {
      CScreenIterator* pLessonIterator = bLesson ? GetLessonIterator() : GetBookIterator();
      if (!pLessonIterator->Init(m_pRootSurface, sAction, rsError) || !pLessonIterator->DisplayIteration(rsError))
      {
         cout << rsError << "\n";
         return NULL;
      }

      rsDisplayFile = pPrevScreen->GetStringValue("name");
   }
   else if (sAction.RemovePrefixNoCase("game:"))
   {
      if (!DisplayCometGame(m_pRootSurface, sAction, rsError))
         return NULL;

      rsDisplayFile = pPrevScreen->GetStringValue("name");
   }
   else
      rsDisplayFile = sAction;

   CScreenEngine* pNewEngine = new CMenuEngine;
   pNewEngine->SetStringValue("name", rsDisplayFile);
   return pNewEngine;
}

CScreenEngine* GetMenuEngine()
{
   return new CMenuEngine;
}

CScreenIterator* GetMenuIterator()
{
   return new CMenuIterator;
}
