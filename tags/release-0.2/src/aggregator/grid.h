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

#include <array_aid.h>
#include <string_aid.h>


class CDataGrid
{
public:
   CDataGrid();
   ~CDataGrid();

   void Initialize(int iNumCols);

   // returns new row number
   int AddRow();

   int GetNumberCols() const;
   int GetNumberRows() const;

   void SetCellText(int iRow, int iCol, CHTString sText);
   void SetCellBold(int iRow, int iCol, bool bBold);
   void SetCellUnderline(int iRow, int iCol, bool bUnderline);

   void SetSideHeadingText(int iRow, CHTString sText);
   void SetSideHeadingBold(int iRow, bool bBold);
   void SetTopHeadingText(int iCol, CHTString sText);
   void SetTopHeadingBold(int iCol, bool bBold);

   CHTString GetCellText(int iRow, int iCol) const;
   bool IsCellBold(int iRow, int iCol) const;
   bool IsCellUnderlined(int iRow, int iCol) const;
   CHTString GetSideHeadingText(int iRow) const;
   bool GetSideHeadingBold(int iRow) const;
   CHTString GetTopHeadingText(int iCol) const;
   bool GetTopHeadingBold(int iCol) const;

protected:
   int m_iNumCols;

   class CDataRow
   {
   public:
      CHTString m_sSideHeading;
      bool m_bSideHeadingBold;
      CHTStringArray m_aColText;
      CHTArray<bool> m_aColBold;
      CHTArray<bool> m_aColUnderlined;
   };

   CHTStringArray m_asHeadings;
   CHTBoolArray m_abBoldHeadings;
   CHTArray<CDataRow> m_aDataRows;
};

