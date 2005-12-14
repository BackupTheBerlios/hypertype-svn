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

#include "assert.h"

#include "lesson.h"
#include "message_box.h"
#include "screen.h"
#include "string_aid.h"
#include "widget_text.h"

CHTString g_sMessageBoxText = "";


class CMessageBoxEngine : public CScreenEngine
{
   public:
      CMessageBoxEngine() {}
      virtual ~CMessageBoxEngine() {}
      virtual bool Load(CKeyedValues* pSettings, CHTString& rsError)
      {
         SetStringValue("MainText:Text", g_sMessageBoxText);
         return true;
      }
      virtual void OnKey(SDL_keysym vKey) {}
      virtual void OnMouse(SDL_Event vMouse) {}
      virtual void OnTimer() {}
};


class CMessageBoxIterator : public CScreenIterator
{
public:
   CMessageBoxIterator() {}
   ~CMessageBoxIterator() {}

protected:
   virtual bool Iterator_Init(CHTString sSettingsFile, CHTString& rsError) { return true; }
   virtual CScreenEngine* Iterator_GetFirstScreen(OUT CHTString& rsDisplayFile, CHTString& rsError)
   {
      rsDisplayFile = "message_box";
      CMessageBoxEngine* pEngine = new CMessageBoxEngine;
      assert(pEngine->Load(NULL, rsError));
      return pEngine;
   }
   virtual CScreenEngine* Iterator_GetNextScreen(CScreenEngine* pPrevScreen, OUT CHTString& rsDisplayFile, CHTString& rsError)
   {
      assert(pPrevScreen->GetStringValue("Action") == "OK");
      return NULL;
   }
};


EMessageBoxResponse MessageBox(CHTString sMessageText, SDL_Surface* pRootSurface, EMessageBoxType eType)
{
   CHTString sError;
   CMessageBoxIterator vIterator;
   if (!vIterator.Init(pRootSurface, "", sError))
   {
      cout << sError << "\n";
      assert(false);
      return EMessageBoxResponse_OK;
   }

   g_sMessageBoxText = sMessageText;
   if (!vIterator.DisplayIteration(sError))
   {
      cout << sError << "\n";
      assert(false);
      return EMessageBoxResponse_OK;
   }

   return EMessageBoxResponse_OK;
}

CScreenEngine* GetMessageBoxEngine()
{
   return new CMessageBoxEngine;
}

