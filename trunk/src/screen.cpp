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

#include "screen.h"

#include <assert.h>
#include "game.h"
#include "menu.h"
#include "message_box.h"
#include "lesson.h"
#include "OS.h"
#include "widget_display.h"
#include "xml_aid.h"

bool g_bWindowedMode = false;

CScreenIterator::CScreenIterator()
{
   m_pRootSurface = NULL;
}

CScreenIterator::~CScreenIterator()
{
}

bool CScreenIterator::Init(SDL_Surface* pRootSurface, CHTString sSettingsFile, CHTString& rsError)
{
   m_pRootSurface = pRootSurface;
   if (!Iterator_Init(sSettingsFile, rsError))
      return false;

   return true;
}

bool CScreenIterator::DisplayIteration(CHTString& rsError)
{
   CHTString sScreenFile;
   CScreenEngine* pEngine = Iterator_GetFirstScreen(sScreenFile, rsError);

   while (pEngine)
   {
      CWidgetDisplay* pDisplay = LoadDisplay(sScreenFile, pEngine, rsError);
      if (!pDisplay)
      {
         assert(rsError != "");
         return false;
      }

      pDisplay->Display(m_pRootSurface);

      if (!Iterator_OnScreenEnd(pEngine))
         return true;

      CHTString sAction = pEngine->GetStringValue("Action");
      if (IteratorHandlesAction(sAction) && !HandleAction(sAction))
         return true;

      CScreenEngine* pNewEngine = Iterator_GetNextScreen(pEngine, sScreenFile, rsError);

      delete pEngine;
      delete pDisplay;

      pEngine = pNewEngine;
   }

   if (rsError != "")
      return false;

   return true;
}

CWidgetDisplay* CScreenIterator::LoadDisplay(CHTString sScreen, CScreenEngine* pEngine, CHTString& rsError)
{
   // Normally just interpret screen directly to file
   CHTString sScreenFile = GetFileForScreen(sScreen);
   xmlDocPtr pDoc = xmlReadFile(sScreenFile.GetString(), NULL, 0);
   if (pDoc == NULL)
   {
      rsError = "Unable to read XML file " + sScreenFile + ".";
      return NULL;
   }

   xmlNodePtr pRootElem = xmlDocGetRootElement(pDoc);
   if (!IsSameNoCase((char*)pRootElem->name, "display"))
   {
      xmlFreeDoc(pDoc);
      rsError = "<display> element must be root element.";
      return NULL;
   }

   CWidgetDisplay* pDisplay = new CWidgetDisplay;
   pDisplay->Init(pEngine);
   if (!pDisplay->LoadFromXml(pRootElem, rsError))
   {
      xmlFreeDoc(pDoc);
      delete pDisplay;
      return NULL;
   }

   xmlFreeDoc(pDoc);
   return pDisplay;
}

CHTString CScreenIterator::GetFileForScreen(CHTString sScreenName)
{
   CHTString sPath = "screens/" + sScreenName + ".xml";
   MungerDataFilePath(sPath);
   return sPath;
}

bool CScreenIterator::IteratorHandlesAction(CHTString sAction)
{
   if (sAction == "quit")
      return true;

   return false;
}

bool CScreenIterator::HandleAction(CHTString sAction)
{
   if (sAction == "quit")
      return false;

   return true;
}


CScreenEngine::CScreenEngine()
{
}

CScreenEngine::~CScreenEngine()
{
}

bool CScreenEngine::Load(CKeyedValues* pSettings, CHTString& rsError)
{
   return false;
}

bool CScreenEngine::HasStringValue(CHTString sKey)
{
   return (m_vStringDict.m_asKeys.LinearFind(sKey) != -1);
}

bool CScreenEngine::HasDoubleValue(CHTString sKey)
{
   return (m_vDoubleDict.m_asKeys.LinearFind(sKey) != -1);
}

bool CScreenEngine::HasIntValue(CHTString sKey)
{
   return (m_vIntDict.m_asKeys.LinearFind(sKey) != -1);
}

bool CScreenEngine::HasBoolValue(CHTString sKey)
{
   return (m_vBoolDict.m_asKeys.LinearFind(sKey) != -1);
}

CHTString CScreenEngine::GetStringValue(CHTString sKey, bool bMustExist)
{
   if (!HasStringValue(sKey))
   {
      if (bMustExist)
      {
         cout << "Unable to find string for key " << sKey << endl;
         assert(false);
      }
      return "";
   }

   int iArrayPos = m_vStringDict.m_asKeys.LinearFind(sKey);
   return m_vStringDict.m_asValues[iArrayPos];
}

double CScreenEngine::GetDoubleValue(CHTString sKey, bool bMustExist)
{
   if (!HasDoubleValue(sKey))
   {
      if (bMustExist)
      {
         cout << "Unable to find double for key " << sKey << endl;
         assert(false);
      }
      return 0.0;
   }

   int iArrayPos = m_vDoubleDict.m_asKeys.LinearFind(sKey);
   return m_vDoubleDict.m_adValues[iArrayPos];
}

int CScreenEngine::GetIntValue(CHTString sKey, bool bMustExist)
{
   if (!HasIntValue(sKey))
   {
      if (bMustExist)
      {
         cout << "Unable to find int for key " << sKey << endl;
         assert(false);
      }
      return 0;
   }

   int iArrayPos = m_vIntDict.m_asKeys.LinearFind(sKey);
   return m_vIntDict.m_aiValues[iArrayPos];
}

bool CScreenEngine::GetBoolValue(CHTString sKey, bool bMustExist)
{
   if (!HasBoolValue(sKey))
   {
      if (bMustExist)
      {
         cout << "Unable to find bool for key " << sKey << endl;
         assert(false);
      }
      return false;
   }

   int iArrayPos = m_vBoolDict.m_asKeys.LinearFind(sKey);
   return m_vBoolDict.m_abValues[iArrayPos];
}

void CScreenEngine::SetStringValue(CHTString sKey, CHTString sValue)
{
   if (HasStringValue(sKey))
   {
      int iArrayPos = m_vStringDict.m_asKeys.LinearFind(sKey);
      m_vStringDict.m_asValues[iArrayPos] = sValue;
   }
   else
   {
      m_vStringDict.m_asKeys.Add(sKey);
      m_vStringDict.m_asValues.Add(sValue);
   }
}

void CScreenEngine::SetDoubleValue(CHTString sKey, double dValue)
{
   if (HasDoubleValue(sKey))
   {
      int iArrayPos = m_vDoubleDict.m_asKeys.LinearFind(sKey);
      m_vDoubleDict.m_adValues[iArrayPos] = dValue;
   }
   else
   {
      m_vDoubleDict.m_asKeys.Add(sKey);
      m_vDoubleDict.m_adValues.Add(dValue);
   }
}

void CScreenEngine::SetIntValue(CHTString sKey, int iValue)
{
   if (HasIntValue(sKey))
   {
      int iArrayPos = m_vIntDict.m_asKeys.LinearFind(sKey);
      m_vIntDict.m_aiValues[iArrayPos] = iValue;
   }
   else
   {
      m_vIntDict.m_asKeys.Add(sKey);
      m_vIntDict.m_aiValues.Add(iValue);
   }
}

void CScreenEngine::SetBoolValue(CHTString sKey, bool bValue)
{
   if (HasBoolValue(sKey))
   {
      int iArrayPos = m_vBoolDict.m_asKeys.LinearFind(sKey);
      m_vBoolDict.m_abValues[iArrayPos] = bValue;
   }
   else
   {
      m_vBoolDict.m_asKeys.Add(sKey);
      m_vBoolDict.m_abValues.Add(bValue);
   }
}

