/***************************************************************************
 *   Copyright (C) 2005 by Josh Nisly   *
 *   joshn@linsetesh   *
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
#include <time.h>

#include "string_aid.h"
#include "time_aid.h"

tm* GetCurrentTime()
{
   time_t vCurrentTime;
   time(&vCurrentTime);
   return localtime(&vCurrentTime);
}

int GetCurrentYear()
{
   return GetCurrentTime()->tm_year+1900;
}

int GetCurrentWeek()
{
   char aBuffer[100];
   strftime(aBuffer, 100, "%U", GetCurrentTime());
   int iTestWeek;
   assert(StringToInt(aBuffer, iTestWeek));
   if (iTestWeek != GetWeekFromTime(GetCurrentTime()))
      cout << "Weeks didn't match! libc: " << iTestWeek << ". mine: " << GetCurrentWeek() << endl;
   return GetWeekFromTime(GetCurrentTime());
}

int GetCurrentMonth()
{
   return GetCurrentTime()->tm_mon;
}

int GetCurrentDay()
{
   return GetCurrentTime()->tm_mday;
}


int GetCurrentHour()
{
   return GetCurrentTime()->tm_hour;
}

int GetCurrentMinute()
{
   return GetCurrentTime()->tm_min;
}

int GetCurrentSecond()
{
   return GetCurrentTime()->tm_sec;
}

int GetWeekFromTime(tm* pTime)
{
  return (pTime->tm_yday - pTime->tm_wday + 7) / 7;
}

int GetWeekFromMDY(int iMonth, int iDay, int iYear)
{
   tm time;
   memset(&time, '\0', sizeof(time));
   time.tm_year = iYear-1900;
   time.tm_mon = iMonth;
   time.tm_mday = iDay;
   time.tm_sec = 0;
   mktime(&time); // generate year day and weekday
   return GetWeekFromTime(&time);
}

void IncrementTime(int& riHour, int& riMinute, int& riSecond, int iNumSecondsToIncrement)
{
   riSecond += iNumSecondsToIncrement;
   if (riSecond >= 60)
   {
      riMinute += (riSecond / 60);
      riSecond = (riSecond % 60);
   }
   if (riMinute >= 60)
   {
      riHour += (riMinute / 60);
      riMinute = (riMinute % 60);
   }
   if (riHour >= 24)
      riHour = (riHour % 24);
}

bool IsWithinTimePeriod(int iHour, int iMinute, int iSecond, int iTimePeriodInSeconds)
{
   for (int iIncrement = 0; iIncrement <= iTimePeriodInSeconds; iIncrement++)
   {
      int iTestHour = iHour;
      int iTestMinute = iMinute;
      int iTestSecond = iSecond;
      IncrementTime(iTestHour, iTestMinute, iTestSecond, iIncrement);
      if (iTestHour == GetCurrentHour() && iTestMinute == GetCurrentMinute() && iTestSecond == GetCurrentSecond())
         return true;
   }

   return false;
}

void TestTimeFunctions()
{
   int iHour = 10;
   int iMinute = 20;
   int iSecond = 30;
   IncrementTime(iHour, iMinute, iSecond, 20);
   assert(iHour == 10);
   assert(iMinute == 20);
   assert(iSecond == 50);
   IncrementTime(iHour, iMinute, iSecond, 20);
   assert(iHour == 10);
   assert(iMinute == 21);
   assert(iSecond == 10);
   IncrementTime(iHour, iMinute, iSecond, 3600);
   assert(iHour == 11);
   assert(iMinute == 21);
   assert(iSecond == 10);
}
