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

#include "globals.h"
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
   TestDoubleComparison();

   if (!EstablishInstanceLock())
   {
      cout << "Found existing instance running.  Exiting.\n";
      return 0;
   }

   srand(time(NULL));

   /* Initialize video and TTF font support */
   CHTString sError;
   if (!InitializeSDL(sError))
   {
      cout << "Could not initialize SDL:" << sError << endl;
      SDL_Quit();
      return 1;
   }
   
   SDL_EnableUNICODE(SDL_ENABLE);
   int iScreenOptions = SDL_SWSURFACE | SDL_HWPALETTE;
   if (!g_bWindowedMode)
      iScreenOptions |= SDL_FULLSCREEN;
   SDL_Surface* pMainScreen = SDL_SetVideoMode(640, 480, 16, iScreenOptions);

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
