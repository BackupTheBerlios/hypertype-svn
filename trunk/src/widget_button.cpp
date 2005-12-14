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
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_video.h>

#include "image_aid.h"
#include "screen.h"
#include "SDL_aid.h"
#include "widget_button.h"
#include "widget_settings.h"

CButtonWidget::CButtonWidget() : CWidget()
{
   m_pUpImage = NULL;
   m_pDownImage = NULL;
   m_pLabelFont = NULL;
   m_vAccelleratorKey = SDLK_UNKNOWN;
   m_iAccelTextPos = -1;

   m_bMouseOverButton = false;
}

CButtonWidget::~CButtonWidget()
{
   if (m_pLabelFont)
      TTF_CloseFont(m_pLabelFont);
}

void CButtonWidget::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   CWidget::OnPaintWidget(pPaintingParms->m_pDrawingSurface);
   m_bMouseOverButton = MouseOverButton();

   SDL_Rect vButtonRect;
   vButtonRect.x = m_iWidgetLeft;
   vButtonRect.w = m_iWidgetWidth;
   vButtonRect.y = m_iWidgetTop;
   vButtonRect.h = m_iWidgetHeight;

   CHTSurface vButtonSurface;
   vButtonSurface.m_iSurfaceX = m_iWidgetLeft;
   vButtonSurface.m_iSurfaceY = m_iWidgetTop;
   vButtonSurface.m_pSurface = GetButtonFace();

   CHTSurface vDestSurface;
   vDestSurface.m_iSurfaceX = 0;
   vDestSurface.m_iSurfaceY = 0;
   vDestSurface.m_pSurface = pPaintingParms->m_pDrawingSurface;

   // Draw button background
   BlitClippedSurface(vButtonSurface, vDestSurface, pPaintingParms->m_vClippingRect);

   // Draw button label
   SDL_Color vTextColor = GetLabelColor();
   SDL_Rect vTextRect = vButtonRect;
   DrawText(m_sLabel, pPaintingParms->m_pDrawingSurface, m_pLabelFont, vTextColor, vTextRect, ETextHAlign_Center, ETextVAlign_Center);
   PaintAccelMark(pPaintingParms, vTextRect);

   pPaintingParms->m_aRectsToUpdate.Add(GetWidgetRect());
}

void CButtonWidget::LoadSettings(CKeyedValues& rSettings)
{
   LoadRectSettings(rSettings);
   CHTString sIgnoredError;
   (void)rSettings.GetStringValue("upimage", m_sUpImagePath, sIgnoredError);
   (void)rSettings.GetStringValue("downimage", m_sDownImagePath, sIgnoredError);
   (void)rSettings.GetStringValue("label", m_sLabel, sIgnoredError);
   (void)rSettings.GetStringValue("action", m_sAction, sIgnoredError);
}

bool CButtonWidget::ValidateSettings(CHTString& rsElementaryError)
{
   if (!ValidateRectSettings(false, rsElementaryError))
      return false;

   m_pUpImage = GetOrLoadImage(m_sUpImagePath, rsElementaryError);
   if (!m_pUpImage)
      return false;
   m_pDownImage = GetOrLoadImage(m_sDownImagePath, rsElementaryError);
   if (!m_pDownImage)
      return false;
   m_pLabelFont = LoadFont("fonts/menu.ttf", 20, rsElementaryError);
   if (!m_pLabelFont)
      return false;

   if (m_sLabel == "")
   {
      rsElementaryError = "The \"button label\" setting was not specified.";
      return false;
   }
   if (m_sAction == "")
   {
      rsElementaryError = "The \"button action\" setting was not specified.";
      return false;
   }

   m_iWidgetWidth = m_pDownImage->w;
   m_iWidgetHeight = m_pDownImage->h;
   if (m_iWidgetHeight != m_pUpImage->h || m_iWidgetWidth != m_pUpImage->w)
   {
      rsElementaryError = "Both the up and down images must be the same size.";
      return false;
   }

   if (m_sLabel.Find("_", m_iAccelTextPos))
   {
      m_sLabel.RemoveAt(m_iAccelTextPos, 1);
      m_vAccelleratorKey = (SDLKey)tolower(m_sLabel[m_iAccelTextPos]);
   }
   return true;
}

bool CButtonWidget::WantsMouseEvents()
{
   return true;
}

void CButtonWidget::HandleMouseEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   if (rEvent.type == SDL_MOUSEBUTTONDOWN)
   {
      if (MouseOverButton())
         pEngine->SetStringValue("Action", m_sAction);
   }
   else if (rEvent.type == SDL_MOUSEMOTION)
   {
      if (m_bMouseOverButton != MouseOverButton()) // only repaint if necessary
      {
         RepaintBackground(pPaintingParms);
         PaintWidget(pPaintingParms, pEngine);
      }
   }
}

bool CButtonWidget::WantsKeyboardEvents()
{
   return true;
}

void CButtonWidget::HandleKeyboardEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   int iModifiers = rEvent.key.keysym.mod;
   iModifiers &= ~KMOD_NUM; // ignore NumLock
   if (iModifiers == KMOD_LALT || iModifiers == KMOD_RALT)
   {
      if (rEvent.key.keysym.sym == m_vAccelleratorKey)
         pEngine->SetStringValue("Action", m_sAction);
   }
}

SDL_Color CButtonWidget::GetLabelColor()
{
   SDL_Color vTextColor;
   vTextColor.r = 0;
   vTextColor.g = 0;
   vTextColor.b = 0;

   return vTextColor;
}

void CButtonWidget::PaintAccelMark(CPaintingParms* pPaintingParms, SDL_Rect vLabelRect)
{
   if (m_iAccelTextPos != -1)
   {
      // Calculate the screen offset of the underlined character
      int iScreenOffset = m_iAccelTextPos == 0 ? 0 : CalcTextWidth(m_pLabelFont, m_sLabel.Left(m_iAccelTextPos));
      vLabelRect.x += iScreenOffset;

      // Set text style to underline
      int iStyle = TTF_GetFontStyle(m_pLabelFont);
      TTF_SetFontStyle(m_pLabelFont, iStyle|TTF_STYLE_UNDERLINE);
      SDL_Color vTextColor = GetLabelColor();
      // Draw the text
      DrawText(m_sLabel[m_iAccelTextPos], pPaintingParms->m_pDrawingSurface, m_pLabelFont, vTextColor, vLabelRect, ETextHAlign_Left, ETextVAlign_Top);
      // Remove underline style
      TTF_SetFontStyle(m_pLabelFont, iStyle);
   }
}

bool CButtonWidget::MouseOverButton()
{
   int x,y;
   SDL_GetMouseState(&x, &y);
   return (x >= m_iWidgetLeft && x <= m_iWidgetLeft + m_iWidgetWidth &&
           y >= m_iWidgetTop && y <= m_iWidgetTop + m_iWidgetHeight);
}

