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

#include "typing_history_aggregator.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <file_aid.h>
#include <time_aid.h>

#include "grid.h"
#include "grid_wx.h"

class CAddWeekModifier : public CDataModifier
{
public:
   CAddWeekModifier() {}

   virtual void OnAddRecord(CKeyedValues& rRecordToAdd)
   {
      int iYear, iMonth, iDay;
      if (rRecordToAdd.GetIntValue("year", iYear) &&
          rRecordToAdd.GetIntValue("month", iMonth) &&
          rRecordToAdd.GetIntValue("day", iDay))
      {
         rRecordToAdd.SetIntValue("week", GetWeekFromMDY(iMonth, iDay, iYear));
      }
   }
};


CTypingHistoryAggregator::CTypingHistoryAggregator()
{
}

CTypingHistoryAggregator::~CTypingHistoryAggregator()
{
   m_bTotalsOnlyLastBuild = false;
}

void CTypingHistoryAggregator::Initialize(CHTString& rsWarning)
{
   // For now, always just initialize list of users from the home folders we have available.
   // Later we might allow it to be passed in as a commandline option.
   LoadUserListFromHomeDirs(rsWarning);
}

void CTypingHistoryAggregator::GetUserFilterOptions(CHTStringArray& rasOptions)
{
   rasOptions.clear();
   rasOptions.Add("All");
   rasOptions.Append(m_asUsers);
}

void CTypingHistoryAggregator::GetDateFilterOptions(CHTStringArray& rasOptions)
{
   rasOptions.clear();
   rasOptions.Add("Today");
   rasOptions.Add("This Week");
   rasOptions.Add("Last Week");
   rasOptions.Add("This Month");
   rasOptions.Add("All");
}

void CTypingHistoryAggregator::GetGroupingOptions(CHTStringArray& rasOptions)
{
   rasOptions.clear();
   rasOptions.Add("None");
   rasOptions.Add("User");
   rasOptions.Add("Day");
   rasOptions.Add("Week");
}

bool CTypingHistoryAggregator::Build_SingleUser(CHTString sUser, CHTString sDateFilter, CHTString& rsError)
{
   return Build_CustomSettings(sUser, sDateFilter, "None", "None", false, rsError);
}

bool CTypingHistoryAggregator::Build_AllUsers(CHTString sDateFilter, CHTString& rsError)
{
   return Build_CustomSettings("All", sDateFilter, "User", "", true, rsError);
}

void AddDateField(CFieldDefinition vField, CFieldDefinitionArray& raFields, CHTString sFirstGrouping, CHTString sSecondGrouping, bool bTotalsOnly)
{
   if ((sFirstGrouping == vField.GetFieldLabel()) != bTotalsOnly)
      return;

   raFields.Add(vField);
}

bool CTypingHistoryAggregator::Build_CustomSettings(CHTString sUserFilter, CHTString sDateFilter, CHTString sFirstGrouping, CHTString sSecondGrouping, bool bTotalsOnly, CHTString& rsError)
{
   // Subset
   m_SubsetForLastBuild.Reset();
   if (sUserFilter != "" && sUserFilter != "All")
      m_SubsetForLastBuild.SetSubsetRequirement("user", EValueComparison_Equal, sUserFilter);

   m_SubsetForLastBuild.SetSubsetRequirement("completion", EValueComparison_GreaterThan, 10);

   AddSubsetRequirementsForDate(sDateFilter);

   m_GroupingsForLastBuild.Reset();
   if (sFirstGrouping != "" && sFirstGrouping != "None")
      m_GroupingsForLastBuild.AddGrouping(sFirstGrouping);
   if (sSecondGrouping != "" && sSecondGrouping != "None")
      m_GroupingsForLastBuild.AddGrouping(sSecondGrouping);

   m_bTotalsOnlyLastBuild = bTotalsOnly;

   CKeyedValuesArray aUserData;
   for (int iFile = 0; iFile < m_asUserDataPaths.size(); iFile++)
   {
      CKeyedValues vUserData;
      vUserData.SetStringValue("User", m_asUsers[iFile]);
      vUserData.SetIntValue("totallessons", 1);
      aUserData.Add(vUserData);
   }

   CAddWeekModifier vModifier;
   if (!m_Aggregator.InitializeFromDataFiles(m_asUserDataPaths, aUserData, &vModifier, rsError))
      return false;

   // Fields
   // TODO: eventually these could be loaded from a definition file...
   m_aFieldsForLastBuild.clear();
   m_aFieldsForLastBuild.Add(CFieldDefinition("User", "User", EDataType_String, ETotalType_OnlyIfUnique, false));
   AddDateField(CFieldDefinition("Week", "Week", EDataType_Number, ETotalType_OnlyIfUnique, false), m_aFieldsForLastBuild, sFirstGrouping, sSecondGrouping, m_bTotalsOnlyLastBuild);
   AddDateField(CFieldDefinition("year", "Year", EDataType_Number, ETotalType_OnlyIfUnique, false), m_aFieldsForLastBuild, sFirstGrouping, sSecondGrouping, m_bTotalsOnlyLastBuild);
   AddDateField(CFieldDefinition("month", "Month", EDataType_Number, ETotalType_OnlyIfUnique, false), m_aFieldsForLastBuild, sFirstGrouping, sSecondGrouping, m_bTotalsOnlyLastBuild);
   AddDateField(CFieldDefinition("day", "Day", EDataType_Number, ETotalType_OnlyIfUnique, false), m_aFieldsForLastBuild, sFirstGrouping, sSecondGrouping, m_bTotalsOnlyLastBuild);
   AddDateField(CFieldDefinition("name", "Lesson", EDataType_String, ETotalType_OnlyIfUnique, false), m_aFieldsForLastBuild, sFirstGrouping, sSecondGrouping, m_bTotalsOnlyLastBuild);
   m_aFieldsForLastBuild.Add(CFieldDefinition("accuracy", "Accuracy\n%", EDataType_Number, ETotalType_Average, true));
   m_aFieldsForLastBuild.Add(CFieldDefinition("completion", "%\nComplete", EDataType_Number, ETotalType_Average, true));
   m_aFieldsForLastBuild.Add(CFieldDefinition("AdjustedWPM", "Speed", EDataType_Number, ETotalType_Average, true));
   m_aFieldsForLastBuild.Add(CFieldDefinition("totallessons", "Total\nLessons", EDataType_Number, ETotalType_Sum, false));

   return m_Aggregator.Build(m_aFieldsForLastBuild, m_SubsetForLastBuild, m_GroupingsForLastBuild, rsError);
}

void CTypingHistoryAggregator::PopulateDataGrid(wxGrid& rGrid)
{
   CDataGrid vMemGrid;
   m_Aggregator.ConstructDataGrid(m_aFieldsForLastBuild, m_bTotalsOnlyLastBuild, vMemGrid);
   PopulateWXGridFromDataGrid(vMemGrid, rGrid);
}

void CTypingHistoryAggregator::LoadUserListFromHomeDirs(CHTString& rsWarning)
{
   CHTString sError;
   CHTStringArray asDirEntries;
   if (!GetDirectoryListing("/home", asDirEntries, sError))
   {
      cout << sError << endl;
      return;
   }

   for (int iEntry = 0; iEntry < asDirEntries.size(); iEntry++)
   {
      CHTString sDirEntry = asDirEntries[iEntry];
      if (sDirEntry == "." || sDirEntry == "..")
         continue;
      CHTString sFullFilePath = "/home/" + sDirEntry + "/.hypertype/lesson_history.xml";
      // TODO: we could eventually check against the specific "file does not exist" error here
      struct stat ignored;
      if (stat(sFullFilePath, &ignored) == 0)
      {
         m_asUserDataPaths.Add(sFullFilePath);
         m_asUsers.Add(sDirEntry);
      }
   }
}

void CTypingHistoryAggregator::AddSubsetRequirementsForDate(CHTString sDateFilter)
{
   // TODO: figure out some way to support the week grouping option
   if (sDateFilter == "All")
      return;

   if (sDateFilter == "Today")
   {
      m_SubsetForLastBuild.SetSubsetRequirement("year", EValueComparison_Equal, GetCurrentYear());
      m_SubsetForLastBuild.SetSubsetRequirement("month", EValueComparison_Equal, GetCurrentMonth());
      m_SubsetForLastBuild.SetSubsetRequirement("day", EValueComparison_Equal, GetCurrentDay());
   }

   if (sDateFilter == "This Week" || sDateFilter == "Last Week")
   {
      int iWeek = sDateFilter == "This Week" ? GetCurrentWeek() : GetCurrentWeek()-1;

      m_SubsetForLastBuild.SetSubsetRequirement("year", EValueComparison_Equal, GetCurrentYear());
      m_SubsetForLastBuild.SetSubsetRequirement("week", EValueComparison_Equal, iWeek);
   }

   if (sDateFilter == "This Month")
   {
      m_SubsetForLastBuild.SetSubsetRequirement("year", EValueComparison_Equal, GetCurrentYear());
      m_SubsetForLastBuild.SetSubsetRequirement("month", EValueComparison_Equal, GetCurrentMonth());
   }
}

