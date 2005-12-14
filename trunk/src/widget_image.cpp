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
#include <SDL_video.h>

#include "image_aid.h"
#include "screen.h"
#include "SDL_aid.h"
#include "string_aid.h"
#include "widget_image.h"
#include "widget_settings.h"

CImageWidget::CImageWidget() : CWidget()
{
   m_pImage = NULL;
}

CImageWidget::~CImageWidget()
{
}

void CImageWidget::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   CWidget::OnPaintWidget(pPaintingParms->m_pDrawingSurface);
   CHTSurface vImageSurface;
   vImageSurface.m_iSurfaceX = m_iWidgetLeft;
   vImageSurface.m_iSurfaceY = m_iWidgetTop;
   vImageSurface.m_pSurface = m_pImage;

   CHTSurface vDestSurface;
   vDestSurface.m_iSurfaceX = 0;
   vDestSurface.m_iSurfaceY = 0;
   vDestSurface.m_pSurface = pPaintingParms->m_pDrawingSurface;

   BlitClippedSurface(vImageSurface, vDestSurface, pPaintingParms->m_vClippingRect);
   pPaintingParms->m_aRectsToUpdate.Add(GetWidgetRect());
}

void CImageWidget::SetImage(SDL_Surface* pImage)
{
   m_pImage = pImage;
   if (m_pImage)
   {
      m_iWidgetWidth = m_pImage->w;
      m_iWidgetHeight = m_pImage->h;
   }
   else
   {
      m_iWidgetWidth = -1;
      m_iWidgetHeight = -1;
   }
}

void CImageWidget::LoadSettings(CKeyedValues& rSettings)
{
   LoadRectSettings(rSettings);
   CHTString sIgnoredError;
   (void)rSettings.GetStringValue("image", m_sImagePath, sIgnoredError);
}

bool CImageWidget::ValidateSettings(CHTString& rsElementaryError)
{
   if (!ValidateRectSettings(false, rsElementaryError))
      return false;
   SetImage(GetOrLoadImage(m_sImagePath, rsElementaryError));
   if (!m_pImage)
      return false;

   return true;
}
