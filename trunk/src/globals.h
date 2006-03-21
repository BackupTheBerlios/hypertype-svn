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

extern bool g_bDebugMode;

#include "string_aid.h"

// Translates 'M' to 'upper-m'.  If aChar is not an uppercase letter, it is returned unmodified.
CHTString TranslateUpperCharIfNecessary(char aChar);

bool IsDoubleEQ(double d1, double d2, double dThreshold);
bool IsDoubleLT(double d1, double d2, double dThreshold);
bool IsDoubleGT(double d1, double d2, double dThreshold);
bool IsDoubleLTE(double d1, double d2, double dThreshold);
bool IsDoubleGTE(double d1, double d2, double dThreshold);

void TestDoubleComparison();
