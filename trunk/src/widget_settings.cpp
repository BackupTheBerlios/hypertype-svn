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

#include "widget_settings.h"

#include "xml_aid.h"

CKeyedValues::CKeyedValues()
{
   Reset();
}

CKeyedValues::~CKeyedValues()
{
}

void CKeyedValues::Reset()
{
   m_asKeys.clear();
   m_asValues.clear();
}

void CKeyedValues::CopyFromOther(const CKeyedValues& rOther_c, bool bOverwriteConflicting)
{
   for (int iOtherSetting = 0; iOtherSetting < rOther_c.m_asKeys.size(); iOtherSetting++)
   {
      if (!HasSetting(rOther_c.m_asKeys[iOtherSetting]) || bOverwriteConflicting)
         SetStringValue(rOther_c.m_asKeys[iOtherSetting], rOther_c.m_asValues[iOtherSetting]);
   }
}

void CKeyedValues::LoadFromXml(xmlNodePtr pXmlSource)
{
   while (true)
   {
      AdvanceToElementNode(pXmlSource);
      if (pXmlSource == NULL)
         return;

      m_asKeys.Add((char*)pXmlSource->name);
      if (pXmlSource->children)
         m_asValues.Add((char*)pXmlSource->children->content);
      else
         m_asValues.Add("");

      pXmlSource = pXmlSource->next;
   }
}

bool CKeyedValues::SaveToXmlFile(CHTString sFile, CHTString& rsError)
{
   rsError = "Unimplemented function.";
   return false;
}

bool CKeyedValues::HasSetting(CHTString sKey) const
{
   return (m_asKeys.LinearFind(sKey) != -1);
}

bool CKeyedValues::GetStringValue(CHTString sKey, CHTString& rsValue, CHTString& rsElementaryError) const
{
   if (!HasSetting(sKey))
   {
      rsElementaryError = "The \"" + sKey + "\" value was not specified, or was specified incorrectly.";
      return false;
   }

   int iArrayIndex = m_asKeys.LinearFind(sKey);
   rsValue = m_asValues[iArrayIndex];
   return true;
}

bool CKeyedValues::GetIntValue(CHTString sKey, int& riNum, CHTString& rsElementaryError) const
{
   CHTString sValue;
   if (!GetStringValue(sKey, sValue, rsElementaryError))
      return false;

   if (!StringToInt(sValue, riNum))
   {
      rsElementaryError = "The value for setting \"" + sKey + "\" is not a valid integer.";
      return false;
   }

   return true;
}

bool CKeyedValues::GetDoubleValue(CHTString sKey, double& rdNum, CHTString& rsElementaryError) const
{
   CHTString sValue;
   if (!GetStringValue(sKey, sValue, rsElementaryError))
      return false;

   if (!StringToDouble(sValue, rdNum))
   {
      rsElementaryError = "The value for setting \"" + sKey + "\" is not a valid double.";
      return false;
   }

   return true;
}

bool CKeyedValues::GetBoolValue(CHTString sKey, bool& rbBool, CHTString& rsElementaryError) const
{
   CHTString sValue;
   if (!GetStringValue(sKey, sValue, rsElementaryError))
      return false;

   if (!StringToBool(sValue, rbBool))
   {
      rsElementaryError = "The value for setting \"" + sKey + "\" is not a valid boolean.";
      return false;
   }

   return true;
}

bool CKeyedValues::GetStringValue(CHTString sKey, CHTString& rsValue) const
{
   CHTString sIgnored;
   return GetStringValue(sKey, rsValue, sIgnored);
}

bool CKeyedValues::GetIntValue(CHTString sKey, int& riNum) const
{
   CHTString sIgnored;
   return GetIntValue(sKey, riNum, sIgnored);
}

bool CKeyedValues::GetDoubleValue(CHTString sKey, double& rdNum) const
{
   CHTString sIgnored;
   return GetDoubleValue(sKey, rdNum, sIgnored);
}

bool CKeyedValues::GetBoolValue(CHTString sKey, bool& rbBool) const
{
   CHTString sIgnored;
   return GetBoolValue(sKey, rbBool, sIgnored);
}


void CKeyedValues::SetStringValue(CHTString sKey, CHTString sValue)
{
   int iArrayIndex = m_asKeys.LinearFind(sKey);
   if (iArrayIndex == -1)
   {
      m_asKeys.Add(sKey);
      m_asValues.Add(sValue);
   }
   else
      m_asValues[iArrayIndex] = sValue;
}

void CKeyedValues::SetIntValue(CHTString sKey, int iNum)
{
   CHTString sValue = IntToHTString(iNum);
   SetStringValue(sKey, sValue);
}

void CKeyedValues::SetBoolValue(CHTString sKey, bool bBool)
{
   CHTString sValue = BoolToString(bBool);
   SetStringValue(sKey, sValue);
}

void CKeyedValues::SetDoubleValue(CHTString sKey, double dNum)
{
   CHTString sValue = DoubleToString(dNum);
   SetStringValue(sKey, sValue);
}

