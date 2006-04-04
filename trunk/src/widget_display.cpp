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

#include "message_box.h"
#include "OS.h"
#include "screen.h"
#include "widget.h"
#include "widget_display.h"
#include "xml_aid.h"

// END HEADER



// BEGIN CPP

// Handles actual screen drawing.  Does little more then telling widgets to paint and
// handling user input (keypresses, mouse clicks, etc.)
CWidgetDisplay::CWidgetDisplay()
{
   m_pEngine = NULL;
   m_bClearBackground = true;
   m_pWidgetManager = new CWidgetManager;
}

CWidgetDisplay::~CWidgetDisplay()
{
   delete m_pWidgetManager;
}

void CWidgetDisplay::Init(CScreenEngine* pEngine)
{
   m_pEngine = pEngine;
}

bool CWidgetDisplay::LoadFromXml(xmlNode* pXml, CHTString& rsError)
{
   if (!IsSameNoCase((char*)pXml->name, "display"))
   {
      assert(false);
      rsError = "Bad Xml.";
      return false;
   }
   xmlNode* pWidgetSettings = pXml->children;
   if (!InternalLoadFromXml(pWidgetSettings, rsError))
      return false;

   if (!m_pWidgetManager->ValidateAllWidgetSettings(rsError))
      return false;

   return true;
}

bool CWidgetDisplay::InternalLoadFromXml(xmlNode* pSettingsXml, CHTString& rsError)
{
   AdvanceToElementNode(pSettingsXml);
   while (pSettingsXml)
   {
      CHTString sNodeName = (char*)pSettingsXml->name;
      if (IsSameNoCase(sNodeName, "widget"))
      {
         if (!m_pWidgetManager->LoadWidgetSettings(pSettingsXml->children, m_pEngine, rsError))
            return false;
      }
      else if (IsSameNoCase(sNodeName, "clearscreen"))
      {
         if (!StringToBool((char*)pSettingsXml->children->content, m_bClearBackground))
         {
            rsError = "The clearscreen setting was not specified correctly.";
            return false;
         }
      }
      else if (IsSameNoCase(sNodeName, "include"))
      {
         CHTString sIncludeFile = (char*)pSettingsXml->children->content;
         MungerDataFilePath(sIncludeFile);
         xmlDocPtr pDoc = xmlReadFile(sIncludeFile, NULL, 0);
         if (pDoc == NULL)
         {
            rsError = "Unable to open include file " + sIncludeFile + ".";
            return false;
         }
         xmlNodePtr pIncludeRoot = xmlDocGetRootElement(pDoc);
         if (!IsSameNoCase((char*)pIncludeRoot->name, "include"))
         {
            xmlFreeDoc(pDoc);
            rsError = "<include> element must be root element.";
            return false;
         }
         if (!InternalLoadFromXml(pIncludeRoot->children, rsError))
         {
            xmlFreeDoc(pDoc);
            return false;
         }
         xmlFreeDoc(pDoc);
      }
      else
      {
         cout << "Unrecognized display xml node: " << sNodeName << endl;
         return false;
      }

      pSettingsXml = pSettingsXml->next;
      AdvanceToElementNode(pSettingsXml);
   }

   return true;
}

// Any user action will be stored in the engine, keyed on "action"
bool CWidgetDisplay::Display(SDL_Surface* pEntireScreen)
{
   if (m_bClearBackground)
      ClearSurface(pEntireScreen);

   CPaintingParms vParms;
   vParms.m_pDrawingSurface = pEntireScreen;
   vParms.m_vClippingRect.x = 0;
   vParms.m_vClippingRect.y = 0;
   vParms.m_vClippingRect.w = pEntireScreen->w;
   vParms.m_vClippingRect.h = pEntireScreen->h;
   PaintAllWidgets(&vParms, m_pEngine);

   bool bRepaint = false;
   m_pEngine->OnScreenStart(pEntireScreen, bRepaint);
   if (bRepaint)
      PaintAllWidgets(&vParms, m_pEngine);

   // Go into message loop
   int iLastUpdateTime = SDL_GetTicks();
   while (true)
   {
      SDL_Event vEvent;
      while (SDL_PollEvent(&vEvent))
      {
         if (!HandleEvent(vEvent, &vParms))
            return false;
      }

      CHTString sAction = m_pEngine->GetStringValue("action", false);
      if (sAction != "")
         break;

      if (SDL_GetTicks() - iLastUpdateTime > 200)
      {
         OnTimer(&vParms);
         iLastUpdateTime = SDL_GetTicks();
      }

      SDL_Delay(10);
   }

   m_pEngine->OnScreenEnd(pEntireScreen);

   return true;
}

void CWidgetDisplay::PaintAllWidgets(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   for (int iWidget = 0; iWidget < m_pWidgetManager->GetNumWidgets(); iWidget++)
   {
      m_pWidgetManager->GetWidget(iWidget)->PaintWidget(pPaintingParms, pEngine);
   }
   SDL_UpdateRects(pPaintingParms->m_pDrawingSurface, pPaintingParms->m_aRectsToUpdate);
   pPaintingParms->m_aRectsToUpdate.clear();
}

void CWidgetDisplay::ClearSurface(SDL_Surface* pSurface)
{
   SDL_Rect vClipRect;
   vClipRect.x = 0;
   vClipRect.y = 0;
   vClipRect.w = pSurface->w;
   vClipRect.h = pSurface->h;
   SDL_FillRect(pSurface, &vClipRect, SDL_MapRGB(pSurface->format, 0, 0, 0));
   SDL_UpdateRect(pSurface, vClipRect.x, vClipRect.y, vClipRect.w, vClipRect.h);
}

bool CWidgetDisplay::HandleEvent(SDL_Event vEvent, CPaintingParms* pPaintingParms)
{
   // Handle keyboard and mouse events
   switch (vEvent.type)
   {
      case SDL_KEYDOWN:
      //case SDL_KEYUP:
         OnKey(vEvent, pPaintingParms);
         break;
      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
         OnMouse(vEvent, pPaintingParms);
         break;

      case SDL_QUIT:
         return false;

      default:
         break;
   }

   return true;
}

void CWidgetDisplay::OnKey(SDL_Event vEvent, CPaintingParms* pPaintingParms)
{
   // Notify the engine first, so it'll be updated by the time the widgets query it
   m_pEngine->OnKey(vEvent.key.keysym);

   // If we have an action, don't bother letting the widgets know about the keystroke
   if (m_pEngine->GetStringValue("Action", false) != "")
      return;

   for (int iWidget = 0; iWidget < m_pWidgetManager->GetNumWidgets(); iWidget++)
   {
      if (m_pWidgetManager->GetWidget(iWidget)->WantsKeyboardEvents())
         m_pWidgetManager->GetWidget(iWidget)->HandleKeyboardEvent(vEvent, pPaintingParms, m_pEngine);
   }

   SDL_UpdateRects(pPaintingParms->m_pDrawingSurface, pPaintingParms->m_aRectsToUpdate);
   pPaintingParms->m_aRectsToUpdate.clear();
}

void CWidgetDisplay::OnMouse(SDL_Event vEvent, CPaintingParms* pPaintingParms)
{
   // Notify the engine first, so it'll be updated by the time the widgets query it
   m_pEngine->OnMouse(vEvent);

   for (int iWidget = 0; iWidget < m_pWidgetManager->GetNumWidgets(); iWidget++)
   {
      if (m_pWidgetManager->GetWidget(iWidget)->WantsMouseEvents())
         m_pWidgetManager->GetWidget(iWidget)->HandleMouseEvent(vEvent, pPaintingParms, m_pEngine);
   }
   SDL_UpdateRects(pPaintingParms->m_pDrawingSurface, pPaintingParms->m_aRectsToUpdate);
   pPaintingParms->m_aRectsToUpdate.clear();
}

void CWidgetDisplay::OnTimer(CPaintingParms* pPaintingParms)
{
   // Notify the engine first, so it'll be updated by the time the widgets query it
   m_pEngine->OnTimer();

   for (int iWidget = 0; iWidget < m_pWidgetManager->GetNumWidgets(); iWidget++)
   {
      if (m_pWidgetManager->GetWidget(iWidget)->WantsTimerEvents())
         m_pWidgetManager->GetWidget(iWidget)->HandleTimerEvent(pPaintingParms, m_pEngine);
   }
   SDL_UpdateRects(pPaintingParms->m_pDrawingSurface, pPaintingParms->m_aRectsToUpdate);
   pPaintingParms->m_aRectsToUpdate.clear();
}



