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

CHTString GenerateTextFromCharacters(CHTString sCharacters, int iTargetLength, bool bRepeat, bool bUseSpaces);

// Returns false if sFileWithStrings is invalid or if file I\O error
bool GenerateTextFromStrings(CHTString sFileWithStrings, CHTString sPossibleCharacters, int iTargetLength, bool bRandom, OUT CHTString& rsText, CHTString& rsError);

// Only uses strings out of asStrings that only contain characters in
// sPossibleCharacters.  If sPossibleCharacters is blank, uses all.
CHTString GenerateTextFromStrings(CHTStringArray asStrings, CHTString sPossibleCharacters, int iTargetLength, bool bRandom);

