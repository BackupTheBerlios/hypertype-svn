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

bool g_bDebugMode = false;

#include <assert.h>
#include <math.h>

#include "globals.h"

CHTString TranslateUpperCharIfNecessary(char aChar)
{
   if (isalpha(aChar) && isupper(aChar))
      return CHTString("upper-") + CHTString(tolower(aChar));

   return aChar;
}

bool IsDoubleEQ(double d1, double d2, double dThreshold)
{
   return (fabs(d1)-fabs(d2)) <= dThreshold;
}

bool IsDoubleLT(double d1, double d2, double dThreshold)
{
   return (d1 - d2) < -dThreshold;
}

bool IsDoubleGT(double d1, double d2, double dThreshold)
{
   return (d1 - d2) > dThreshold;
}

bool IsDoubleLTE(double d1, double d2, double dThreshold)
{
   return (d1 - d2) <= dThreshold;
}

bool IsDoubleGTE(double d1, double d2, double dThreshold)
{
   return (d1 - d2) >= -dThreshold;
}


void TestDoubleComparison()
{
   assert(IsDoubleEQ(10, 10.1, 0.2));
   assert(IsDoubleLT(10, 10.1, 0.05));
   assert(IsDoubleGT(10.1, 10, 0.05));
   assert(!IsDoubleLT(10.3, 10, 0.2));
   assert(!IsDoubleGT(10, 10.3, 0.2));
   assert(IsDoubleLTE(10, 9.96, 0.05));
   assert(IsDoubleGTE(9.97, 10, 0.05));
   assert(!IsDoubleLTE(10, 9.86, 0.05));
   assert(!IsDoubleGTE(9.87, 10, 0.05));
   assert(IsDoubleGTE(20.055, 20.1, 0.05));
}
