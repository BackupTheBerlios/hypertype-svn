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

#include "widget_completion.h"

#include <assert.h>
#include "screen.h"
#include "widget_text.h"

CCompletionWidget::CCompletionWidget()
{
   m_pTextWidget = new CTextWidget;
}

CCompletionWidget::~CCompletionWidget()
{
   delete m_pTextWidget;
}

void CCompletionWidget::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   CWidget::OnPaintWidget(pPaintingParms->m_pDrawingSurface);
   RepaintBackground(pPaintingParms);

   double dAccuracy = pEngine->GetDoubleValue("CompletionPercentage");
   CHTString sAccuracy = DoubleToString(dAccuracy) + "%";
   m_pTextWidget->SetText(sAccuracy);
   m_pTextWidget->PaintWidget(pPaintingParms, pEngine);
}

bool CCompletionWidget::WantsKeyboardEvents()
{
   return true;
}

void CCompletionWidget::HandleKeyboardEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   PaintWidget(pPaintingParms, pEngine);
}

void CCompletionWidget::LoadSettings(CKeyedValues& rSettings)
{
   LoadRectSettings(rSettings);
}

bool CCompletionWidget::ValidateSettings(CHTString& rsElementaryError)
{
   if (!ValidateRectSettings(true, rsElementaryError))
      return false;

   m_pTextWidget->SetTop(m_iWidgetTop);
   m_pTextWidget->SetLeft(m_iWidgetLeft);
   m_pTextWidget->SetWidth(m_iWidgetWidth);
   m_pTextWidget->SetHeight(m_iWidgetHeight);
   m_pTextWidget->SetHAlign(ETextHAlign_Center);
   m_pTextWidget->SetShouldWrap(false);
   if (!m_pTextWidget->SetFont("fonts/menu.ttf", 15, rsElementaryError))
      return false;

   return true;
}

