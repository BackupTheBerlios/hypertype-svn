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

#include <string_aid.h>
#include <array_aid.h>
#include <widget_settings.h>

#include "total_aid.h"

enum EValueComparison
{
   EValueComparison_LessThan,
   EValueComparison_GreaterThan,
   EValueComparison_Equal,
   EValueComparison_NotEqual,
};

class CSubsetComponent;
class CSubsetObject
{
public:
   CSubsetObject();
   ~CSubsetObject();

   void Reset();

   void SetSubsetRequirement(CHTString sField, EValueComparison eComparison, CHTString sValue);
   void SetSubsetRequirement(CHTString sField, EValueComparison eComparison, double dValue);

   bool RecordPassesSubset(const CKeyedValues& rRecord_c) const;

protected:
   CHTArray<CSubsetComponent*> m_apSubsetParts;
};

class CGroupingInfo
{
public:
   CGroupingInfo();

   void Reset();

   void AddGrouping(CHTString sField);

   int GetNumGroupings() const;
   CHTString GetGrouping(int iGrouping) const;
protected:
   CHTStringArray m_asGroupings;
};

class CFieldDefinition
{
public:
   CFieldDefinition(CHTString sName, CHTString sLabel, EDataType eDataType, ETotalType eTotalType, bool bShowDecimals)
   {
      SetFieldName(sName);
      SetFieldLabel(sLabel);
      SetFieldDataType(eDataType);
      SetFieldTotalType(eTotalType);
      SetShowDecimals(bShowDecimals);
   }

   void SetFieldName(CHTString sFieldName) { m_sFieldName = sFieldName; }
   void SetFieldLabel(CHTString sFieldLabel) { m_sFieldLabel = sFieldLabel; }
   void SetFieldDataType(EDataType eDataType) { m_eDataType = eDataType; }
   void SetFieldTotalType(ETotalType eTotalType) { m_eTotalType = eTotalType; }
   void SetShowDecimals(bool bShow) { m_bShowDecimals = bShow; }

   CHTString GetFieldName() const { return m_sFieldName; }
   CHTString GetFieldLabel() const { return m_sFieldLabel; }
   EDataType GetFieldDataType() const { return m_eDataType; }
   ETotalType GetFieldTotalType() const { return m_eTotalType; }
   bool GetShowDecimals() const { return m_bShowDecimals; }

protected:
   ETotalType m_eTotalType;
   EDataType m_eDataType;
   CHTString m_sFieldName;
   CHTString m_sFieldLabel;
   bool m_bShowDecimals;
};

#define CFieldDefinitionArray CHTArray<CFieldDefinition>
#define CKeyedValuesArray CHTArray<CKeyedValues>

class CDataModifier
{
public:
   virtual void OnAddRecord(CKeyedValues& rRecordToAdd)=0;
};

class CDataGrid;
class CDataSection;
class CAggregatorEngine
{
public:
   CAggregatorEngine();
   ~CAggregatorEngine();

   // pDataModifier may be NULL
   bool InitializeFromDataFiles(CHTStringArray asDataFilePaths, CKeyedValuesArray aCorrespondingData, CDataModifier* pDataModifier, CHTString& rsError);

   bool Build(const CFieldDefinitionArray& raFields_c, const CSubsetObject& rSubset_c, const CGroupingInfo& rGrouping_c, CHTString& rsError);

   void ConstructDataGrid(const CFieldDefinitionArray& raFields_c, bool bShowTotalsOnly, CDataGrid& rDataGrid);

protected:
   void ResetBuildData();
   void AddRowsForSection(const CFieldDefinitionArray& raFields_c, CDataSection* pSection, bool bShowTotalsOnly, CDataGrid& rDataGrid);
   void AddRowForTotalArray(const CFieldDefinitionArray& raFields_c, const CTotalArray& raTotals_c, bool bBold, CDataGrid& rDataGrid);
   CDataSection* GetOrAddSectionForRecord(const CKeyedValues& rRecord, CHTString sFieldForGrouping, CDataSection* pParentDataSection);
   void TotalSection(CDataSection* pDataSection, const CFieldDefinitionArray& raFieldsToTotal_c);
   void CombineTotals(CTotalArray& raTotalsToAddTo, CTotalArray aTotalsToAdd, const CFieldDefinitionArray& raFieldsToTotal_c);

   CKeyedValuesArray m_aData;
   CDataSection* m_pRootSection;
};

