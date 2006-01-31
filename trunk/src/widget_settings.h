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

#ifndef _WIDGET_SETTINGS_H_
#define _WIDGET_SETTINGS_H_

#include "array_aid.h"
#include "string_aid.h"
#include <libxml/tree.h>

// This class is basically to fascilitate random access via key/value
// pairs to an xml file.
class CKeyedValues
{
public:
   CKeyedValues();
   ~CKeyedValues();

   void Reset();

   // Adds all settings from rOther_c.  If both objects have different settings,
   // rOther_c's settings wins if bOverwriteConflicting is true
   void CopyFromOther(const CKeyedValues& rOther_c, bool bOverwriteConflicting);

   void LoadFromXml(xmlNodePtr pXmlSource);
   bool LoadFromConfFile(CHTString sConfFile, CHTString& rsError);

   bool SaveToXmlFile(CHTString sFile, CHTString& rsError);
   bool SaveToConfFile(CHTString sFile, CHTString& rsError);

   bool HasSetting(CHTString sKey) const;

   // The following functions return false if setting does not exist or if setting is not the correct data type
   bool GetStringValue(CHTString sKey, CHTString& rsValue, CHTString& rsElementaryError) const;
   bool GetIntValue(CHTString sKey, int& riNum, CHTString& rsElementaryError) const;
   bool GetDoubleValue(CHTString sKey, double& rdNum, CHTString& rsElementaryError) const;
   bool GetBoolValue(CHTString sKey, bool& rbBool, CHTString& rsElementaryError) const;

   bool GetStringValue(CHTString sKey, CHTString& rsValue) const;
   bool GetIntValue(CHTString sKey, int& riNum) const;
   bool GetDoubleValue(CHTString sKey, double& rdNum) const;
   bool GetBoolValue(CHTString sKey, bool& rbBool) const;

   void SetStringValue(CHTString sKey, CHTString sValue);
   void SetIntValue(CHTString sKey, int iNum);
   void SetDoubleValue(CHTString sKey, double dNum);
   void SetBoolValue(CHTString sKey, bool bBool);

protected:
   CHTArray<CHTString> m_asKeys;
   CHTArray<CHTString> m_asValues;
};


#endif
