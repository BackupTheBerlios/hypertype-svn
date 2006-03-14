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
#include <SDL_ttf.h>

#include "font_aid.h"
#include "screen.h"
#include "SDL_aid.h"
#include "widget_settings.h"
#include "widget_text.h"

CTextWidget::CTextWidget() : CWidget()
{
   m_bWrap = false;
   m_pTextFont = NULL;
   m_eHAlign = ETextHAlign_Left;
   m_eVAlign = ETextVAlign_Top;
   m_vTextColor.r = 0;
   m_vTextColor.g = 0;
   m_vTextColor.b = 0;
   m_iFontHeight = 20;
}

CTextWidget::~CTextWidget()
{
}

void CTextWidget::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   CWidget::OnPaintWidget(pPaintingParms->m_pDrawingSurface);

   RepaintBackground(pPaintingParms);

   m_sDisplayedText.Clear();

   if (pEngine->HasStringValue(m_sWidgetName+":Text"))
      m_sText = pEngine->GetStringValue(m_sWidgetName+":Text");

   int iTopOffset = 0;
   CHTString sTextToDraw = m_sText;
   while(sTextToDraw.Length() > 0)
   {
      CHTString sCurLine;
      if (!m_bWrap)
         sCurLine = sTextToDraw;
      else
         sCurLine = CalcTextLine(sTextToDraw, m_pTextFont, m_iWidgetWidth);
      SDL_Rect vTextRect;
      vTextRect.x = m_iWidgetLeft;
      vTextRect.y = m_iWidgetTop + iTopOffset;
      vTextRect.w = m_iWidgetWidth;
      vTextRect.h = m_iWidgetHeight;
      DrawText(sCurLine, pPaintingParms->m_pDrawingSurface, m_pTextFont, m_vTextColor, vTextRect, m_eHAlign, m_eVAlign);
      pPaintingParms->m_aRectsToUpdate.Add(vTextRect);

      int iTextHeight = int(double(vTextRect.h) * 1.3);
      iTopOffset += iTextHeight;
      sTextToDraw.RemoveAt(0, sCurLine.Length());
      m_sDisplayedText += sCurLine;
      if (iTopOffset + iTextHeight > m_iWidgetHeight)
         return;
   }
}

void CTextWidget::LoadSettings(CKeyedValues& rSettings)
{
   LoadRectSettings(rSettings);

   CHTString sIgnoredError;
   (void)rSettings.GetStringValue("text", m_sText, sIgnoredError);
   (void)rSettings.GetBoolValue("wrap", m_bWrap, sIgnoredError);
   (void)rSettings.GetIntValue("fontsize", m_iFontHeight, sIgnoredError);
}

bool CTextWidget::ValidateSettings(CHTString& rsElementaryError)
{
   if (m_sText == "")
   {
      rsElementaryError = "The \"widget text\" setting was not specified.";
      return false;
   }
   if (!ValidateRectSettings(true, rsElementaryError))
      return false;

   if (m_sText == "")
   {
      rsElementaryError = "The \"widget text\" setting was not specified.";
      return false;
   }
   if (m_iFontHeight > 40 || m_iFontHeight < 10)
   {
      rsElementaryError = "The \"widget text\" setting was not valid.";
      return false;
   }

   return SetFont("fonts/menu.ttf", m_iFontHeight, rsElementaryError);
}

void CTextWidget::SetText(CHTString sText)
{
   m_sText = sText;
}

void CTextWidget::SetTextColor(SDL_Color vColor)
{
   m_vTextColor = vColor;
}

void CTextWidget::SetShouldWrap(bool bWrap)
{
   m_bWrap = bWrap;
}

bool CTextWidget::SetFont(CHTString sFontFilePath, int iFontSize, CHTString& rsError)
{
   m_pTextFont = GetOrLoadFont(sFontFilePath, iFontSize, rsError);
   if (!m_pTextFont)
      return false;

   return true;
}

void CTextWidget::SetHAlign(ETextHAlign eHAlign)
{
   m_eHAlign = eHAlign;
}

void CTextWidget::SetVAlign(ETextVAlign eVAlign)
{
   m_eVAlign = eVAlign;
}

int CTextWidget::GetTextHeight()
{
   int iTextHeight = 0;
   int iIgnored = 0;
   TTF_SizeText(m_pTextFont, "hj", &iIgnored, &iTextHeight);
   return iTextHeight;
}

CHTString CTextWidget::GetDisplayedText()
{
   return m_sDisplayedText;
}

SDL_Rect CalcPartialTextRect(CHTString sText, int iStartingOffset, TTF_Font* pFont, SDL_Rect vEntireTextRect)
{
   int iTextOffset = CalcTextWidth(pFont, sText.Left(iStartingOffset));
   vEntireTextRect.x += iTextOffset;
   vEntireTextRect.w -= iTextOffset;
   return vEntireTextRect;
}

void CTextWidget::UpdatePaintedText(CHTString sNewText, CPaintingParms* pPaintingParms, bool bForcePaint)
{
   CWidget::OnPaintWidget(pPaintingParms->m_pDrawingSurface);

   if (m_sDisplayedText.IsSame(sNewText) && !bForcePaint)
      return;

   SDL_Rect vTextRect = GetWidgetRect();
   CHTString sOldText = GetDisplayedText();
   SDL_Surface* pTargetSurface = pPaintingParms->m_pDrawingSurface;

   int iFirstCharToBlank = -1;
   // Find out if there's anything we need to blank out
   for (int iChar = 0; iChar < sOldText.Length(); iChar++)
   {
      // new text shorter?
      if (iChar > sNewText.Length()-1 || sOldText[iChar] != sNewText[iChar])
      {
         iFirstCharToBlank = iChar;
         break;
      }
   }

   // Blank out anything necessary
   int iFirstCharToPaint = sOldText.Length()/*+1-1*/;
   if (iFirstCharToBlank != -1)
   {
      SDL_Rect vSrcRect = CalcPartialTextRect(sOldText, iFirstCharToBlank, m_pTextFont, vTextRect);
      BlitClippedSurface(CHTSurface(m_pBackground, vSrcRect.x, vSrcRect.y), CHTSurface(pTargetSurface, 0, 0), vSrcRect);
      iFirstCharToPaint = iFirstCharToBlank;
      pPaintingParms->m_aRectsToUpdate.Add(vSrcRect);
   }

   if (bForcePaint)
      iFirstCharToPaint = 0;

   // Paint new text
   CHTString sNewTextToPaint = sNewText.Mid(iFirstCharToPaint);
   if (sNewTextToPaint.Length() > 0)
   {
      SDL_Rect vNewTextRect = CalcPartialTextRect(sOldText, iFirstCharToPaint, m_pTextFont, vTextRect);
      DrawText(sNewTextToPaint, pTargetSurface, m_pTextFont, m_vTextColor, vNewTextRect, m_eHAlign, m_eVAlign);
      pPaintingParms->m_aRectsToUpdate.Add(vNewTextRect);
   }
   m_sDisplayedText = sNewText;
}

