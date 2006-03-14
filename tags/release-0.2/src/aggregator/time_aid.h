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

int GetCurrentYear();
int GetCurrentMonth(); /* 0-based */
int GetCurrentWeek(); /* 0-based */
int GetCurrentDay(); /* 0-based */
int GetCurrentHour(); /* 0-based */
int GetCurrentMinute(); /* 0-based */
int GetCurrentSecond(); /* 0-based */

int GetWeekFromTime(tm* pTime);
int GetWeekFromMDY(int iMonth, int iDay, int iYear);

void IncrementTime(int& riHour, int& riMinute, int& riSecond, int iNumSecondsToIncrement);

bool IsWithinTimePeriod(int iHour, int iMinute, int iSecond, int iTimePeriodInSeconds);

void TestTimeFunctions();

