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
#include <xml_aid.h>

#include "aggregator_engine.h"
#include "grid.h"

class CSubsetComponent
{
public:
   CSubsetComponent(CHTString sField, EValueComparison eComparison, CHTString sValue);
   CSubsetComponent(CHTString sField, EValueComparison eComparison, double dValue);

   bool SubsetComponentMatchesRecord(const CKeyedValues& rRecord_c);

private:
   bool m_bStringComponent;

   CHTString m_sField;
   CHTString m_sValue;
   double m_dValue;
   EValueComparison m_eComparison;
};

#define CDataSectionArray CHTArray<CDataSection>
class CDataSection
{
public:
   CDataSection(CHTString sSectionName, CHTString sSectionLabel, bool bHasTotalRow);
   ~CDataSection();

   CHTString GetSectionName();
   CHTString GetSectionLabel();

   CDataSectionArray& GetSubSections();

   void SetRowData(CTotalArray aData);
   bool HasRowData() { return m_aRowData.size() > 0; }
   void GetRowData(/*OUT*/CTotalArray& raData);

   bool HasTotalRow();
   void GetTotalRowData(/*OUT*/CTotalArray& rData);

   void SetTotalRowData(CTotalArray aData);

protected:
   CHTString m_sName;
   CHTString m_sLabel;

   CDataSectionArray m_aSubSections;

   CTotalArray m_aRowData;
   CTotalArray m_aTotalData;

   bool m_bHasTotalRow;
};

void FillTotalsArray(const CFieldDefinitionArray& raFields, CKeyedValues vFieldValues, CTotalArray& raTotalArrayToFill);

// END HEADER
////////////////////////////////////////

////////////////////////////////////////
// BEGIN CPP

CSubsetObject::CSubsetObject()
{
}

CSubsetObject::~CSubsetObject()
{
   Reset();
}


void CSubsetObject::Reset()
{
   EmptyPtrArray(m_apSubsetParts);
}

void CSubsetObject::SetSubsetRequirement(CHTString sField, EValueComparison eComparison, CHTString sValue)
{
   CSubsetComponent* pNewComponent = new CSubsetComponent(sField, eComparison, sValue);
   m_apSubsetParts.Add(pNewComponent);
}

void CSubsetObject::SetSubsetRequirement(CHTString sField, EValueComparison eComparison, double dValue)
{
   CSubsetComponent* pNewComponent = new CSubsetComponent(sField, eComparison, dValue);
   m_apSubsetParts.Add(pNewComponent);
}

bool CSubsetObject::RecordPassesSubset(const CKeyedValues& rRecord_c) const
{
   for (int iComponent = 0; iComponent < m_apSubsetParts.size(); iComponent++)
   {
      if (!m_apSubsetParts[iComponent]->SubsetComponentMatchesRecord(rRecord_c))
         return false;
   }

   return true;
}

CGroupingInfo::CGroupingInfo()
{
   Reset();
}

void CGroupingInfo::Reset()
{
   m_asGroupings.clear();
}

void CGroupingInfo::AddGrouping(CHTString sField)
{
   m_asGroupings.Add(sField);
}

int CGroupingInfo::GetNumGroupings() const
{
   return m_asGroupings.size();
}

CHTString CGroupingInfo::GetGrouping(int iGrouping) const
{
   return m_asGroupings[iGrouping];
}


CAggregatorEngine::CAggregatorEngine()
{
   m_pRootSection = NULL;
}

CAggregatorEngine::~CAggregatorEngine()
{
   ResetBuildData();
}

bool CAggregatorEngine::InitializeFromDataFiles(CHTStringArray asDataFilePaths, CHTArray<CKeyedValues> aCorrespondingData, CDataModifier* pDataModifier, CHTString& rsError)
{
   m_aData.clear();
   for (int iDataFile = 0; iDataFile < asDataFilePaths.size(); iDataFile++)
   {
      xmlDoc* pDoc = xmlReadFile(asDataFilePaths[iDataFile], NULL, 0);
      if (!pDoc)
      {
         rsError = "Unable to read data file '" + asDataFilePaths[iDataFile] + "'.";
         return false;
      }

      xmlNode* pRootElem = xmlDocGetRootElement(pDoc);
      xmlNode* pDataElem = pRootElem->children;

      while (true)
      {
         AdvanceToElementNode(pDataElem);
         if (!pDataElem)
            break;

         CKeyedValues vRecordData;
         vRecordData.LoadFromXml(pDataElem->children);
         vRecordData.CopyFromOther(aCorrespondingData[iDataFile], false);

         if (pDataModifier) pDataModifier->OnAddRecord(vRecordData);

         m_aData.Add(vRecordData);

         pDataElem = pDataElem->next;
      }

      xmlFreeDoc(pDoc);
   }

   return true;
}

bool CAggregatorEngine::Build(const CFieldDefinitionArray& raFields_c, const CSubsetObject& rSubset_c, const CGroupingInfo& rGrouping_c, CHTString& rsError)
{
   ResetBuildData();
   m_pRootSection = new CDataSection("", "", true);

   // Run through data, moving it into the appropriate groups
   for (int iDataRecord = 0; iDataRecord < m_aData.size(); iDataRecord++)
   {
      const CKeyedValues& rCurRecord_c = m_aData[iDataRecord];
      if (!rSubset_c.RecordPassesSubset(rCurRecord_c))
         continue;

      CDataSection* pDataSection = m_pRootSection;
      for (int iGroup = 0; iGroup < rGrouping_c.GetNumGroupings(); iGroup++)
      {
         CHTString sGroupField = rGrouping_c.GetGrouping(iGroup);
         pDataSection = GetOrAddSectionForRecord(rCurRecord_c, sGroupField, pDataSection);
      }
      if (!pDataSection)
         continue;

      CTotalArray aRecordData;
      FillTotalsArray(raFields_c, rCurRecord_c, aRecordData);

      CDataSection vSection("", "", false);
      vSection.SetRowData(aRecordData);
      pDataSection->GetSubSections().Add(vSection);
   }

   // Run through newly popuplated groups, totalling data
   TotalSection(m_pRootSection, raFields_c);
   return true;
}

void CAggregatorEngine::ResetBuildData()
{
   if (m_pRootSection)
      delete m_pRootSection;
   m_pRootSection = NULL;
}

void CAggregatorEngine::AddRowForTotalArray(const CFieldDefinitionArray& raFields_c, const CTotalArray& raTotals_c, bool bBold, CDataGrid& rDataGrid)
{
   int iNewRow = rDataGrid.AddRow();
   for (int iField = 0; iField < raFields_c.size(); iField++)
   {
      CHTString sCellText;
      if (raFields_c[iField].GetFieldDataType() == EDataType_String)
         sCellText = raTotals_c[iField].GetStringTotal();
      else
      {
         sCellText = raTotals_c[iField].GetDoubleTotalAsString(raFields_c[iField].GetShowDecimals());
      }

      rDataGrid.SetCellText(iNewRow, iField, sCellText);
      rDataGrid.SetCellBold(iNewRow, iField, bBold);
   }
}

void CAggregatorEngine::AddRowsForSection(const CFieldDefinitionArray& raFields_c, CDataSection* pSection, bool bShowTotalsOnly, CDataGrid& rDataGrid)
{
   if (!pSection->HasRowData() && pSection->GetSectionName() != "" && !bShowTotalsOnly)
   {
      int iHeaderRow = rDataGrid.AddRow();
      rDataGrid.SetCellText(iHeaderRow, 0, pSection->GetSectionName());
      rDataGrid.SetCellBold(iHeaderRow, 0, true);
      rDataGrid.SetCellUnderline(iHeaderRow, 0, true);
   }

   for (int iSubSection = 0; iSubSection < pSection->GetSubSections().size(); iSubSection++)
   {
      AddRowsForSection(raFields_c, &pSection->GetSubSections()[iSubSection], bShowTotalsOnly, rDataGrid);
   }

   if (!bShowTotalsOnly)
   {
      CTotalArray aRowData;
      pSection->GetRowData(aRowData);
      if (aRowData.size() > 0)
         AddRowForTotalArray(raFields_c, aRowData, false, rDataGrid);
   }

   if (!pSection->HasRowData())
   {
      CTotalArray aTotalData;
      pSection->GetTotalRowData(aTotalData);
      if (aTotalData.size() > 0)
         AddRowForTotalArray(raFields_c, aTotalData, true, rDataGrid);
   }
}

void CAggregatorEngine::ConstructDataGrid(const CFieldDefinitionArray& raFields_c, bool bShowTotalsOnly, CDataGrid& rDataGrid)
{
   rDataGrid.Initialize(raFields_c.size());

   // Top Heading
   for (int iField = 0; iField < raFields_c.size(); iField++)
   {
      rDataGrid.SetTopHeadingText(iField, raFields_c[iField].GetFieldLabel());
   }

   // Side Heading

   // Main cells
   AddRowsForSection(raFields_c, m_pRootSection, bShowTotalsOnly, rDataGrid);
}

CDataSection* CAggregatorEngine::GetOrAddSectionForRecord(const CKeyedValues& rRecord, CHTString sFieldForGrouping, CDataSection* pParentDataSection)
{
   CHTString sFieldValue;
   if (!rRecord.GetStringValue(sFieldForGrouping, sFieldValue))
      return NULL;

   for (int iSubSection = 0; iSubSection < pParentDataSection->GetSubSections().size(); iSubSection++)
   {
      if (pParentDataSection->GetSubSections()[iSubSection].GetSectionName() == sFieldValue)
         return &pParentDataSection->GetSubSections()[iSubSection];
   }

   CDataSection vNewDataSection(sFieldValue, sFieldForGrouping, true);
   pParentDataSection->GetSubSections().Add(vNewDataSection);
   return &pParentDataSection->GetSubSections()[pParentDataSection->GetSubSections().size()-1];
}

void CAggregatorEngine::TotalSection(CDataSection* pDataSection, const CFieldDefinitionArray& raFieldsToTotal_c)
{
   CTotalArray aRunningTotals;

   CDataSectionArray& raSubSections_c = pDataSection->GetSubSections();
   for (int iSubSection = 0; iSubSection < raSubSections_c.size(); iSubSection++)
   {
      TotalSection(&raSubSections_c[iSubSection], raFieldsToTotal_c);

      CTotalArray aSubTotals;
      if (raSubSections_c[iSubSection].HasRowData())
         raSubSections_c[iSubSection].GetRowData(aSubTotals);
      else
         raSubSections_c[iSubSection].GetTotalRowData(aSubTotals);
      CombineTotals(aRunningTotals, aSubTotals, raFieldsToTotal_c);
   }

   pDataSection->SetTotalRowData(aRunningTotals);
}

void CAggregatorEngine::CombineTotals(CTotalArray& raTotalsToAddTo, CTotalArray aTotalsToAdd, const CFieldDefinitionArray& raFieldsToTotal_c)
{
   for (int iTotal = 0; iTotal < raFieldsToTotal_c.size(); iTotal++)
   {
      int iSrcArrayPos = -1;
      for (int iSrcTotal = 0; iSrcTotal < aTotalsToAdd.size(); iSrcTotal++)
      {
         if (aTotalsToAdd[iSrcTotal].GetName() == raFieldsToTotal_c[iTotal].GetFieldName())
            iSrcArrayPos = iSrcTotal;
      }
      if (iSrcArrayPos == -1)
         continue;

      int iDestArrayPos = -1;
      for (int iDestTotal = 0; iDestTotal < raTotalsToAddTo.size(); iDestTotal++)
      {
         if (raTotalsToAddTo[iDestTotal].GetName() == raFieldsToTotal_c[iTotal].GetFieldName())
            iDestArrayPos = iDestTotal;
      }
      if (iDestArrayPos == -1)
      {
         CTotal vBlankTotal(raFieldsToTotal_c[iTotal].GetFieldName(), raFieldsToTotal_c[iTotal].GetFieldDataType(), raFieldsToTotal_c[iTotal].GetFieldTotalType());
         raTotalsToAddTo.Add(vBlankTotal);
         iDestArrayPos = raTotalsToAddTo.size()-1;
      }

      if (raFieldsToTotal_c[iTotal].GetFieldTotalType() != ETotalType_None)
         raTotalsToAddTo[iDestArrayPos].IncrementTotal(aTotalsToAdd[iSrcArrayPos]);
   }
}



///////////////////////////////////////////////////////////////////////////////////////////////////

CSubsetComponent::CSubsetComponent(CHTString sField, EValueComparison eComparison, CHTString sValue)
{
   m_bStringComponent = true;
   m_sField = sField;
   m_sValue = sValue;
   m_dValue = 0;
   m_eComparison = eComparison;
}

CSubsetComponent::CSubsetComponent(CHTString sField, EValueComparison eComparison, double dValue)
{
   m_bStringComponent = false;
   m_sField = sField;
   m_sValue = "";
   m_dValue = dValue;
   m_eComparison = eComparison;
}

bool CSubsetComponent::SubsetComponentMatchesRecord(const CKeyedValues& rRecord_c)
{
   if (m_bStringComponent)
   {
      CHTString sFieldValue;
      if (!rRecord_c.GetStringValue(m_sField, sFieldValue))
         return false;

      switch (m_eComparison)
      {
         case EValueComparison_GreaterThan:
            return sFieldValue > m_sValue;
         case EValueComparison_LessThan:
            return sFieldValue < m_sValue;
         case EValueComparison_Equal:
            return sFieldValue == m_sValue;
         case EValueComparison_NotEqual:
            return sFieldValue != m_sValue;
      }
   }
   else
   {
      double dFieldValue;
      if (!rRecord_c.GetDoubleValue(m_sField, dFieldValue))
         return false;

      switch (m_eComparison)
      {
         case EValueComparison_GreaterThan:
            return dFieldValue > m_dValue;
         case EValueComparison_LessThan:
            return dFieldValue < m_dValue;
         case EValueComparison_Equal:
            return dFieldValue == m_dValue;
         case EValueComparison_NotEqual:
            return dFieldValue != m_dValue;
      }
   }

   assert(false);
   return false;
}

CDataSection::CDataSection(CHTString sSectionName, CHTString sSectionLabel, bool bHasTotalRow)
{
   m_sName = sSectionName;
   m_sLabel = sSectionLabel;
   m_bHasTotalRow = bHasTotalRow;
}

CDataSection::~CDataSection()
{
}

CHTString CDataSection::GetSectionName()
{
   return m_sName;
}

CHTString CDataSection::GetSectionLabel()
{
   return m_sLabel;
}

CDataSectionArray& CDataSection::GetSubSections()
{
   return m_aSubSections;
}

void CDataSection::SetRowData(CTotalArray aData)
{
   m_aRowData = aData;
}

void CDataSection::GetRowData(/*OUT*/CTotalArray& raData)
{
   raData = m_aRowData;
}

bool CDataSection::HasTotalRow()
{
   return m_bHasTotalRow;
}

void CDataSection::GetTotalRowData(/*OUT*/CTotalArray& rData)
{
   rData = m_aTotalData;
}

void CDataSection::SetTotalRowData(CTotalArray aData)
{
   m_aTotalData = aData;
}

void FillTotalsArray(const CFieldDefinitionArray& raFields, CKeyedValues vFieldValues, CTotalArray& raTotalArrayToFill)
{
   for (int iField = 0; iField < raFields.size(); iField++)
   {
      CFieldDefinition vFieldDef = raFields[iField];
      CTotal vTotal(vFieldDef.GetFieldName(), vFieldDef.GetFieldDataType(), vFieldDef.GetFieldTotalType());
      if (vFieldDef.GetFieldDataType() == EDataType_String)
      {
         CHTString sFieldValue;
         if (vFieldValues.GetStringValue(vFieldDef.GetFieldName(), sFieldValue))
            vTotal.IncrementStringTotal(sFieldValue);
      }
      else
      {
         double dFieldValue;
         if (vFieldValues.GetDoubleValue(vFieldDef.GetFieldName(), dFieldValue))
            vTotal.IncrementDoubleTotal(dFieldValue);
      }
      raTotalArrayToFill.Add(vTotal);
   }
}

