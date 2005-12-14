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

#include <wx/wx.h>
#include <wx/grid.h>

#include "grid.h"

void PopulateWXGridFromDataGrid(const CDataGrid& rSrcDataGrid, wxGrid& rDestWXGrid)
{
   // Remove old rows, and insert new ones as appropriate
   rDestWXGrid.DeleteCols(0, rDestWXGrid.GetNumberCols());
   rDestWXGrid.DeleteRows(0, rDestWXGrid.GetNumberRows());
   rDestWXGrid.AppendCols(rSrcDataGrid.GetNumberCols());
   rDestWXGrid.AppendRows(rSrcDataGrid.GetNumberRows());
   rDestWXGrid.SetDefaultCellTextColour(wxColour(0, 0, 0));
   rDestWXGrid.SetLabelTextColour(wxColour(0, 0, 0));
   rDestWXGrid.SetRowLabelSize(0);
   rDestWXGrid.EnableDragRowSize(false);

   // Top Headings
   for (int iCol = 0; iCol < rSrcDataGrid.GetNumberCols(); iCol++)
   {
      rDestWXGrid.SetColLabelValue(iCol, wxString(rSrcDataGrid.GetTopHeadingText(iCol), wxConvUTF8));
   }

   for (int iRow = 0; iRow < rSrcDataGrid.GetNumberRows(); iRow++)
   {
      // Side Heading
      rDestWXGrid.SetRowLabelValue(iRow, wxString(rSrcDataGrid.GetSideHeadingText(iRow), wxConvUTF8));

      // Data Row
      for (int iCol = 0; iCol < rSrcDataGrid.GetNumberCols(); iCol++)
      {
         rDestWXGrid.SetCellValue(iRow, iCol, wxString(rSrcDataGrid.GetCellText(iRow, iCol), wxConvUTF8));
         rDestWXGrid.SetReadOnly(iRow, iCol, true);

         wxFont vCellFont = rDestWXGrid.GetCellFont(iRow, iCol);
         if (rSrcDataGrid.IsCellBold(iRow, iCol))
            vCellFont.SetWeight(wxFONTWEIGHT_BOLD);
         if (rSrcDataGrid.IsCellUnderlined(iRow, iCol))
            vCellFont.SetUnderlined(true);
         rDestWXGrid.SetCellFont(iRow, iCol, vCellFont);
      }
   }

   rDestWXGrid.AutoSizeColumns(false);
   rDestWXGrid.AutoSizeRows(false);
}
