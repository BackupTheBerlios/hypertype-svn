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

#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include "widget.h"
#include "SDL_aid.h"

class CTextWidget : public CWidget
{
public:
   CTextWidget();
   virtual ~CTextWidget();

   virtual void PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine);

   void SetText(CHTString sText);
   void SetTextColor(SDL_Color vColor);
   void SetShouldWrap(bool bWrap);
   bool SetFont(CHTString sFontFilePath, int iFontSize, CHTString& rsError);
   void SetHAlign(ETextHAlign eHAlign);
   void SetVAlign(ETextVAlign eVAlign);

   int GetTextHeight();
   CHTString GetDisplayedText(); // returns the actual text displayed in the last paint operation.  May differ from set text if there is not enough space to display entire text

   void UpdatePaintedText(CHTString sNewText, CPaintingParms* pPaintingParms, bool bForcePaint);

private:
   virtual void LoadSettings(CKeyedValues& rSettings);
   virtual bool ValidateSettings(CHTString& rsElementaryError);

   CHTString m_sDisplayedText;
   CHTString m_sText;
   bool m_bWrap;
   TTF_Font* m_pTextFont;
   ETextHAlign m_eHAlign;
   ETextVAlign m_eVAlign;
   SDL_Color m_vTextColor;
   int m_iFontHeight;
};

#endif
