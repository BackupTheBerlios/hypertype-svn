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

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "font_aid.h"
#include "OS.h"
#include "string_aid.h"
#include "SDL_aid.h"

SDL_Surface* LoadImage(CHTString sImagePath, CHTString& rsLoadError)
{
   MungerDataFilePath(sImagePath);
   SDL_Surface* pTmpImage = IMG_Load(sImagePath);
   SDL_Surface* pFinalImage = NULL;
   if (pTmpImage)
   {
      pFinalImage = SDL_DisplayFormatAlpha(pTmpImage);
      SDL_FreeSurface(pTmpImage);
   }
   else
   {
      rsLoadError = CHTString("Unable to load image: ") + SDL_GetError();
      cout << "Warning: " << rsLoadError << endl;
   }
   return pFinalImage;
}

TTF_Font* LoadFont(CHTString sFontPath, int iTextHeight, CHTString& rsLoadError)
{
   MungerDataFilePath(sFontPath);
   TTF_Font* pFont = TTF_OpenFont(sFontPath, iTextHeight);
   if (!pFont)
   {
      rsLoadError = SDL_GetError();
      cout << "Warning: " << rsLoadError << endl;
   }

   return pFont;
}

bool CanWrapText(CHTString sText, int iWrapOffset)
{
   if (iWrapOffset >= sText.Length()-1)
      return true;

   if (sText[iWrapOffset+1] == ' ' ||
       sText[iWrapOffset+1] == '\r' ||
       sText[iWrapOffset+1] == '\n' ||
       sText[iWrapOffset+1] == '\t')
   {
      return true;
   }
   else
      return false;
}

int CalcTextWidth(TTF_Font* pTextFont, CHTString sText)
{
   int iWidth = 0;
   int iHeight = 0;
   TTF_SizeText(pTextFont, sText, &iWidth, &iHeight);
   return iWidth;
}

bool CalcTextWidth(CHTString sFontPath, int iTextSize, CHTString sText, OUT int& riWidth, OUT CHTString& rsError)
{
   TTF_Font* pFont = GetOrLoadFont(sFontPath, iTextSize, rsError);
   if (!pFont)
      return false;

   riWidth = CalcTextWidth(pFont, sText);
   return true;
}

CHTString CalcTextLine(CHTString sEntireText, TTF_Font* pTextFont, int iMaxWidth)
{
   // TODO: Remove spaces when wrapping
   int iLastWrapPos = -1;

   for (int iCharPos = 0; iCharPos < sEntireText.Length(); iCharPos++)
   {
      // always break on line-feed
      if (sEntireText[iCharPos] == '\n' || sEntireText[iCharPos] == '\r')
      {
         iLastWrapPos = iCharPos;
         break;
      }

      // can wrap at this location?
      if (CanWrapText(sEntireText, iCharPos))
      {
         // wrap pos is end of line; get current line
         CHTString sCurAttempt = sEntireText.Left(iCharPos+1);

         // fits within max width?
         if (CalcTextWidth(pTextFont, sCurAttempt) < iMaxWidth)
            iLastWrapPos = iCharPos;
         else
            break;
      }
   }

   if (iLastWrapPos == -1)
   {
      // no place to wrap; give whole line
      return sEntireText;
   }
   else
   {
      // wrap position is end of line
      return sEntireText.Left(iLastWrapPos+1);
   }
}

bool DrawText(CHTString sLine, SDL_Surface* pSurface, TTF_Font* pTextFont, SDL_Color vTextColor,
              SDL_Rect& vDrawingRect, ETextHAlign eHAlignment, ETextVAlign eVAlignment)
{
   SDL_Surface* pText = TTF_RenderText_Solid(pTextFont, sLine, vTextColor);
   if (!pText)
      return false;

   switch (eHAlignment)
   {
   case ETextHAlign_Left:
      break;
   case ETextHAlign_Center:
      vDrawingRect.x += max((vDrawingRect.w - pText->w) / 2, 0);
      break;
   case ETextHAlign_Right:
      vDrawingRect.x += max(vDrawingRect.w - pText->w, 0);
      break;
   }

   switch (eVAlignment)
   {
   case ETextVAlign_Top:
      break;
   case ETextVAlign_Center:
      vDrawingRect.y += (vDrawingRect.h - pText->h) / 2;
      break;
   case ETextVAlign_Bottom:
      vDrawingRect.y += vDrawingRect.h - pText->h;
      break;
   }

   vDrawingRect.w = pText->w;
   vDrawingRect.h = pText->h;

   // We don't let SDL_BlitSurface change vDrawingRect, since we already know that it's
   // correct, and SDL_BlitSurface doesn't handle negative x and y values that way we want to.
   SDL_Rect vTmpRect = vDrawingRect;
   SDL_BlitSurface(pText, NULL, pSurface, &vTmpRect);

   SDL_FreeSurface(pText);
   return true;
}

void ClipRect(SDL_Rect vClippingRect, SDL_Rect& rRectToClip)
{
   SDL_Rect vOrigRect = rRectToClip;
   rRectToClip.x = max(rRectToClip.x, vClippingRect.x);
   rRectToClip.y = max(rRectToClip.y, vClippingRect.y);

   rRectToClip.w = min(rRectToClip.x+rRectToClip.w, vClippingRect.x+vClippingRect.w);
   rRectToClip.w = max(rRectToClip.w-rRectToClip.x, 0);

   rRectToClip.h = min(rRectToClip.y+rRectToClip.h, vClippingRect.y+vClippingRect.h);
   rRectToClip.h = max(rRectToClip.h-rRectToClip.y, 0);
}

SDL_Color SDLColorFromRGB(int r, int g, int b)
{
   SDL_Color vColor;
   vColor.r = r;
   vColor.g = g;
   vColor.b = b;

   return vColor;
}

SDL_Rect SDLRectFromDims(int x, int y, int w, int h)
{
   SDL_Rect vRect;
   vRect.x = x;
   vRect.y = y;
   vRect.w = w;
   vRect.h = h;
   return vRect;
}


void BlitClippedSurface(CHTSurface vSourceSurface, CHTSurface vDestSurface, SDL_Rect vAbsClippingRect)
{
   // Validation (out of bounds conditions)
   if (vSourceSurface.m_iSurfaceX + vSourceSurface.m_pSurface->w <= vAbsClippingRect.x) return;
   if (vSourceSurface.m_iSurfaceY + vSourceSurface.m_pSurface->h <= vAbsClippingRect.y) return;
   if (vAbsClippingRect.x + vAbsClippingRect.w <= vSourceSurface.m_iSurfaceX) return;
   if (vAbsClippingRect.y + vAbsClippingRect.h <= vSourceSurface.m_iSurfaceY) return;

   // Calc source rect
   SDL_Rect vSourceRect;
   vSourceRect.x = max(0, max((int)vAbsClippingRect.x, vDestSurface.m_iSurfaceX) - vSourceSurface.m_iSurfaceX);
   vSourceRect.y = max(0, max((int)vAbsClippingRect.y, vDestSurface.m_iSurfaceY) - vSourceSurface.m_iSurfaceY);
   vSourceRect.w = min(vAbsClippingRect.x+vAbsClippingRect.w, min(vSourceSurface.Right(), vDestSurface.Right()))-(vSourceSurface.m_iSurfaceX+vSourceRect.x);
   vSourceRect.h = min(vAbsClippingRect.y+vAbsClippingRect.h, min(vSourceSurface.Bottom(), vDestSurface.Bottom()))-(vSourceSurface.m_iSurfaceY+vSourceRect.y);

   SDL_Rect vDestRect;
   vDestRect.x = max(vSourceSurface.m_iSurfaceX, (int)vAbsClippingRect.x)-vDestSurface.m_iSurfaceX;
   vDestRect.y = max(vSourceSurface.m_iSurfaceY, (int)vAbsClippingRect.y)-vDestSurface.m_iSurfaceY;
   vDestRect.w = vSourceRect.w;
   vDestRect.h = vSourceRect.h;

   SDL_BlitSurface(vSourceSurface.m_pSurface, &vSourceRect, vDestSurface.m_pSurface, &vDestRect);
}


void SetPixel(SDL_Surface* pSurface, int iX, int iY, Uint32 uiColor)
{
   SDL_Rect vLineRect;
   vLineRect.x = iX;
   vLineRect.y = iY;
   vLineRect.w = 1;
   vLineRect.h = 1;
   SDL_FillRect(pSurface, &vLineRect, uiColor);
}

void DrawCircle(SDL_Surface* pSurface, int iCenterX, int iCenterY, int iRadius, Uint32 uiColor)
{
   int iXOffset=0;
   int iYOffset = iRadius;
   int u=1;
   int v=2*iRadius-1;
   int E=0;
   while(iXOffset < iYOffset)
   {
      SetPixel(pSurface, iCenterX+iXOffset, iCenterY+iYOffset, uiColor);
      SetPixel(pSurface, iCenterX+iYOffset, iCenterY-iXOffset, uiColor);
      SetPixel(pSurface, iCenterX-iXOffset, iCenterY-iYOffset, uiColor);
      SetPixel(pSurface, iCenterX-iYOffset, iCenterY+iXOffset, uiColor);
      iXOffset++;
      E+= u;
      u+=2;
      if (v < 2 * E)
      {
         iYOffset--;
         E -= v;
         v -= 2;
      }
      SetPixel(pSurface, iCenterX+iYOffset, iCenterY+iXOffset, uiColor);
      SetPixel(pSurface, iCenterX+iXOffset, iCenterY-iYOffset, uiColor);
      SetPixel(pSurface, iCenterX-iYOffset, iCenterY-iXOffset, uiColor);
      SetPixel(pSurface, iCenterX-iXOffset, iCenterY+iYOffset, uiColor);
   }
}

void DrawFilledCircle(SDL_Surface* pSurface, int iCenterX, int iCenterY, int iCircleRadius, Uint32 uiColor)
{
   for (int iRadius = 1; iRadius <= iCircleRadius; iRadius+=2)
      DrawCircle(pSurface, iCenterX, iCenterY, iRadius, uiColor);
   for (int iRadius = 1; iRadius <= iCircleRadius; iRadius+=2)
      DrawCircle(pSurface, iCenterX+1, iCenterY, iRadius, uiColor);
   for (int iRadius = 1; iRadius <= iCircleRadius; iRadius+=2)
      DrawCircle(pSurface, iCenterX, iCenterY+1, iRadius, uiColor);
   for (int iRadius = 1; iRadius <= iCircleRadius; iRadius+=2)
      DrawCircle(pSurface, iCenterX+1, iCenterY+1, iRadius, uiColor);
}

void SDL_UpdateRects(SDL_Surface* pSurface, const CRectArray& raRectsToUpdate_c)
{
   if (raRectsToUpdate_c.size() == 0)
      return;

   for (int iRect = 0; iRect < raRectsToUpdate_c.size(); iRect++)
   {
      // Limit update calls to screen (Updates either crash or quiety fail on offscreen coordinates)
      int x = max(0, (int)raRectsToUpdate_c[iRect].x);
      int y = max(0, (int)raRectsToUpdate_c[iRect].y);
      int w = min((int)raRectsToUpdate_c[iRect].w, pSurface->w-(int)raRectsToUpdate_c[iRect].x);
      int h = min((int)raRectsToUpdate_c[iRect].h, pSurface->h-(int)raRectsToUpdate_c[iRect].y);
      SDL_UpdateRect(pSurface, x, y, w, h);
   }


/*   SDL_Rect* paRects = new SDL_Rect[raRectsToUpdate_c.size()];

   for (int iRect = 0; iRect < raRectsToUpdate_c.size(); iRect++)
   {
      paRects[iRect] = raRectsToUpdate_c[iRect];
   }i
   delete [] paRects;*/
}

const char aCharConversions_c[][2] =
{
   ';', ':',
   '`', '~',
   '1', '!',
   '2', '@',
   '3', '#',
   '4', '$',
   '5', '%',
   '6', '^',
   '7', '&',
   '8', '*',
   '9', '(',
   '0', ')',
   '-', '_',
   '=', '+',
   '[', '{',
   ']', '}',
   '\\', '|',
   '\'', '"',
   ',', '<',
   '.', '>',
   '/', '?',
};

const int iNumConversions_c = sizeof(aCharConversions_c) / sizeof(char) / 2;

char TranslateKeysymToChar(SDL_keysym vKey)
{
   // If using unicode works, use it.  Otherwise, fall back to the manual handling of shift and Caps Lock keys
   if (vKey.unicode > 0 && vKey.unicode < 0x80)
      return (char)vKey.unicode;

   cout << "Key: " << (char)vKey.unicode << endl;
   if (vKey.mod & KMOD_CAPS)
      cout << "CAPS LOCK!!" << endl;
   if (vKey.mod & KMOD_LSHIFT || vKey.mod & KMOD_RSHIFT || vKey.mod & KMOD_CAPS)
   {
      char aKey = (char)vKey.sym;
      if (isalpha(aKey))
         return toupper(aKey);
   }

   if (vKey.mod & KMOD_LSHIFT || vKey.mod & KMOD_RSHIFT)
   {
      char aKey = (char)vKey.sym;
      for (int iConversion = 0; iConversion < iNumConversions_c; iConversion++)
      {
         if (aKey == aCharConversions_c[iConversion][0])
            return aCharConversions_c[iConversion][1];
      }
   }

   return char(vKey.sym);
}


