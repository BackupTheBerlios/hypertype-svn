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

#include "precompiled.h"

#include <getopt.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"
#include "globals.h"
#include "screen.h"
#include "widget_settings_file.h"
#include "file_aid.h"

char* aszDataPaths[] = {
   "./data",
   DATA_PREFIX"/"PACKAGE,
};


CHTString fg_sProgramFolder;

CHTString ExtractDirectoryFromPath(CHTString sPath)
{
   int iLastFoundOffset = 0;
   int iThisFindOffset = 0;
   while (sPath.Find("/", iLastFoundOffset, iThisFindOffset))
      iLastFoundOffset = iThisFindOffset+1;

   return sPath.Left(iLastFoundOffset);
}

CHTString ExtractFileNameFromPath(CHTString sPath)
{
   int iLastFoundOffset = 0;
   int iThisFindOffset = 0;
   while (sPath.Find("/", iLastFoundOffset, iThisFindOffset))
      iLastFoundOffset = iThisFindOffset+1;

   sPath.RemoveAt(0, iLastFoundOffset);
   return sPath;
}

void SetProgramPath(CHTString sProgramPath)
{
   fg_sProgramFolder = ExtractDirectoryFromPath(sProgramPath);
}

CHTString GetProgramFolder()
{
   return fg_sProgramFolder;
}

CHTString GetHomeFolder()
{
   return CHTString(getenv("HOME"));
}

bool CreateDirectory(CHTString sDirectoryPath, int iPermissionsMode)
{
   return (mkdir(sDirectoryPath, iPermissionsMode) != -1);
}

void HandleProgramArguments(int argc, char* const argv[])
{
   SetProgramPath(argv[0]);

   option longopt;
   while (true)
   {
      int iOptionVal = getopt_long(argc, argv, "wd", &longopt, NULL);
      if (iOptionVal == -1)
         break;
      switch (iOptionVal)
      {
      case 'w':
         g_bWindowedMode = true;
         break;
      case 'd':
         g_bDebugMode = true;
         break;
      }
   }
}

bool EstablishInstanceLock()
{
   int g_fdInstanceLock = open(GetSettingsFile(), O_RDONLY);
   return (flock(g_fdInstanceLock, LOCK_EX|LOCK_NB) != -1 || (errno != EWOULDBLOCK && errno != EINTR));
}

void MungerDataFilePath(CHTString& rsPath)
{
   int iNumDataFolders = sizeof(aszDataPaths) / sizeof(char*);
   for (int iDataPath = 0; iDataPath < iNumDataFolders; iDataPath++)
   {
      CHTString sTestPath = CHTString(aszDataPaths[iDataPath]) + "/" + rsPath;
      if (ExistFile(sTestPath))
      {
         rsPath = sTestPath;
         return;
      }
   }
   cout << "Unmungered path: " << rsPath << "!" << endl;
   return;
}
