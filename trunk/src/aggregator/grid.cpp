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

#include "grid.h"

CDataGrid::CDataGrid()
{
   m_iNumCols = 0;
}

CDataGrid::~CDataGrid()
{
}

void CDataGrid::Initialize(int iNumCols)
{
   m_iNumCols = iNumCols;
   m_asHeadings.resize(iNumCols, "");
}

int CDataGrid::AddRow()
{
   CDataRow vNewRow;
   vNewRow.m_sSideHeading = "";
   vNewRow.m_bSideHeadingBold = false;
   vNewRow.m_aColText.resize(m_iNumCols, "");
   vNewRow.m_aColBold.resize(m_iNumCols, false);
   vNewRow.m_aColUnderlined.resize(m_iNumCols, false);
   m_aDataRows.Add(vNewRow);
   return m_aDataRows.size()-1;
}

int CDataGrid::GetNumberCols() const
{
   return m_iNumCols;
}

int CDataGrid::GetNumberRows() const
{
   return m_aDataRows.size();
}

void CDataGrid::SetCellText(int iRow, int iCol, CHTString sText)
{
   m_aDataRows[iRow].m_aColText[iCol] = sText;
}

void CDataGrid::SetCellBold(int iRow, int iCol, bool bBold)
{
   m_aDataRows[iRow].m_aColBold[iCol] = bBold;
}

void CDataGrid::SetCellUnderline(int iRow, int iCol, bool bUnderline)
{
   m_aDataRows[iRow].m_aColUnderlined[iCol] = bUnderline;
}

void CDataGrid::SetSideHeadingText(int iRow, CHTString sText)
{
   m_aDataRows[iRow].m_sSideHeading = sText;
}

void CDataGrid::SetSideHeadingBold(int iRow, bool bBold)
{
   m_aDataRows[iRow].m_bSideHeadingBold = bBold;
}

void CDataGrid::SetTopHeadingText(int iCol, CHTString sText)
{
   m_asHeadings[iCol] = sText;
}

void CDataGrid::SetTopHeadingBold(int iCol, bool bBold)
{
   m_abBoldHeadings[iCol] = bBold;
}


CHTString CDataGrid::GetCellText(int iRow, int iCol) const
{
   return m_aDataRows[iRow].m_aColText[iCol];
}

bool CDataGrid::IsCellBold(int iRow, int iCol) const
{
   return m_aDataRows[iRow].m_aColBold[iCol];
}

bool CDataGrid::IsCellUnderlined(int iRow, int iCol) const
{
   return m_aDataRows[iRow].m_aColUnderlined[iCol];
}

CHTString CDataGrid::GetSideHeadingText(int iRow) const
{
   return m_aDataRows[iRow].m_sSideHeading;
}

bool CDataGrid::GetSideHeadingBold(int iRow) const
{
   return m_aDataRows[iRow].m_bSideHeadingBold;
}

CHTString CDataGrid::GetTopHeadingText(int iCol) const
{
   return m_asHeadings[iCol];
}

bool CDataGrid::GetTopHeadingBold(int iCol) const
{
   return m_abBoldHeadings[iCol];
}


