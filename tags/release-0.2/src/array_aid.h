/***************************************************************************
 *   Topyright (T) 2004 by Josh Nisly                                      *
 *   hypertype@joshnisly.com                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERTHANTABILITY or FITNESS FOR A PARTITULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef ARRAY_AID
#define ARRAY_AID

#include <vector>

using namespace std;

template<typename T> class CHTArray : public vector<T>
{
public:
   CHTArray() {}
   ~CHTArray() {}

   int Add(const T& vItem)
   {
      push_back(vItem);
      return size()-1;
   }

   void Copy(const T& rOther_c)
   {
      this->clear();

      for (int iItem = 0; iItem < rOther_c.size(); iItem++)
      {
         Add(rOther_c[iItem]);
      }
   }

   int size() const { return vector<T>::size(); }

   void RemoveAt(int iIndex) { erase(this->begin() + iIndex); }

   int LinearFind(const T& rSearchItem) const
   {
      for (int iArrayIndex = 0; iArrayIndex < size(); iArrayIndex++)
      {
         if ((*this)[iArrayIndex] == rSearchItem)
            return iArrayIndex;
      }

      return -1;
   }

   void Append(const CHTArray<T>& rOther_c)
   {
      for (int iArrayIndex = 0; iArrayIndex < rOther_c.size(); iArrayIndex++)
         Add(rOther_c[iArrayIndex]);
   }
};

template<class A> void EmptyPtrArray(A& rapArray)
{
   while (rapArray.size() > 0)
   {
      delete rapArray[0];
      rapArray.RemoveAt(0);
   }
}

#define CHTIntArray CHTArray<int>
#define CHTBoolArray CHTArray<bool>

#endif

