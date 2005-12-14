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

#include "widget_settings_file.h"
#include "file_aid.h"
#include "OS.h"

bool CFileKeyedValues::LoadFromConfFile(CHTString sConfFile, CHTString& rsError)
{
   CTextFile vFile;
   if (!vFile.Open(sConfFile, false, false, rsError))
   {
      rsError = "Unable to open configuration file: "+rsError;
      return false;
   }

   CHTStringArray asLines;
   if (!vFile.SlurpFile(asLines, rsError) || !vFile.Close(rsError))
   {
      rsError = "Unable to read configuration file: "+rsError;
      return false;
   }
   for (int iLine = 0; iLine < asLines.size(); iLine++)
   {
      CHTString sLine = asLines[iLine];
      if (sLine == "" || sLine.StartsWith("#")) continue;

      int iEquals;
      if (!sLine.Find("=", iEquals))
      {
         rsError = "The configuration file is in an invalid format.  The correct format is 'key=setting'.";
         return false;
      }

      m_asKeys.Add(sLine.Left(iEquals));
      m_asValues.Add(sLine.Mid(iEquals+1));
   }

   return true;
}

bool CFileKeyedValues::SaveToConfFile(CHTString sFile, CHTString& rsError)
{
   CTextFile vFile;
   if (!vFile.Open(sFile, true, false, rsError))
      return false;

   for (int iSetting = 0; iSetting < m_asKeys.size(); iSetting++)
   {
      if (!vFile.AppendLine(m_asKeys[iSetting]+"="+m_asValues[iSetting], rsError))
         return false;
   }

   if (!vFile.Close(rsError))
      return false;

   return true;
}

CHTString GetSettingsDir()
{
   return GetHomeFolder() + "/.hypertype";
}
