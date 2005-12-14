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

#include "widget_lesson_progress.h"
#include "screen.h"
#include "SDL_aid.h"

CLessonProgressWidget::CLessonProgressWidget()
{
}

CLessonProgressWidget::~CLessonProgressWidget()
{
}

void CLessonProgressWidget::LoadSettings(CKeyedValues& rSettings)
{
   LoadRectSettings(rSettings);
}

bool CLessonProgressWidget::ValidateSettings(CHTString& rsElementaryError)
{
   return ValidateRectSettings(true, rsElementaryError);
}

void CLessonProgressWidget::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   int iNumberLessons = pEngine->GetIntValue("NumberLessons", false);
   int iCurrentLesson = pEngine->GetIntValue("CurrentLesson", false);
   if (iNumberLessons <= 1)
      return; // don't bother showing progress indicator if there's only a single lesson
   int iCircleRadius_c = 8;
   int iDistanceBetweenCircles = (m_iWidgetWidth-iCircleRadius_c*2)/(iNumberLessons-1);

   // Draw connecting line
   SDL_Rect vLineRect;
   vLineRect.x = m_iWidgetLeft+iCircleRadius_c;
   vLineRect.y = (m_iWidgetTop + (m_iWidgetHeight/2));
   vLineRect.w = (iNumberLessons-1)*iDistanceBetweenCircles;
   vLineRect.h = 3;
   SDL_FillRect(pPaintingParms->m_pDrawingSurface, &vLineRect, 0);

   // Draw circles over line
   for (int iCircle = 0; iCircle < iNumberLessons; iCircle++)
   {
      // Use red for current lesson, dark green for other lessons
      bool bCurrentLesson = (iCircle == iCurrentLesson-1);
      Uint32 uiColor;
      if (bCurrentLesson)
         uiColor = SDL_MapRGB(pPaintingParms->m_pDrawingSurface->format, 255 , 0, 0);
      else
         uiColor = SDL_MapRGB(pPaintingParms->m_pDrawingSurface->format, 0, 190, 128);

      int iHorizOffset = m_iWidgetLeft+(iCircle*iDistanceBetweenCircles)+iCircleRadius_c;
      DrawFilledCircle(pPaintingParms->m_pDrawingSurface, iHorizOffset,
                       vLineRect.y+1, 8, uiColor);
   }
   pPaintingParms->m_aRectsToUpdate.Add(GetWidgetRect());
}

