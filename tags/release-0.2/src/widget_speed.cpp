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

#include "widget_speed.h"

#include <assert.h>
#include "screen.h"
#include "widget_settings.h"

bool StringToLabelAlignment(CHTString sString, OUT ELabelAlignment& reAlignment)
{
   if (sString == "Top")
      reAlignment = ELabelAlignment_Top;
   else if (sString == "Bottom")
      reAlignment = ELabelAlignment_Bottom;
   else if (sString == "Left")
      reAlignment = ELabelAlignment_Left;
   else if (sString == "Right")
      reAlignment = ELabelAlignment_Right;
   else
      return false;

   return true;
}

CStatWidget::CStatWidget()
{
   m_iLastUpdate = 0;
   m_bShowPercentSign = false;
   m_eLabelAlignment = ELabelAlignment_Top;
}

CStatWidget::~CStatWidget()
{
}

void CStatWidget::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   CWidget::OnPaintWidget(pPaintingParms->m_pDrawingSurface);
   RepaintBackground(pPaintingParms);

   CHTString sStatValue;
   if (pEngine->HasDoubleValue(m_sStatName))
      sStatValue = DoubleToString(pEngine->GetDoubleValue(m_sStatName));
   else if (pEngine->HasIntValue(m_sStatName))
      sStatValue = IntToHTString(pEngine->GetIntValue(m_sStatName));

   if (sStatValue != "")
   {
      if (m_bShowPercentSign)
         sStatValue += "%";
      m_LabelTextWidget.SetText(m_sLabel);
      m_LabelTextWidget.PaintWidget(pPaintingParms, pEngine);
      m_StatTextWidget.SetText(sStatValue);
      m_StatTextWidget.PaintWidget(pPaintingParms, pEngine);
   }

   m_iLastUpdate = SDL_GetTicks();
}

bool CStatWidget::WantsKeyboardEvents()
{
   return true;
}

void CStatWidget::HandleKeyboardEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   PaintWidget(pPaintingParms, pEngine);
}

void CStatWidget::HandleTimerEvent(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   if (SDL_GetTicks() - m_iLastUpdate >= 500)
      PaintWidget(pPaintingParms, pEngine);
}

void CStatWidget::LoadSettings(CKeyedValues& rSettings)
{
   LoadRectSettings(rSettings);

   CHTString sIgnored;
   (void)rSettings.GetStringValue("Statistic", m_sStatName, sIgnored);
   (void)rSettings.GetStringValue("Label", m_sLabel, sIgnored);
   (void)rSettings.GetBoolValue("ShowPercentSign", m_bShowPercentSign, sIgnored);
   (void)rSettings.GetStringValue("LabelAlignment", m_sTmpAlignment, sIgnored);
}

bool CStatWidget::ValidateSettings(CHTString& rsElementaryError)
{
   if (!ValidateRectSettings(true, rsElementaryError))
      return false;

   if (m_sLabel == "" || m_sStatName == "")
   {
      rsElementaryError = "The 'Label' or 'Statistic' setting is missing or invalid.";
      return false;
   }

   if (m_sTmpAlignment != "" && !StringToLabelAlignment(m_sTmpAlignment, m_eLabelAlignment))
   {
      rsElementaryError = "The 'LabelAlignment' setting is invalid.";
      return false;
   }

   ETextHAlign eLabelHAlign;
   ETextVAlign eLabelVAlign;
   ETextHAlign eStatHAlign;
   ETextVAlign eStatVAlign;

   switch (m_eLabelAlignment)
   {
      case ELabelAlignment_Top:
      {
         eLabelHAlign = ETextHAlign_Center;
         eLabelVAlign = ETextVAlign_Top;

         eStatHAlign = ETextHAlign_Center;
         eStatVAlign = ETextVAlign_Bottom;
      }
      break;
      case ELabelAlignment_Bottom:
      {
         eLabelHAlign = ETextHAlign_Center;
         eLabelVAlign = ETextVAlign_Bottom;

         eStatHAlign = ETextHAlign_Center;
         eStatVAlign = ETextVAlign_Top;
      }
      break;
      case ELabelAlignment_Left:
      {
         eLabelHAlign = ETextHAlign_Left;
         eLabelVAlign = ETextVAlign_Top;

         eStatHAlign = ETextHAlign_Right;
         eStatVAlign = ETextVAlign_Top;
      }
      break;
      case ELabelAlignment_Right:
      {
         eLabelHAlign = ETextHAlign_Right;
         eLabelVAlign = ETextVAlign_Top;

         eStatHAlign = ETextHAlign_Left;
         eStatVAlign = ETextVAlign_Top;
      }
      break;
   }

   if (!InitTextWidget(&m_LabelTextWidget, eLabelHAlign, eLabelVAlign, rsElementaryError))
      return false;

   if (!InitTextWidget(&m_StatTextWidget, eStatHAlign, eStatVAlign, rsElementaryError))
      return false;

   return true;
}

bool CStatWidget::InitTextWidget(CTextWidget* pWidget, ETextHAlign eLabelHAlign, ETextVAlign eLabelVAlign, CHTString& rsError)
{
   pWidget->SetTop(m_iWidgetTop);
   pWidget->SetLeft(m_iWidgetLeft);
   pWidget->SetWidth(m_iWidgetWidth);
   pWidget->SetHeight(m_iWidgetHeight);
   pWidget->SetHAlign(eLabelHAlign);
   pWidget->SetVAlign(eLabelVAlign);
   pWidget->SetShouldWrap(false);
   return pWidget->SetFont("fonts/menu.ttf", 20, rsError);
}

