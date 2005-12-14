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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "lesson.h"
#include "OS.h"
#include "screen.h"
#include "string_aid.h"
#include "menu.h"

using namespace std;

int main(int argc, char *argv[])
{
   HandleProgramArguments(argc, argv);

   TestCHTString();

   srand(time(NULL));

   /* Initialize video and TTF font support */
   if(SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0)
   {
      cout << "Could not initialize SDL:" << SDL_GetError() << endl;
      SDL_Quit();
      return 1;
   }
   int iScreenOptions = SDL_SWSURFACE | SDL_HWPALETTE;
   if (!g_bWindowedMode)
      iScreenOptions |= SDL_FULLSCREEN;
   SDL_Surface* pMainScreen = SDL_SetVideoMode(640, 480, 16, iScreenOptions);

   CHTString sError;
   CScreenIterator* pMainIterator = GetMenuIterator();
   if (!pMainIterator->Init(pMainScreen, "", sError) ||
       !pMainIterator->DisplayIteration(sError))
   {
      cout << sError << endl;
   }

   delete(pMainIterator);
   SDL_FreeSurface(pMainScreen);
   SDL_Quit();

   return 0;
}
