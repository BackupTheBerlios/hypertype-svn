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

#include "string_aid.h"

class SDL_Surface;
class CScreenEngine;

enum EMessageBoxType
{
   EMessageBoxType_OK,
   EMessageBoxType_Yes,
   EMessageBoxType_No,
};

enum EMessageBoxResponse
{
   EMessageBoxResponse_OK,
   EMessageBoxResponse_Yes,
   EMessageBoxResponse_No,
};

EMessageBoxResponse MessageBox(CHTString sMessageText, SDL_Surface* pRootSurface, EMessageBoxType eType);

CScreenEngine* GetMessageBoxEngine();

