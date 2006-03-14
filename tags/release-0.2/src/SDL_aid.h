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

#ifndef SDL_AID
#define SDL_AID

#include "precompiled.h"

#include "string_aid.h"
#include <SDL_ttf.h>

enum ETextHAlign
{
   ETextHAlign_Left,
   ETextHAlign_Center,
   ETextHAlign_Right
};

enum ETextVAlign
{
   ETextVAlign_Top,
   ETextVAlign_Center,
   ETextVAlign_Bottom
};

class SDL_Surface;

// Image should be freed with SDL_FreeSurface
SDL_Surface* LoadImage(CHTString sImagePath, CHTString& rsLoadError);
TTF_Font* LoadFont(CHTString sFontPath, int iTextHeight, CHTString& rsLoadError);

// Returns the portion of sEntireText that can fit in iWidth
CHTString CalcTextLine(CHTString sEntireText, TTF_Font* pTextFont, int iMaxWidth);

int CalcTextWidth(TTF_Font* pTextFont, CHTString sText);
bool CalcTextWidth(CHTString sFontPath, int iTextSize, CHTString sText, OUT int& riWidth, OUT CHTString& rsError);

// Updates vDrawingRect to reflect the rect actually painted
bool DrawText(CHTString sLine, SDL_Surface* pSurface, TTF_Font* pTextFont, SDL_Color vTextColor,
              SDL_Rect& vDrawingRect, ETextHAlign eHAlignment, ETextVAlign eVAlignment);

void ClipRect(SDL_Rect vClippingRect, SDL_Rect& rRectToClip);

SDL_Color SDLColorFromRGB(int r, int g, int b);

SDL_Rect SDLRectFromDims(int x, int y, int w, int h);

class CHTSurface
{
public:
   CHTSurface() {}
   CHTSurface(SDL_Surface* pSurface, int iSurfaceX, int iSurfaceY)
   {
      m_pSurface = pSurface;
      m_iSurfaceX = iSurfaceX;
      m_iSurfaceY = iSurfaceY;
   }

   SDL_Surface* m_pSurface;
   int m_iSurfaceX;
   int m_iSurfaceY;
   int Right() { return m_iSurfaceX + m_pSurface->w; }
   int Bottom() { return m_iSurfaceY + m_pSurface->h; }
};

// vAbsClippingRect defines the bounding rect to paint in.
void BlitClippedSurface(CHTSurface vSourceSurface, CHTSurface vDestSurface, SDL_Rect vAbsClippingRect);

// For the next three functions, the caller is responsible for calling SDL_UpdateRect
void SetPixel(SDL_Surface* pSurface, int iX, int iY, Uint32 uiColor);
void DrawCircle(SDL_Surface* pSurface, int iCenterX, int iCenterY, int iRadius, Uint32 uiColor);
void DrawFilledCircle(SDL_Surface* pSurface, int iCenterX, int iCenterY, int iRadius, Uint32 uiColor);

#define CRectArray CHTArray<SDL_Rect>

void SDL_UpdateRects(SDL_Surface* pSurface, const CRectArray& raRectsToUpdate_c);

char TranslateKeysymToChar(SDL_keysym vKey);

#endif

