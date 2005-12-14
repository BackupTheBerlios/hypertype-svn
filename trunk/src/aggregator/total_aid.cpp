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

#include <assert.h>
#include <limits.h>
#include "total_aid.h"

const double dDefaultDouble_c = INT_MIN-1;

CTotal::CTotal(CHTString sTotalName, EDataType eDataType, ETotalType eTotalType)
{
   m_sName = sTotalName;
   m_eTotalType = eTotalType;
   m_eDataType = eDataType;

   m_bContainsData = false;
   m_dNum1 = dDefaultDouble_c;
   m_dNum2 = dDefaultDouble_c;
   //m_sString;
}

CTotal::~CTotal()
{
}

CHTString CTotal::GetName()
{
   return m_sName;
}

void CTotal::IncrementTotal(const CTotal& rOther)
{
   if (m_eDataType == EDataType_Number)
      IncrementDoubleTotal(rOther.m_dNum1, rOther.m_dNum2);
   else
      IncrementStringTotal(rOther.m_sString);

   m_bContainsData = true;
}

void CTotal::IncrementDoubleTotal(double dNum1, double dNum2)
{
   switch (m_eTotalType)
   {
      case ETotalType_Sum:
         if (m_dNum1 == dDefaultDouble_c)
            m_dNum1 = 0.0;
         m_dNum1 += dNum1;
         break;
      case ETotalType_Average:
         if (m_dNum1 == dDefaultDouble_c)
         {
            m_dNum1 = 0.0;
            m_dNum2 = 0.0;
         }
         m_dNum1 += dNum1;
         m_dNum2 += dNum2;
         break;
      case ETotalType_Min:
         if (m_dNum1 == dDefaultDouble_c)
            m_dNum1 = dNum1;
         m_dNum1 = min(m_dNum1, dNum1);
         break;
      case ETotalType_Max:
         if (m_dNum1 == dDefaultDouble_c)
            m_dNum1 = dNum1;
         m_dNum1 = max(m_dNum1, dNum1);
         break;

      case ETotalType_None:
         m_dNum1 = dNum1;
         break;

      case ETotalType_OnlyIfUnique:
         if (!m_bContainsData)
            m_dNum1 = dNum1;
         else if (m_dNum1 != dNum1)
            m_dNum1 = dDefaultDouble_c;
         break;
   }
}

void CTotal::IncrementDoubleTotal(double dVal)
{
   IncrementDoubleTotal(dVal, 1);
}

void CTotal::IncrementStringTotal(CHTString sVal)
{
   switch (m_eTotalType)
   {
      case ETotalType_Sum:
      case ETotalType_Average:
         assert(false);
         break;
      case ETotalType_Min:
         if (m_sString == "" || m_sString > sVal)
            m_sString = sVal;
         break;
      case ETotalType_Max:
         m_sString = max(m_sString, sVal);
         break;

      case ETotalType_None:
         m_sString = sVal;
         break;

      case ETotalType_OnlyIfUnique:
         if (!m_bContainsData)
            m_sString = sVal;
         else if (m_sString != sVal)
            m_sString = "";
         break;
   }
}

CHTString CTotal::GetStringTotal() const
{
   assert(m_eDataType == EDataType_String);
   return m_sString;
}

double CTotal::GetDoubleTotal() const
{
   assert(m_eDataType == EDataType_Number);
   if (m_eTotalType == ETotalType_Average)
      return m_dNum1 / m_dNum2;

   return m_dNum1;
}

CHTString CTotal::GetDoubleTotalAsString(bool bShowDecimals) const
{
   double dTotal = GetDoubleTotal();
   if (dTotal != dDefaultDouble_c)
      return DoubleToString(dTotal, bShowDecimals ? 2 : 0);
   else
      return "";
}

