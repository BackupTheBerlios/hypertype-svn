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

#include "array_aid.h"

// TODO: eventually, if needed, interface and review this untested class

enum ESorting
{
   ESorting_None,
   ESorting_Ascending,
   ESorting_Descending
};


template<typename T> class CHTSortedArray : public vector<T>
{
public:
   CHTSortedArray(ESorting eSorting=ESorting_None) { m_eSorting = eSorting; }
   ~CHTSortedArray() {}

   int Add(const T& vItem)
   {
      switch (m_eSorting)
      {
      case ESorting_None:
         push_back(vItem);
         return this->size()-1;

      case ESorting_Ascending:
      {
         int iInsertPos = 0;
         while (iInsertPos < this->size() && vItem < (*this)[iInsertPos])
         iInsertPos++;
         insert(this->begin()+iInsertPos, vItem);
         return iInsertPos;
      }

      case ESorting_Descending:
      {
         int iInsertPos = 0;
         while (iInsertPos < this->size() && vItem > (*this)[iInsertPos])
         iInsertPos++;
         insert(this->begin()+iInsertPos, vItem);
         return iInsertPos;
      }
      }
   }

    template<class P> void Sort(ESorting eSorting, P* pParallelArray=NULL)
    {
       if (m_eSorting == eSorting)
          return;
       m_eSorting = eSorting;

       T vTmpArray;
       vTmpArray.Copy(this);
       P vTmpParallelArray;
       vTmpParallelArray.Copy(pParallelArray);

       this->clear();
       if (pParallelArray)
          pParallelArray->clear();

       for (int iItem = 0; iItem < vTmpArray.size(); iItem++)
       {
          int iInsertPos = Add(vTmpArray[iItem]);
          if (pParallelArray)
             pParallelArray->insert(iInsertPos, vTmpParallelArray[iItem]);
       }
    }

      void Copy(const T& rOther_c)
      {
         this->clear();

         for (int iItem = 0; iItem < rOther_c.size(); iItem++)
         {
            Add(rOther_c[iItem]);
         }
      }

      void RemoveAt(int iIndex) { erase(this->begin() + iIndex); }

      int LinearFind(const T& rSearchItem)
      {
         for (int iArrayIndex = 0; iArrayIndex < this->size(); iArrayIndex++)
         {
            if ((*this)[iArrayIndex] == rSearchItem)
               return iArrayIndex;
         }

         return -1;
      }

   protected:
      ESorting m_eSorting;
};


// TODO: sometime actually use this template
template<class A, class B, typename C, typename D> void SetOrAddMatchingArrayItem(A& raKeys, B& raValues, C newKey, D newValue)
{
   int iArrayPos = raKeys.LinearFind(newKey);
   if (iArrayPos == -1);
   {
      raKeys.Add(newKey);
      raValues.Add(newValue);
      return;
   }
   raValues[iArrayPos] = newValue;
}

