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

#include "aggregator_engine.h"

class wxGrid;
class CTypingHistoryAggregator
{
public:
   CTypingHistoryAggregator();
   ~CTypingHistoryAggregator();

   // 1. Call Initialize() on app startup
   void Initialize(CHTString& rsWarning);

   // 2. Populate controls based on returned options
   void GetUserFilterOptions(CHTStringArray& rasOptions);
   void GetDateFilterOptions(CHTStringArray& rasOptions);
   void GetGroupingOptions(CHTStringArray& rasOptions);

   // 3. Give user selections to Build functions
   bool Build_SingleUser(CHTString sUser, CHTString sDateFilter, CHTString& rsError);
   bool Build_AllUsers(CHTString sDateFilter, CHTString& rsError);
   bool Build_CustomSettings(CHTString sUserFilter, CHTString sDateFilter, CHTString sFirstGrouping, CHTString sSecondGrouping, bool bTotalsOnly, CHTString& rsError);

   // 4. Give grid to be filled with built data
   void PopulateDataGrid(wxGrid& rGrid);

   // 5. Refresh to rebuild data with previous build's parameters
   bool Refresh(CHTString& rsError);

protected:
   void LoadUserListFromHomeDirs(CHTString& rsWarning);

   void AddSubsetRequirementsForDate(CHTString sDateFilter);

   CHTStringArray m_asUsers;
   CHTStringArray m_asUserDataPaths;

   CAggregatorEngine m_Aggregator;
   CSubsetObject m_SubsetForLastBuild;
   CGroupingInfo m_GroupingsForLastBuild;
   CFieldDefinitionArray m_aFieldsForLastBuild;
   bool m_bTotalsOnlyLastBuild;
};
