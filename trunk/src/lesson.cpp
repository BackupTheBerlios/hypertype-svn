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

#include <assert.h>
#include <SDL_timer.h>

#include "file_aid.h"
#include "game.h"
#include "globals.h"
#include "lesson.h"
#include "message_box.h"
#include "OS.h"
#include "screen.h"
#include "text_generation.h"
#include "widget_settings.h"
#include "widget_settings_file.h"
#include "xml_aid.h"

enum ELessonType
{
   ELessonType_Uninit,
   ELessonType_None,
   ELessonType_NewKeys,
   ELessonType_Practice,
};

class CLessonEngine : public CScreenEngine
{
public:
   CLessonEngine();
   virtual ~CLessonEngine();

   virtual bool Load(CKeyedValues* pSettings, CHTString& rsError);

   virtual void OnKey(SDL_keysym vKey);
   virtual void OnMouse(SDL_Event vMouse);
   virtual void OnTimer();

protected:
   virtual void OnScreenStart(SDL_Surface* pScreen, bool& rbScreenNeedsRepaint);
   virtual void OnScreenEnd(SDL_Surface* pScreen);

   bool SaveLessonPerformance(CHTString& rsError);
   bool LoadAndStoreKeyDescs(CHTString sKeyDescsFile, CHTString sLessonKeys, CHTString& rsError);
   void SplitLessonTextIntoLines(CHTString sLessonText, CHTString sTypedText, int iLineLength, CHTStringArray& rasLessonLines, CHTStringArray& rasTypedLines);
   bool LastKeyCorrect();
   void HandleKeystroke(SDL_keysym vKey);
   void RegisterCorrectKey(CHTString sCorrectKey);
   void RegisterIncorrectKey(CHTString sCorrectKey);
   void GenerateProblemKeys();
   void MungerLessonMessage(CHTString& rsMessage);

   // Includes lesson and typed text and performance stats
   void UpdateLessonStats();

   int m_iLessonStart;
   int m_iLastLessonUpdate;

   // These are parallel arrays.  m_aiMistakesMade holds the number mistakes
   // made for each key in m_asMistakeKeys
   CHTStringArray m_asMistakeKeys;
   CHTArray<int> m_aiMistakesMade;
};

class CLessonIterator : public CScreenIterator
{
public:
   CLessonIterator();
   virtual ~CLessonIterator();

protected:
   virtual bool Iterator_Init(CHTString sSettingsFile, CHTString& rsError);
   virtual CScreenEngine* Iterator_GetFirstScreen(OUT CHTString& rsDisplayFile, CHTString& rsError);
   virtual CScreenEngine* Iterator_GetNextScreen(CScreenEngine* pPrevScreen, OUT CHTString& rsDisplayFile, CHTString& rsError);
   virtual bool Iterator_OnScreenEnd(CScreenEngine* pScreen);

   CHTString GetLessonSetName() { return ExtractFileNameFromPath(m_sRelativeSettingsFile); }

   // Persistent settings functions
   void SaveLessonSetAsLast();
   void SaveLessonAsLast(CHTString sLesson);
   void SaveLessonAsFurthestCompleted(CHTString sLesson);

   bool GetLastLessonSetIfExists(CHTString& rsLessonSetName);
   bool GetLastLessonIfExists(CHTString& rsLessonName);
   bool GetFurthestCompletedLessonIfExists(CHTString& rsLessonName);

   bool ShouldShowProblemKeysForNextLesson(CScreenEngine* pPrevScreen);
   bool MaySkipPastLesson(int iLessonPos);

   // Lesson load functions (any of these may return NULL if the position/name/etc does not exist.)
   // This is normal, and means we are done with this lesson iteration.  rsError will be empty.
   // If the lesson load fails or another error occurs, rsError will be filled in, and the program
   // should exit.
   CLessonEngine* LoadLessonByPos(int iPos, OUT CHTString& rsDisplayFile, OUT CHTString& rsError);
   CLessonEngine* LoadLessonByName(CHTString sName, OUT CHTString& rsDisplayFile, OUT CHTString& rsError);
   CLessonEngine* LoadProblemKeysLesson(CHTString sProblemKeys, OUT CHTString& rsDisplayFile, OUT CHTString& rsError);
   CLessonEngine* Internal_LoadLesson(CKeyedValues* pSettings, IN CHTString sProblemKeys, OUT CHTString& rsDisplayFile, OUT CHTString& rsError);
   // LoadNextLesson behaves like LoadLessonByPos, except that if the lesson is before the start or after the end,
   // it clears the "LastLesson" setting in the config file.
   CLessonEngine* LoadNextLesson(int iNextLessonPos, OUT CHTString& rsDisplayFile, OUT CHTString& rsError);
   bool FindLessonPos(CHTString sLessonName, OUT int& riLessonPos);

   CHTString m_sLessonPriorToProblemKeys;
   CKeyedValues m_vGlobalSettings;
   CKeyedValues m_vProblemKeysLesson;
   CHTArray<CKeyedValues*> m_apLessons;
   CHTString m_sRelativeSettingsFile;
   CHTString m_sAbsoluteSettingsFile;
};

class CBookTextIterator : public CScreenIterator
{
public:
   CBookTextIterator();
   virtual ~CBookTextIterator();

protected:
   virtual bool Iterator_Init(CHTString sSettingsFile, CHTString& rsError);
   virtual CScreenEngine* Iterator_GetFirstScreen(OUT CHTString& rsDisplayFile, CHTString& rsError);
   virtual CScreenEngine* Iterator_GetNextScreen(CScreenEngine* pPrevScreen, OUT CHTString& rsDisplayFile, CHTString& rsError);
   virtual bool Iterator_OnScreenEnd(CScreenEngine* pScreen);

protected:
   bool GetCompletedLessons(OUT int& riNumLessons);
   bool SetCompletedLessons(int iNumLessons, CHTString& rsError);

   double GetMinSpeed(double dDefaultMinSpeed);
   bool SetMinSpeed(double dMinSpeed, CHTString& rsError);

   int m_iLessonLength;
   double m_dMinSpeedRequirement;
   double m_dSpeedIncreaseAmount;
   CHTString m_sBookCompletedMessage;
   CKeyedValues m_LessonSettings;
   CHTStringArray m_asLessonTexts;
   CHTString m_sDisplayFile;
   CHTString m_sSettingsFile;
};

// END HEADER

// BEGIN CPP

CHTString GetLessonHistoryPath()
{
   return GetSettingsDir() + "/lesson_history.xml";
}

ELessonType TextToLessonType(CHTString sText)
{
   if (IsSameNoCase(sText, "none"))
      return ELessonType_None;
   else if (IsSameNoCase(sText, "newkeys"))
      return ELessonType_NewKeys;
   else if (IsSameNoCase(sText, "practice"))
      return ELessonType_Practice;
   else
      return ELessonType_Uninit;
}

bool GetSettingFromSettingsFile(CHTString sKey, OUT CHTString& rsValue)
{
   // Get last lesson
   if (ExistFile(GetSettingsFile()))
   {
      CHTString sIgnoredError;
      CFileKeyedValues vConfSettings;
      if (!vConfSettings.LoadFromConfFile(GetSettingsFile(), sIgnoredError))
         return false;

      return vConfSettings.GetStringValue(sKey, rsValue, sIgnoredError);
   }

   return false;
}

bool SaveSettingToSettingsFile(CHTString sKey, CHTString sValue, CHTString& rsError)
{
   CFileKeyedValues vSettings;
   if (ExistFile(GetSettingsFile()) && !vSettings.LoadFromConfFile(GetSettingsFile(), rsError)) // don't try to load settings if the settings file doesn't exist
      return false;

   CHTString sIgnoredError;
   vSettings.SetStringValue(sKey, sValue);

   if (!vSettings.SaveToConfFile(GetSettingsFile(), rsError))
      return false;

   return true;
}


CLessonEngine::CLessonEngine()
{
   m_iLessonStart = 0;
   m_iLastLessonUpdate = 0;
}

CLessonEngine::~CLessonEngine()
{
}

bool CLessonEngine::Load(CKeyedValues* pSettings, CHTString& rsError)
{
   CHTString sType;
   if (!pSettings->GetStringValue("type", sType, rsError))
      return false;
   SetStringValue("type", sType);

   CHTString sName;
   if (!pSettings->GetStringValue("name", sName, rsError))
      return false;
   SetStringValue("name", sName);

   CHTString sFailureLesson;
   if (!pSettings->GetStringValue("FailureLesson", sFailureLesson, rsError))
   {
      bool bRestartOnFailure = true;
      if (pSettings->GetBoolValue("RestartOnFailure", bRestartOnFailure, rsError) && bRestartOnFailure)
         sFailureLesson = sName;
   }
   SetStringValue("FailureLesson", sFailureLesson);

   bool bProblemKeysLessonAvailable = false;
   (void)pSettings->GetBoolValue("ProblemKeysLesson", bProblemKeysLessonAvailable, rsError);
   SetBoolValue("ProblemKeysLesson", bProblemKeysLessonAvailable);

   ELessonType eLessonType = TextToLessonType(sType);
   switch (eLessonType)
   {
   case ELessonType_None:
   case ELessonType_Uninit:
      rsError = "Invalid lesson type.";
      return false;

   case ELessonType_Practice:
   case ELessonType_NewKeys:
   {
      // Lesson Text
      {
         CHTString sLessonKeys;
         (void)pSettings->GetStringValue("keys", sLessonKeys, rsError);

         CHTString sLessonText;
         CHTString sGenerationStyle;
         if (pSettings->GetStringValue("LessonText", sLessonText, rsError))
         {
            // We have our text, get out
         }
         else
         {
            // We're apparently supposed to generate our text.  Make sure they gave us a generation style.
            if (!pSettings->GetStringValue("TextGenerationStyle", sGenerationStyle, rsError))
               return false;

            int iLessonLength = 0;
            if (!pSettings->GetIntValue("length", iLessonLength, rsError))
               return false;

            SetIntValue("Length", iLessonLength);

            if (sGenerationStyle == "random")
            {
               CHTString sProblemKeys = GetStringValue("ProblemKeys", false);
               if (sProblemKeys != "")
                  sLessonKeys = sProblemKeys;
               if (sLessonKeys == "")
               {
                  rsError = "Keys must be specified for 'random' generation-style lessons.";
                  return false;
               }

               sLessonText = GenerateTextFromCharacters(sLessonKeys, iLessonLength, false, (eLessonType == ELessonType_Practice));
            }
            else if (sGenerationStyle == "repetition")
            {

               sLessonText = GenerateTextFromCharacters(sLessonKeys, iLessonLength, true, (eLessonType == ELessonType_Practice));
            }
            else if (sGenerationStyle == "words")
            {
               CHTString sWordListPath;
               if (!pSettings->GetStringValue("WordListPath", sWordListPath, rsError))
                  return false;

               MungerDataFilePath(sWordListPath);
               if (!GenerateTextFromStrings(sWordListPath, sLessonKeys, iLessonLength, true/*always random for now*/, sLessonText, rsError))
                  return false;
            }
            else if (sGenerationStyle == "phrases")
            {
               CHTString sPhraseListPath;
               if (!pSettings->GetStringValue("PhraseListPath", sPhraseListPath, rsError))
                  return false;

               MungerDataFilePath(sPhraseListPath);
               if (!GenerateTextFromStrings(sPhraseListPath, sLessonKeys, iLessonLength, true/*always random for now*/, sLessonText, rsError))
                  return false;
            }
         }

         // Store the lesson text in a convienient place for for the widgets
         SetStringValue("LessonText", sLessonText);
         SetStringValue("TypedText", "");

         // Key descriptions
         if (eLessonType == ELessonType_NewKeys)
         {
            CHTString sKeyDescsFile;
            if (!pSettings->GetStringValue("KeyDescs", sKeyDescsFile, rsError) ||
                 !LoadAndStoreKeyDescs(sKeyDescsFile, sLessonKeys, rsError))
            {
               return false;
            }
         }
      }

      int iMinAccuracy = 0;
      if (pSettings->GetIntValue("MinAccuracy", iMinAccuracy, rsError))
      {
         CHTString sAccuracyError;
         if (!pSettings->GetStringValue("AccuracyError", sAccuracyError, rsError))
            return false;

         SetIntValue("MinAccuracy", iMinAccuracy);
         SetStringValue("AccuracyError", sAccuracyError);
      }

      double dMinSpeed = 0;
      if (pSettings->GetDoubleValue("MinSpeed", dMinSpeed, rsError))
      {
         CHTString sSpeedError;
         if (!pSettings->GetStringValue("SpeedError", sSpeedError, rsError))
            return false;

         CHTString sMinSpeedAdjustRatio;
         if (GetSettingFromSettingsFile("SpeedAdjustmentRatio", sMinSpeedAdjustRatio))
         {
            double dMinSpeedAdjustRatio = 1.0;
            if (!StringToDouble(sMinSpeedAdjustRatio, dMinSpeedAdjustRatio) || dMinSpeedAdjustRatio < 0 || dMinSpeedAdjustRatio > 10)
            {
               rsError = "The 'RequiredSpeedAdjustmentRatio' setting must be a valid, positive number.";
               return false;
            }
            dMinSpeed *= dMinSpeedAdjustRatio;
         }
         SetDoubleValue("MinSpeed", dMinSpeed);
         SetStringValue("SpeedError", sSpeedError);
      }

      int iLineLength = 0;
      if (pSettings->GetIntValue("LineLength", iLineLength, rsError))
      {
         SetIntValue("LineLength", iLineLength);
      }
      else if (eLessonType == ELessonType_Practice)
      {
         rsError = "A line length must be specified for practice lessons.";
         return false;
      }
   }
   }

   // Get and store messages
   CHTString sIgnored;
   CHTString sPreLessonMessage;
   pSettings->GetStringValue("PreLessonMessage", sPreLessonMessage, sIgnored);
   SetStringValue("PreLessonMessage", sPreLessonMessage);
   CHTString sPostLessonMessage;
   pSettings->GetStringValue("PostLessonMessage", sPostLessonMessage, sIgnored);
   SetStringValue("PostLessonMessage", sPostLessonMessage);

   CHTString sMistakeHandling;
   if (!pSettings->GetStringValue("mistakeHandling", sMistakeHandling, rsError))
      return false;

   EMistakeHandling eHandling;
   if (!TextToMistakeHandling(sMistakeHandling, eHandling))
   {
      rsError = "Invalid value for mistake handling setting.";
      return false;
   }
   SetStringValue("mistakehandling", sMistakeHandling);

   UpdateLessonStats();

   rsError = "";
   return true;
}

void CLessonEngine::OnKey(SDL_keysym vKey)
{
   HandleKeystroke(vKey);
   UpdateLessonStats();
}

void CLessonEngine::OnMouse(SDL_Event vMouse)
{
}

void CLessonEngine::OnTimer()
{
   if (SDL_GetTicks() - m_iLastLessonUpdate >= 100)
      UpdateLessonStats();
}

void CLessonEngine::OnScreenStart(SDL_Surface* pScreen, bool& rbScreenNeedsRepaint)
{
   rbScreenNeedsRepaint = false;

   CHTString sMessage = GetStringValue("PreLessonMessage", false);
   MungerLessonMessage(sMessage);
   if (sMessage != "")
   {
      MessageBox(sMessage, pScreen, EMessageBoxType_OK);
      rbScreenNeedsRepaint = true;
   }

   UpdateLessonStats();
}

void CLessonEngine::OnScreenEnd(SDL_Surface* pScreen)
{
   CHTString sError;
   if (!SaveLessonPerformance(sError))
   {
      cout << sError << "\n";
      assert(false);
   }

   GenerateProblemKeys();

   CHTString sMessage = GetStringValue("PostLessonMessage", false);
   MungerLessonMessage(sMessage);
   if (sMessage != "" && GetStringValue("Action") == "LessonComplete" && GetBoolValue("PassedRequirements"))
      MessageBox(sMessage, pScreen, EMessageBoxType_OK);
}

bool CLessonEngine::SaveLessonPerformance(CHTString& rsError)
{
   // Don't record lessons if they didn't even start
   if (GetStringValue("TypedText") == "")
      return true;

   xmlKeepBlanksDefault(0);
   xmlDocPtr pDoc = xmlParseFile(GetLessonHistoryPath().GetString());
   if (pDoc == NULL)
      pDoc = xmlNewDoc(BAD_CAST "1.0");

   xmlNodePtr pRootElem = xmlDocGetRootElement(pDoc);
   if (!pRootElem || !IsSameNoCase((char*)pRootElem->name, "stats"))
   {
      pRootElem = xmlNewNode(NULL, BAD_CAST "stats");
      xmlDocSetRootElement(pDoc, pRootElem);
   }

   xmlNodePtr pNodeRoot = xmlNewNode(NULL, BAD_CAST "lesson");
   xmlAddChild(pRootElem, pNodeRoot);

// Name
   xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "name", BAD_CAST GetStringValue("name").GetString());

// Type
   xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "type", BAD_CAST GetStringValue("type").GetString());

// Time
   {
      time_t vCurrentTime;
      time(&vCurrentTime);
      tm *pLocalTime = localtime(&vCurrentTime);
      xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "year", BAD_CAST IntToHTString(1900+pLocalTime->tm_year).GetString());
      xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "month", BAD_CAST IntToHTString(pLocalTime->tm_mon).GetString());
      xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "day", BAD_CAST IntToHTString(pLocalTime->tm_mday).GetString());
      xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "hour", BAD_CAST IntToHTString(pLocalTime->tm_hour).GetString());
      xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "minute", BAD_CAST IntToHTString(pLocalTime->tm_min).GetString());
   }

   xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "accuracy", BAD_CAST DoubleToString(GetDoubleValue("Accuracy")).GetString());

   xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "completion", BAD_CAST DoubleToString(GetDoubleValue("CompletionPercentage")).GetString());

   xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "AdjustedWPM", BAD_CAST DoubleToString(GetDoubleValue("AdjustedWPM")).GetString());

   xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "LessonChars", BAD_CAST IntToHTString(GetStringValue("LessonText").Length()).GetString());

   xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "SecondsElapsed", BAD_CAST IntToHTString((SDL_GetTicks() - m_iLessonStart) / 1000).GetString());

   if (GetStringValue("ProblemKeys", false) != "")
      xmlNewTextChild(pNodeRoot, NULL, BAD_CAST "ProblemKeys", BAD_CAST GetStringValue("ProblemKeys").GetString());


   xmlSaveFormatFile(GetLessonHistoryPath(), pDoc, 1);
   xmlFreeDoc(pDoc);
   xmlCleanupParser();
   return true;
}

bool CLessonEngine::LoadAndStoreKeyDescs(CHTString sKeyDescsFile, CHTString sLessonKeys, CHTString& rsError)
{
   MungerDataFilePath(sKeyDescsFile);
   CTextFile vFile;
   if (!vFile.Open(sKeyDescsFile, false, false, rsError))
   {
      rsError = "Unable to open key descriptions file: "+rsError;
      return false;
   }

   CHTStringArray asLines;
   if (!vFile.SlurpFile(asLines, rsError) || !vFile.Close(rsError))
   {
      rsError = "Unable to read key descriptions file: "+rsError;
      return false;
   }
   for (int iLine = 0; iLine < asLines.size(); iLine++)
   {
      CHTString sLine = asLines[iLine];
      if (sLine == "") continue;
      if (sLine.Length() < 2 || sLine[1] != '=')
      {
         rsError = "The key descriptions file is in an invalid format.  The correct format is 'a=a key description'.";
         return false;
      }

      char aKey = sLine[0];
      sLine.RemoveAt(0, 2);
      CHTString sKey = TranslateUpperCharIfNecessary(aKey);
      SetStringValue("KeyDesc"+sKey, sLine);
   }

   for (int iKey = 0; iKey < sLessonKeys.Length(); iKey++)
   {
      char aKey = sLessonKeys[iKey];

      if (!HasStringValue(CHTString("KeyDesc")+CHTString(aKey)))
      {
         rsError = "Unable to find matching key description for key " + CHTString(aKey) + ".";
         return false;
      }
   }

   return true;
}

void CLessonEngine::SplitLessonTextIntoLines(CHTString sLessonText, CHTString sTypedText, int iLineLength, CHTStringArray& rasLessonLines, CHTStringArray& rasTypedLines)
{
   while (sLessonText != "")
   {
      int iLastFoundChar = 0;
      int iThisFoundChar = 0;
      while (true)
      {
         if (!sLessonText.Find(" ", iLastFoundChar, iThisFoundChar))
         {
            // If we couldn't find any spaces in the text, just dump the whole line
            if (iLastFoundChar == 0 || (sLessonText.Length() <= iLineLength))
               iLastFoundChar = sLessonText.Length();
            break;
         }

         if (iThisFoundChar > iLineLength)
         {
            // Special case for bad input files: if the input file has a line with no spaces for a section longer than line length, simply break in the middle of the section
            if (iLastFoundChar == 0)
               iLastFoundChar = iLineLength;
            break;
         }

         iLastFoundChar = iThisFoundChar+1;
      }

      CHTString sLessonLine = sLessonText.Left(iLastFoundChar);
      sLessonText.RemoveAt(0, iLastFoundChar);
      rasLessonLines.Add(sLessonLine);

      int iTypedLineLength = min(iLastFoundChar, sTypedText.Length());
      CHTString sTypedLine = sTypedText.Left(iTypedLineLength);
      sTypedText.RemoveAt(0, iTypedLineLength);
      rasTypedLines.Add(sTypedLine);
   }
}

bool CLessonEngine::LastKeyCorrect()
{
   CHTString sTypedText = GetStringValue("TypedText");
   CHTString sLessonText = GetStringValue("LessonText");
   int iTypedLength = sTypedText.Length();

   return sTypedText.Length() == 0 || sLessonText[iTypedLength-1] == sTypedText[iTypedLength-1];
}

void CLessonEngine::HandleKeystroke(SDL_keysym vKey)
{
   if (m_iLessonStart == 0)
      m_iLessonStart = SDL_GetTicks()-100; // 100 is an arbitrarily chosen delay to keep WPM to skyrocketing to 1000 on first keystroke

   if (vKey.sym == SDLK_BACKSPACE)
   {
      CHTString sTypedText = GetStringValue("TypedText");
      if (sTypedText != "")
      {
         sTypedText = sTypedText.Left(sTypedText.Length()-1);
         SetStringValue("TypedText", sTypedText);
      }
      return;
   }

   if (vKey.sym == SDLK_CAPSLOCK || vKey.sym == SDLK_LALT || vKey.sym == SDLK_RALT || vKey.sym == SDLK_LSHIFT ||
       vKey.sym == SDLK_RSHIFT || vKey.mod & KMOD_LALT || vKey.mod & KMOD_LALT)
      return;

   if (!isprint((char)vKey.sym))
      return;

   char aKey = TranslateKeysymToChar(vKey);

   EMistakeHandling eHandling;
   if (!TextToMistakeHandling(GetStringValue("MistakeHandling"), eHandling))
   {
      assert(false);
      return;
   }

   int iTypedLength = GetStringValue("TypedText").Length();
   char aCorrectKey = GetStringValue("LessonText")[iTypedLength];
   if (aKey == GetStringValue("LessonText")[iTypedLength] && (LastKeyCorrect() || eHandling != EMistakeHandling_MustCorrect))
      RegisterCorrectKey(aCorrectKey);
   else
      RegisterIncorrectKey(aCorrectKey);

   if (LastKeyCorrect())
   {
      // Keystroke before this was correct.  Tack this one on the end; if it's not correct, we'll handle it next time.
      SetStringValue("TypedText", GetStringValue("TypedText")+CHTString(aKey));
   }
   else
   {
      // Last key was incorrect.  Decide whether to let user continue.
      switch (eHandling)
      {
      case EMistakeHandling_AutoCorrect:
      {
         // substitute the character in place
         CHTString sTypedText = GetStringValue("TypedText");
         sTypedText[sTypedText.Length()-1] = aKey;
         SetStringValue("TypedText", sTypedText);
         break;
      }
      case EMistakeHandling_MustCorrect:
         // don't do anything.  The user must first correct the existing mistake
         break;
      case EMistakeHandling_AllowMistakes:
         SetStringValue("TypedText", GetStringValue("TypedText") + CHTString(aKey));
         break;
      }
   }

   if (GetStringValue("TypedText").Length() == GetStringValue("LessonText").Length() && (LastKeyCorrect() || eHandling == EMistakeHandling_AllowMistakes))
      SetStringValue("Action", "LessonComplete");
}

void CLessonEngine::RegisterCorrectKey(CHTString sCorrectKey)
{
   SetIntValue("CorrectKeys", GetIntValue("CorrectKeys", false)+1);
}

void CLessonEngine::RegisterIncorrectKey(CHTString sCorrectKey)
{
   SetIntValue("IncorrectKeys", GetIntValue("IncorrectKeys", false)+1);

   // don't record mistakes if the last character wasn't correct.  This avoids re-recording
   // a mistake when the user doesn't realize it, and attempts to keep on going.
   if (!LastKeyCorrect())
      return;

   int iArrayPos = m_asMistakeKeys.LinearFind(sCorrectKey);
   if (iArrayPos == -1)
   {
      m_asMistakeKeys.Add(sCorrectKey);
      m_aiMistakesMade.Add(1);
   }
   else
   {
      m_aiMistakesMade[iArrayPos] = m_aiMistakesMade[iArrayPos] + 1;
   }
}

void CLessonEngine::GenerateProblemKeys()
{
   int iMinMistakesToQualify = min(10, max(3, GetIntValue("IncorrectKeys", false) / 8));

   CHTString sProblemKeys;
   for (int iMistakeKey = 0; iMistakeKey < m_aiMistakesMade.size(); iMistakeKey++)
   {
      if (m_aiMistakesMade[iMistakeKey] >= iMinMistakesToQualify)
         sProblemKeys += m_asMistakeKeys[iMistakeKey];
   }

   if (sProblemKeys != "" && sProblemKeys.Length() >= 3)
      SetStringValue("ProblemKeys", sProblemKeys);
}

void CLessonEngine::MungerLessonMessage(CHTString& rsMessage)
{
   rsMessage.Replace("%accuracy%", DoubleToString(GetDoubleValue("Accuracy")));
   rsMessage.Replace("%speed%", DoubleToString(GetDoubleValue("AdjustedWPM")));
}

void CLessonEngine::UpdateLessonStats()
{
   // Lesson text
   CHTString sTypedText = GetStringValue("TypedText");
   if (HasIntValue("LineLength"))
   {
      CHTStringArray asLessonLines;
      CHTStringArray asTypedLines;
      SplitLessonTextIntoLines(GetStringValue("LessonText"), sTypedText, GetIntValue("LineLength"), asLessonLines, asTypedLines);
      int iLastTypedLine = 0;
      for (int iLine = 0; iLine < asLessonLines.size(); iLine++)
      {
         SetStringValue(FormatString("LessonTextLine%i", iLine), asLessonLines[iLine]);
         SetStringValue(FormatString("TypedTextLine%i", iLine), asTypedLines[iLine]);
         if (asTypedLines[iLine] != "")
            iLastTypedLine = iLine;
      }
      SetIntValue("LastTypedLine", iLastTypedLine);
   }

// LastKeyCorrect
   SetBoolValue("LastKeyCorrect", LastKeyCorrect());

// accuracy
   int iCorrectKeystrokes = GetIntValue("CorrectKeys", false);
   int iIncorrectKeystrokes = GetIntValue("IncorrectKeys", false);
   double dAccuracy = 100.0;
   EMistakeHandling eHandling;
   if (!TextToMistakeHandling(GetStringValue("MistakeHandling"), eHandling))
   {
      assert(false);
      return;
   }
   if (eHandling == EMistakeHandling_AllowMistakes)
   {
      if (sTypedText.Length() > 0)
      {
         dAccuracy = -double(double(CalcNumberDifferentChars(sTypedText, GetStringValue("LessonText"), sTypedText.Length()) / 2.0) + double(iIncorrectKeystrokes)/2.0);
         dAccuracy /= sTypedText.Length();
         dAccuracy *= 100.0;
         dAccuracy += 100.0;
      }
   }
   else if (iCorrectKeystrokes + iIncorrectKeystrokes > 0)
   {
      dAccuracy = double(((double)iCorrectKeystrokes / double(iCorrectKeystrokes + iIncorrectKeystrokes)) * 100.0);
   }

   SetDoubleValue("Accuracy", dAccuracy);

// completion percentage
   double dCompletion = 0.0;
   if (GetStringValue("TypedText") != "")
      dCompletion = (double)GetStringValue("TypedText").Length() / (double)GetStringValue("LessonText").Length() * 100.0;
   SetDoubleValue("CompletionPercentage", dCompletion);

// speed
   double dWPM = 0;
   if (m_iLessonStart != 0)
   {
      double dMinutesElapsed = double(SDL_GetTicks() - m_iLessonStart) / double(1000 * 60);
      CHTString sTypedText = GetStringValue("TypedText");
      int iGoodChars = sTypedText.Length() - CalcNumberDifferentChars(sTypedText, GetStringValue("LessonText"), sTypedText.Length());
      dWPM = ((double)iGoodChars / 6.0) / dMinutesElapsed;
   }
   SetDoubleValue("AdjustedWPM", dWPM);

// pass/fail performance requirements
   SetBoolValue("PassedRequirements", true);
   SetStringValue("RequirementsError", "");

   // For each requirement, if it's not met, store that
   // (Checked in reverse order of importance, so last check has overrides any other error)
   if (HasDoubleValue("MinSpeed"))
   {
      double dMinSpeed = GetDoubleValue("MinSpeed");
      if (GetDoubleValue("AdjustedWPM") < dMinSpeed)
      {
         SetBoolValue("PassedRequirements", false);
         SetStringValue("RequirementsError", GetStringValue("SpeedError"));
      }
   }

   if (HasIntValue("MinAccuracy"))
   {
      int iMinAccuracy = GetIntValue("MinAccuracy");
      if ((int)GetDoubleValue("Accuracy") < iMinAccuracy)
      {
         SetBoolValue("PassedRequirements", false);
         SetStringValue("RequirementsError", GetStringValue("AccuracyError"));
      }
   }
   m_iLastLessonUpdate = SDL_GetTicks();
}

CLessonIterator::CLessonIterator()
{
}

CLessonIterator::~CLessonIterator()
{
}

bool CLessonIterator::Iterator_Init(CHTString sSettingsFile, CHTString& rsError)
{
   if (!ExistFile(GetSettingsDir()) && !CreateDirectory(GetSettingsDir(), 0744))
   {
      rsError = "Unable to create settings folder.";
      return false;
   }

   // The screen creator can specify "Last" as the settings file for a lesson set file to
   // tell us to open the last lesson set.  If they did, we just change ourselves to point
   // to that last lesson set file, and go ahead with the load.  If the setting doesn't
   // exist, we just kick out, and we'll take handle it in Iterator_GetFirstScreen.
   if (sSettingsFile == "Last")
   {
      CHTString sLastLessonSet;
      if (!GetLastLessonSetIfExists(sLastLessonSet))
         return true;

      sSettingsFile = sLastLessonSet;
   }
   m_sAbsoluteSettingsFile = m_sRelativeSettingsFile = sSettingsFile;
   MungerDataFilePath(m_sAbsoluteSettingsFile);

   xmlDoc* pDoc = xmlReadFile(m_sAbsoluteSettingsFile.GetString(), NULL, 0);
   if (pDoc == NULL)
   {
      rsError = "Unable to read XML file " + sSettingsFile + ".";
      return false;
   }
   xmlNode* pRootElem = xmlDocGetRootElement(pDoc);
   if (!IsSameNoCase((char*)pRootElem->name, "lessons"))
   {
      xmlFreeDoc(pDoc);
      rsError = "The <lessons> element must be root element.";
      return false;
   }

   xmlNode* pLessonElem = pRootElem->children;
   while (true)
   {
      AdvanceToElementNode(pLessonElem);
      if (!pLessonElem)
         break;

      if (IsSameNoCase((char*)pLessonElem->name, "globals"))
      {
         if (m_apLessons.size() > 0)
         {
            xmlFreeDoc(pDoc);
            rsError = "The global settings must be specified before any lessons.";
            return false;
         }

         m_vGlobalSettings.LoadFromXml(pLessonElem->children);
      }
      else if (IsSameNoCase((char*)pLessonElem->name, "ProblemKeysLesson"))
      {
         m_vProblemKeysLesson.LoadFromXml(pLessonElem->children);
         m_vProblemKeysLesson.CopyFromOther(m_vGlobalSettings, false);
      }
      else if (IsSameNoCase((char*)pLessonElem->name, "lesson"))
      {
         CKeyedValues* pSettings = new CKeyedValues;
         pSettings->LoadFromXml(pLessonElem->children);
         pSettings->CopyFromOther(m_vGlobalSettings, false);

         int iTmp;
         CHTString sLessonName;
         if (!pSettings->GetStringValue("name", sLessonName, rsError) || sLessonName == "" || FindLessonPos(sLessonName, iTmp))
         {
            rsError = "The name for lesson '"+sLessonName+"' is missing or has the same name as another lesson.";
            return false;
         }

         m_apLessons.Add(pSettings);
      }
      else
      {
         xmlFreeDoc(pDoc);
         rsError = "Encountered unexpected child element.";
         return false;
      }

      pLessonElem = pLessonElem->next;
   }

   xmlFreeDoc(pDoc);
   SaveLessonSetAsLast(); // Record for posterity that we're the last lesson set
   return true;
}

CScreenEngine* CLessonIterator::Iterator_GetFirstScreen(OUT CHTString& rsDisplayFile, CHTString& rsError)
{
   CHTString sLessonFromPrevIteration;
   if (GetLastLessonIfExists(sLessonFromPrevIteration) && sLessonFromPrevIteration != "")
   {
      SaveLessonAsLast("");
      CLessonEngine* pEngine = LoadLessonByName(sLessonFromPrevIteration, rsDisplayFile, rsError);
      // If the previous lesson didn't match any of our lessons, just return the first lesson
      if (pEngine)
         return pEngine;
   }

   return LoadLessonByPos(0, rsDisplayFile, rsError);
}

CScreenEngine* CLessonIterator::Iterator_GetNextScreen(CScreenEngine* pPrevScreen, OUT CHTString& rsDisplayFile, CHTString& rsError)
{
/* Logic of this function:
   1. Look at action to determine if it is an "absolute" action (Next, Prev, Quit).  If it is, follow it.
   2. If it is a "LessonComplete" action:
      a. If we just finished a problem keys lesson, return to where we were
      b. If we failed the lesson, display problem keys if desired, otherwise go to failure lesson
      c. If it is before the start or after the end of the lessons, quit
*/
   rsError = "";

   CHTString sPrevLessonName = pPrevScreen->GetStringValue("name");
   // Get our relative position in lesson progression
   int iLastPos;
   if (!FindLessonPos(sPrevLessonName, iLastPos))
   {
      assert(m_sLessonPriorToProblemKeys != ""); // should only happen after problem keys lesson
      iLastPos = 0;
   }

// Handle absolute actions
   CHTString sAction = pPrevScreen->GetStringValue("action");
   if (sAction == "QuitLessons")
      return NULL;
   if (sAction == "PrevLesson")
      return LoadLessonByPos(iLastPos-1, rsDisplayFile, rsError);
   if (sAction == "RestartLesson")
      return LoadLessonByPos(iLastPos, rsDisplayFile, rsError);
   if (sAction == "NextLesson")
   {
      int iNextLessonPos = MaySkipPastLesson(iLastPos) ? iLastPos+1 : iLastPos;
      return LoadNextLesson(iNextLessonPos, rsDisplayFile, rsError);
   }

// Handle "LessonComplete" action
   if (sAction != "LessonComplete")
   {
      rsError = "Unrecognized action: " + sAction;
      return NULL;
   }

// Have we just finished a problem keys lesson?
   if (m_sLessonPriorToProblemKeys != "")
   {
      CHTString sTmpLesson = m_sLessonPriorToProblemKeys;
      m_sLessonPriorToProblemKeys = "";
      return LoadLessonByName(sTmpLesson, rsDisplayFile, rsError);
   }

// If we didn't pass performance requirements, display problem keys or failure lesson
   if (!pPrevScreen->GetBoolValue("PassedRequirements"))
   {
      if (ShouldShowProblemKeysForNextLesson(pPrevScreen))
      {
         m_sLessonPriorToProblemKeys = sPrevLessonName;
         return LoadProblemKeysLesson(pPrevScreen->GetStringValue("ProblemKeys"), rsDisplayFile, rsError);
      }
      else
         return LoadLessonByName(pPrevScreen->GetStringValue("FailureLesson"), rsDisplayFile, rsError);
   }

   SaveLessonAsFurthestCompleted(sPrevLessonName);

   return LoadNextLesson(iLastPos+1, rsDisplayFile, rsError);
}

bool CLessonIterator::Iterator_OnScreenEnd(CScreenEngine* pScreen)
{
   SaveLessonAsLast(pScreen->GetStringValue("name"));

   if (pScreen->GetStringValue("action") == "LessonComplete" && !pScreen->GetBoolValue("PassedRequirements") && !ShouldShowProblemKeysForNextLesson(pScreen))
      MessageBox(pScreen->GetStringValue("RequirementsError"), m_pRootSurface, EMessageBoxType_OK);

   return true;
}

bool CLessonIterator::GetLastLessonSetIfExists(CHTString& rsLessonSetName)
{
   return GetSettingFromSettingsFile("LastLessonSet", rsLessonSetName);
}

bool CLessonIterator::GetLastLessonIfExists(CHTString& rsLessonName)
{
   if (GetSettingFromSettingsFile(GetLessonSetName()+"_LastLesson", rsLessonName))
      return true;

   rsLessonName = "";
   return false;
}

void CLessonIterator::SaveLessonSetAsLast()
{
   CHTString sError;
   if (!SaveSettingToSettingsFile("LastLessonSet", m_sRelativeSettingsFile, sError))
      cout << "Warning: " << sError << "\n"; // It's not good if we can't save, but crashing at this point is overkill
}

void CLessonIterator::SaveLessonAsLast(CHTString sLesson)
{
   CHTString sError;
   if (!SaveSettingToSettingsFile(GetLessonSetName()+"_LastLesson", sLesson, sError))
      cout << "Warning: " << sError << "\n"; // It's not good if we can't save, but crashing at this point is overkill
}

bool CLessonIterator::GetFurthestCompletedLessonIfExists(CHTString& rsLessonName)
{
   return GetSettingFromSettingsFile(GetLessonSetName()+"_Furthest", rsLessonName);
}

void CLessonIterator::SaveLessonAsFurthestCompleted(CHTString sLesson)
{
   int iOurLessonPos = 0;
   if (!FindLessonPos(sLesson, iOurLessonPos)) iOurLessonPos = 0;

   CHTString sPrevFurthestLesson;
   int iPrevFurthestLessonPos = 0;
   if (GetFurthestCompletedLessonIfExists(sPrevFurthestLesson) && FindLessonPos(sPrevFurthestLesson, iPrevFurthestLessonPos))
   {
      if (iOurLessonPos <= iPrevFurthestLessonPos)
         return; // don't save if we already have a furthest lesson that further than we are
   }

   CHTString sError;
   if (!SaveSettingToSettingsFile(GetLessonSetName()+"_Furthest", sLesson, sError))
      cout << "Warning: " << sError << "\n"; // It's not good if we can't save, but crashing at this point is overkill
}

bool CLessonIterator::ShouldShowProblemKeysForNextLesson(CScreenEngine* pPrevScreen)
{
   bool bProblemKeysLessonAllowed = pPrevScreen->HasBoolValue("ProblemKeysLesson") && pPrevScreen->GetBoolValue("ProblemKeysLesson");
   return bProblemKeysLessonAllowed && pPrevScreen->GetStringValue("ProblemKeys", false) != "";
}

bool CLessonIterator::MaySkipPastLesson(int iLessonPos)
{
   if (g_bDebugMode)
      return true;

   CHTString sFurthestLesson;
   int iFurthestLessonPos = 0;
   if (GetFurthestCompletedLessonIfExists(sFurthestLesson) && FindLessonPos(sFurthestLesson, iFurthestLessonPos))
      return (iLessonPos <= iFurthestLessonPos);

   return false;
}

bool CLessonIterator::FindLessonPos(CHTString sLessonName, OUT int& riLessonPos)
{
   for (int iLesson = 0; iLesson < m_apLessons.size(); iLesson++)
   {
      CHTString sIgnoredError;
      CHTString sCurLessonName;
      assert(m_apLessons[iLesson]->GetStringValue("name", sCurLessonName, sIgnoredError));
      if (sCurLessonName == sLessonName)
      {
         riLessonPos = iLesson;
         return true;
      }
   }

   return false;
}

CLessonEngine* CLessonIterator::LoadNextLesson(int iNextLessonPos, OUT CHTString& rsDisplayFile, OUT CHTString& rsError)
{
   CLessonEngine* pEngine = LoadLessonByPos(iNextLessonPos, rsDisplayFile, rsError);
   if (!pEngine && rsError == "")
      SaveLessonAsLast("");
   return pEngine;
}


CLessonEngine* CLessonIterator::LoadLessonByPos(int iPos, OUT CHTString& rsDisplayFile, OUT CHTString& rsError)
{
   rsError = "";


   CKeyedValues* pSettings = NULL;
   while (true)
   {
      if (iPos < 0 || iPos >= m_apLessons.size())
         return NULL;

      pSettings = m_apLessons[iPos];
      // This is essentially an ugly hack to support games in the middle of a lesson list
      CHTString sIgnored;
      CHTString sGameSettingsFile;
      if (!pSettings->GetStringValue("GameSettingsFile", sGameSettingsFile, sIgnored))
         break;

      if (!DisplayCometGame(m_pRootSurface, sGameSettingsFile, rsError))
         return NULL;

      iPos++;
   }
   CLessonEngine* pEngine = Internal_LoadLesson(pSettings, "", rsDisplayFile, rsError);
   if (pEngine)
   {
      pEngine->SetIntValue("CurrentLesson", iPos+1);
      pEngine->SetIntValue("NumberLessons", m_apLessons.size());
   }
   return pEngine;
}

CLessonEngine* CLessonIterator::LoadLessonByName(CHTString sName, OUT CHTString& rsDisplayFile, OUT CHTString& rsError)
{
   int iLessonPos = 0;
   if (!FindLessonPos(sName, iLessonPos))
      return NULL;

   return LoadLessonByPos(iLessonPos, rsDisplayFile, rsError);
}

CLessonEngine* CLessonIterator::LoadProblemKeysLesson(CHTString sProblemKeys, OUT CHTString& rsDisplayFile, OUT CHTString& rsError)
{
   CLessonEngine* pEngine = Internal_LoadLesson(&m_vProblemKeysLesson, sProblemKeys, rsDisplayFile, rsError);
   return pEngine;
}

CLessonEngine* CLessonIterator::Internal_LoadLesson(CKeyedValues* pSettings, IN CHTString sProblemKeys, OUT CHTString& rsDisplayFile, OUT CHTString& rsError)
{
   CLessonEngine* pEngine = new CLessonEngine;
   if (sProblemKeys != "")
      pEngine->SetStringValue("ProblemKeys", sProblemKeys);

   if (!pEngine->Load(pSettings, rsError))
   {
      rsError = "Unable to load lesson " + pEngine->GetStringValue("name", false) + ": " + rsError;
      delete pEngine;
      return NULL;
   }

   CHTString sIgnoredError;
   assert(pSettings->GetStringValue("DisplayFile", rsDisplayFile, sIgnoredError));
   return pEngine;
}

bool TextToMistakeHandling(CHTString sText, EMistakeHandling& reHandling)
{
   if (IsSameNoCase(sText, "auto correct"))
   {
      reHandling = EMistakeHandling_AutoCorrect;
      return true;
   }
   else if (IsSameNoCase(sText, "must correct"))
   {
      reHandling = EMistakeHandling_MustCorrect;
      return true;
   }
   else if (IsSameNoCase(sText, "allow mistakes"))
   {
      reHandling = EMistakeHandling_AllowMistakes;
      return true;
   }

   return false;
}

CBookTextIterator::CBookTextIterator()
{
   m_iLessonLength = 0;
   m_dMinSpeedRequirement = 0;
   m_dSpeedIncreaseAmount = 0;
}

CBookTextIterator::~CBookTextIterator()
{
}

bool CBookTextIterator::Iterator_Init(CHTString sSettingsFile, CHTString& rsError)
{
   m_sSettingsFile = sSettingsFile;
   MungerDataFilePath(m_sSettingsFile);

   xmlDoc* pDoc = xmlReadFile(m_sSettingsFile.GetString(), NULL, 0);
   if (pDoc == NULL)
   {
      rsError = "Unable to read XML file " + sSettingsFile + ".";
      return false;
   }
   xmlNode* pRootElem = xmlDocGetRootElement(pDoc);
   if (!IsSameNoCase((char*)pRootElem->name, "settings"))
   {
      xmlFreeDoc(pDoc);
      rsError = "The <settings> element must be root element.";
      return false;
   }

   CHTStringArray asFileLines;
   xmlNode* pLessonElem = pRootElem->children;
   while (true)
   {
      AdvanceToElementNode(pLessonElem);
      if (!pLessonElem)
         break;

      if (IsSameNoCase((char*)pLessonElem->name, "BookTextPath"))
      {
         CHTString sBookPath = (char*)pLessonElem->children->content;
         MungerDataFilePath(sBookPath);
         CTextFile vFile;
         if (!vFile.Open(sBookPath, false, false, rsError))
         {
            rsError = "Unable to open book text file: " + rsError;
            return false;
         }
         if (!vFile.SlurpFile(asFileLines, rsError))
         {
            rsError = "Unable to read book text file: " + rsError;
            return false;
         }
         assert(vFile.Close(rsError));
      }
      else if (IsSameNoCase((char*)pLessonElem->name, "LessonLength"))
      {
         if (!StringToInt((char*)pLessonElem->children->content, m_iLessonLength))
         {
            rsError = "The 'LessonLength' setting must be a number.";
            return false;
         }
      }
      else if (IsSameNoCase((char*)pLessonElem->name, "BookCompletedMessage"))
      {
         m_sBookCompletedMessage = (char*)pLessonElem->children->content;
      }
      else if (IsSameNoCase((char*)pLessonElem->name, "DisplayFile"))
      {
         m_sDisplayFile = (char*)pLessonElem->children->content;
      }
      else if (IsSameNoCase((char*)pLessonElem->name, "lesson"))
      {
         m_LessonSettings.LoadFromXml(pLessonElem->children);
      }
      else if (IsSameNoCase((char*)pLessonElem->name, "SpeedIncreaseAmount"))
      {
         if (!StringToDouble((char*)pLessonElem->children->content, m_dSpeedIncreaseAmount))
         {
            rsError = "The 'SpeedIncreaseAmount' setting must be a number.";
            return false;
         }
      }
      else if (IsSameNoCase((char*)pLessonElem->name, "MinSpeedRequirement"))
      {
         if (!StringToDouble((char*)pLessonElem->children->content, m_dMinSpeedRequirement))
         {
            rsError = "The 'MinSpeedRequirement' setting must be a number.";
            return false;
         }
      }

      pLessonElem = pLessonElem->next;
   }

   // Compile lesson texts from book
   while (asFileLines.size() > 0)
   {
      CHTString sLessonText;
      while (sLessonText.Length() < m_iLessonLength && asFileLines.size() > 0)
      {
         sLessonText += asFileLines[0] + " ";
         asFileLines.RemoveAt(0);
      }
      sLessonText.TrimRight(' ');
      if (sLessonText != "")
         m_asLessonTexts.Add(sLessonText);
   }

   return true;
}

CScreenEngine* CBookTextIterator::Iterator_GetFirstScreen(OUT CHTString& rsDisplayFile, CHTString& rsError)
{
   return Iterator_GetNextScreen(NULL, rsDisplayFile, rsError);
}

CScreenEngine* CBookTextIterator::Iterator_GetNextScreen(CScreenEngine* pPrevScreen, OUT CHTString& rsDisplayFile, CHTString& rsError)
{
   rsError = "";

   // Handle any actions that we need to
   if (pPrevScreen)
   {
      CHTString sAction = pPrevScreen->GetStringValue("Action");
      if (sAction == "QuitLessons")
         return NULL;
   }

   // See if the whole book has been completed.  If so, show message and quit.
   // Compile lesson text from book
   int iStartingLesson;
   if (!GetCompletedLessons(iStartingLesson))
      iStartingLesson = 0;

   if (iStartingLesson >= m_asLessonTexts.size())
   {
      MessageBox(m_sBookCompletedMessage, m_pRootSurface, EMessageBoxType_OK);
      assert(SetCompletedLessons(0, rsError));
      return NULL;
   }

   // Load new screen
   CLessonEngine* pNewScreen = new CLessonEngine;
   m_LessonSettings.SetStringValue("LessonText", m_asLessonTexts[iStartingLesson]);
   double dMinSpeed = GetMinSpeed(m_dMinSpeedRequirement);
   m_LessonSettings.SetDoubleValue("MinSpeed", dMinSpeed);

   if (!pNewScreen->Load(&m_LessonSettings, rsError))
   {
      delete pNewScreen;
      return NULL;
   }
   pNewScreen->SetIntValue("LessonNumber", iStartingLesson);

   // Set vars for lesson progress indicator
   int iTotalLessons = m_asLessonTexts.size();
   int iShownLessons = min(iTotalLessons, 20);
   double dRatio = double(iTotalLessons) / double(iShownLessons);
   pNewScreen->SetIntValue("NumberLessons", iShownLessons);
   pNewScreen->SetIntValue("CurrentLesson", int(iStartingLesson / dRatio) + 1);

   rsDisplayFile = m_sDisplayFile;
   return pNewScreen;
}

bool CBookTextIterator::Iterator_OnScreenEnd(CScreenEngine* pScreen)
{
   CHTString sAction = pScreen->GetStringValue("Action");
   if (sAction == "LessonComplete")
   {
      if (!pScreen->GetBoolValue("PassedRequirements"))
         MessageBox(pScreen->GetStringValue("RequirementsError"), m_pRootSurface, EMessageBoxType_OK);
      else
      {
         int iCompletedLesson = pScreen->GetIntValue("LessonNumber", false);
         iCompletedLesson++;

         CHTString sError;
         if (!SetCompletedLessons(iCompletedLesson, sError) ||
             !SetMinSpeed(pScreen->GetDoubleValue("MinSpeed")+m_dSpeedIncreaseAmount, sError))
         {
            cout << sError;
            return false;
         }
      }
   }

   return true;
}

bool CBookTextIterator::GetCompletedLessons(OUT int& riNumLessons)
{
   CHTString sLineSetting;
   return GetSettingFromSettingsFile(ExtractFileNameFromPath(m_sSettingsFile)+"_CompletedLessons", sLineSetting) &&
           StringToInt(sLineSetting, riNumLessons);
}

bool CBookTextIterator::SetCompletedLessons(int iNumLessons, CHTString& rsError)
{
   return SaveSettingToSettingsFile(ExtractFileNameFromPath(m_sSettingsFile)+"_CompletedLessons", IntToHTString(iNumLessons), rsError);
}

double CBookTextIterator::GetMinSpeed(double dDefaultMinSpeed)
{
   CHTString sSpeedSetting;
   double dMinSpeed;
   if (GetSettingFromSettingsFile("MinBookSpeed", sSpeedSetting) && StringToDouble(sSpeedSetting, dMinSpeed))
      return dMinSpeed;

   return dDefaultMinSpeed;
}

bool CBookTextIterator::SetMinSpeed(double dMinSpeed, CHTString& rsError)
{
   return SaveSettingToSettingsFile("MinBookSpeed", DoubleToString(dMinSpeed), rsError);
}


////////////////////////////// Interfaced functions //////////////////////////////////////////


CScreenIterator* GetLessonIterator()
{
   return new CLessonIterator;
}

CScreenIterator* GetBookIterator()
{
   return new CBookTextIterator;
}


