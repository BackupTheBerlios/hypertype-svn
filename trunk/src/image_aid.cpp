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

#include "SDL_aid.h"

class CLoadedImage
{
public:
   CHTString m_sImagePath;
   SDL_Surface* m_pImage;
};

class CImageManager
{
public:
   CImageManager() {}
   ~CImageManager();

   SDL_Surface* GetOrLoadImage(CHTString sImageFile, CHTString& rsError);

private:
   CHTArray<CLoadedImage> m_aImages;
} fg_vImageManager;

// END HEADER
/////////////////////////////

/////////////////////////////
// BEGIN CPP

CImageManager::~CImageManager()
{
   for (int iImage = 0; iImage < m_aImages.size(); iImage++)
   {
      SDL_FreeSurface(m_aImages[iImage].m_pImage);
   }
}

SDL_Surface* CImageManager::GetOrLoadImage(CHTString sImageFile, CHTString& rsError)
{
   for (int iImage = 0; iImage < m_aImages.size(); iImage++)
   {
      if (m_aImages[iImage].m_sImagePath.IsSame(sImageFile))
         return m_aImages[iImage].m_pImage;
   }

   CLoadedImage vNewImage;
   vNewImage.m_pImage = LoadImage(sImageFile, rsError);
   if (!vNewImage.m_pImage)
      return false;

   vNewImage.m_sImagePath = sImageFile;
   m_aImages.Add(vNewImage);
   return vNewImage.m_pImage;
}



SDL_Surface* GetOrLoadImage(CHTString sImageFile, CHTString& rsError)
{
   return fg_vImageManager.GetOrLoadImage(sImageFile, rsError);
}
