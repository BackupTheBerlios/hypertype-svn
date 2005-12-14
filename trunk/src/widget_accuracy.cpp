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

#include "lesson.h"
#include "screen.h"
#include "widget_accuracy.h"

CAccuracyWidget::CAccuracyWidget() : CWidget()
{
}

CAccuracyWidget::~CAccuracyWidget()
{
}

void CAccuracyWidget::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   CWidget::OnPaintWidget(pPaintingParms->m_pDrawingSurface);
   RepaintBackground(pPaintingParms);

   double dAccuracy = pEngine->GetDoubleValue("Accuracy");
   CHTString sAccuracy = DoubleToString(dAccuracy) + "%";
   m_TextWidget.SetText(sAccuracy);
   m_TextWidget.PaintWidget(pPaintingParms, pEngine);
}

bool CAccuracyWidget::WantsKeyboardEvents()
{
   return true;
}

void CAccuracyWidget::HandleKeyboardEvent(const SDL_Event&, CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   PaintWidget(pPaintingParms, pEngine);
}

void CAccuracyWidget::LoadSettings(CKeyedValues& rSettings)
{
   LoadRectSettings(rSettings);
}

bool CAccuracyWidget::ValidateSettings(CHTString& rsElementaryError)
{
   if (!ValidateRectSettings(true, rsElementaryError))
      return false;

   m_TextWidget.SetTop(m_iWidgetTop);
   m_TextWidget.SetLeft(m_iWidgetLeft);
   m_TextWidget.SetWidth(m_iWidgetWidth);
   m_TextWidget.SetHeight(m_iWidgetHeight);
   m_TextWidget.SetHAlign(ETextHAlign_Center);
   m_TextWidget.SetShouldWrap(false);
   if (!m_TextWidget.SetFont("fonts/menu.ttf", 15, rsElementaryError))
      return false;

   return true;
}

