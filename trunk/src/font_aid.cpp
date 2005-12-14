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

#include "precompiled.h"

#include "SDL_aid.h"

class CLoadedFont
{
public:
   CHTString m_sFontPath;
   int m_iFontSize;
   TTF_Font* m_pFont;
};

class CFontManager
{
public:
   CFontManager() {}
   ~CFontManager();

   TTF_Font* GetOrLoadFont(CHTString sFontFile, int iFontSize, CHTString& rsError);

private:
   CHTArray<CLoadedFont> m_aFonts;
} fg_vFontManager;

// END HEADER
/////////////////////////////

/////////////////////////////
// BEGIN CPP

CFontManager::~CFontManager()
{
   for (int iFont = 0; iFont < m_aFonts.size(); iFont++)
   {
      TTF_CloseFont(m_aFonts[iFont].m_pFont);
   }
}

TTF_Font* CFontManager::GetOrLoadFont(CHTString sFontFile, int iFontSize, CHTString& rsError)
{
   for (int iFont = 0; iFont < m_aFonts.size(); iFont++)
   {
      if (m_aFonts[iFont].m_sFontPath.IsSame(sFontFile) && m_aFonts[iFont].m_iFontSize == iFontSize)
         return m_aFonts[iFont].m_pFont;
   }

   CLoadedFont vNewFont;
   vNewFont.m_pFont = LoadFont(sFontFile, iFontSize, rsError);
   if (!vNewFont.m_pFont)
      return false;

   vNewFont.m_sFontPath = sFontFile;
   vNewFont.m_iFontSize = iFontSize;
   m_aFonts.Add(vNewFont);
   return vNewFont.m_pFont;
}



TTF_Font* GetOrLoadFont(CHTString sFontFile, int iFontSize, CHTString& rsError)
{
   return fg_vFontManager.GetOrLoadFont(sFontFile, iFontSize, rsError);
}
