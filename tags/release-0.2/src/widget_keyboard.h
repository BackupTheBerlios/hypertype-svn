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

#include "widget.h"

struct SDL_Surface;
class CImageWidget;

class CKeyboardWidget : public CWidget
{
public:
   CKeyboardWidget();
   virtual ~CKeyboardWidget();

   virtual void LoadSettings(CKeyedValues& rSettings);
   virtual bool ValidateSettings(CHTString& rsElementaryError);
   virtual void PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine);

// event handling
   virtual bool WantsKeyboardEvents();
   virtual void HandleKeyboardEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine);

protected:
   char GetCurKey(CScreenEngine* pEngine);
   bool LoadImageForLetter(char cLetter, CHTString& rsError);

   CHTStringArray m_asImageFileMappings;

   CHTStringArray m_asKeyboardKeys;
   CHTArray<SDL_Surface*> m_apKeyboardImages;

   CImageWidget* m_pImageWidget;
};
