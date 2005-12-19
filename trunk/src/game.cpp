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
#include "game.h"

#include "file_aid.h"
#include "font_aid.h"
#include "image_aid.h"
#include "message_box.h"
#include "OS.h"
#include "screen.h"
#include "SDL_aid.h"
#include "widget.h"
#include "widget_image.h"
#include "widget_settings.h"
#include "xml_aid.h"


/*

Responsibilities of various classes:

StateTracker:
   * has IsIterating()
   * callers call Initialize() and AdvanceState().  After first AdvanceState() call, IsIterating() becomes true.
        After sprite is exploded and animation is complete, IsIterating() becomes false.

SpriteWidget:
   * manages statetracker, plus text drawing
   * receives text from engine (SpriteCorrectText/SpriteTypedText)
   * when it receives text from the engine, it starts the StateTracker.
   * explodes sprite when text becomes empty.

GameEngine:
   * sees when sprite text is typed correctly, and removes sprite text, making sprite explode
   * maintains wordlist, delegating words to individual widgets

*/


#define CSurfaceArray CHTArray<SDL_Surface*>

int GetPixelsPerIteration(int iLevel)
{
   return 2 + (iLevel * 2);
}

// This class is responsible for maintaining the state of the sprite and its target.
// This includes cycling through the sprite and target images, as well as tracking their destruction.
class CCollidingSpriteStateTracker
{
public:
   CCollidingSpriteStateTracker();
   ~CCollidingSpriteStateTracker();

   void Initialize(SDL_Rect vWidgetRect, CSurfaceArray apSpriteImages, CSurfaceArray m_apSpriteDestructionImages,
                   SDL_Surface* pUnreachedImage, CSurfaceArray m_apTargetDestructionImages);

   void AdvanceState(); // Advances the sprite animation one frame
   bool IsIterating() { return m_pCurSpriteImage || (m_bEndReached && m_iCurrentTargetDestructionImage < m_apTargetDestructionImages.size()-1); }
   bool TargetDestroyed();

   void RegisterExternalHit();

   bool GetSpriteImageToDisplay(SDL_Surface*& rpImage, int& riX, int& riY); // if returns false, sprite should not be displayed
   void GetTargetImageToDisplay(SDL_Surface*& rpImage, int& riX, int& riY); // will never give back NULL

   void SetMovementSpeed(int iPixelsPerIteration) { m_iPixelsPerIteration = iPixelsPerIteration; }

protected:
   void Restart();

   CSurfaceArray* GetSpriteImageArray() { return m_bSpriteBeingDestroyed ? &m_apSpriteDestructionImages : &m_apSpriteImages; }
   SDL_Surface* GetTargetImage() { return m_bEndReached ? m_apTargetDestructionImages[m_iCurrentTargetDestructionImage] : m_pUnreachedTargetImage; }

   SDL_Rect m_WidgetRect;
   int m_iPixelsPerIteration;

// Images
   CSurfaceArray m_apSpriteImages;
   CSurfaceArray m_apSpriteDestructionImages;
   SDL_Surface* m_pUnreachedTargetImage;
   CSurfaceArray m_apTargetDestructionImages;

// State variables
   int m_iCurrentMovePos; // Offset from top of widget to top of sprite (may be negative before the sprite has fully appeared.)
   bool m_bEndReached; // True if the sprite has hit the target
   bool m_bSpriteBeingDestroyed; // True if we should be displaying the destruction images for the sprite
   int m_iCurrentSpriteImage; // refers to an offset in m_apSpriteImages or m_apSpriteDestructionImages, depending on m_bSpriteBeingDestroyed
   int m_iCurrentTargetDestructionImage;
   SDL_Surface* m_pCurSpriteImage; // These store the current images, for retrieval by the GetCurrentNNNImage() functions
   SDL_Surface* m_pCurTargetImage;
};

class CCollidingSprite : public CWidget
{
public:
   CCollidingSprite();
   virtual ~CCollidingSprite();

   virtual void LoadSettings(CKeyedValues& rSettings);
   virtual bool ValidateSettings(CHTString& rsElementaryError);
   virtual void PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine);

   virtual bool WantsKeyboardEvents() { return true; }
   virtual void HandleKeyboardEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine);
   virtual bool WantsTimerEvents() { return true; }
   virtual void HandleTimerEvent(CPaintingParms* pPaintingParms, CScreenEngine* pEngine);

protected:
   bool LoadImageArray(CHTStringArray& rasImagePaths, OUT CHTArray<SDL_Surface*>& rapImages, CHTString& rsError);
   void RepaintPartialBackground(SDL_Rect vRectToRepaint, CPaintingParms* pPaintingParms);
   bool LoadAndSaveImageArray(CKeyedValues& rSettings, CHTString sArraySetting, CSurfaceArray& rapImages, CHTString& rsError);
   void PaintSurface(SDL_Surface* pSourceSurface, int iSurfaceX, int iSurfaceY, SDL_Rect& rLastPaintedRect, CPaintingParms* pPaintingParms); // rLastPaintedRect is updated to completed paint

   CHTString GetTextToType(CScreenEngine* pEngine);
   CHTString GetTypedText(CScreenEngine* pEngine);
   bool LastCharCorrect(CScreenEngine* pEngine);
   bool ShouldStartIteration(CScreenEngine* pEngine);
   void SetTargetHit(CScreenEngine* pEngine);
   void SetSpriteRunning(CScreenEngine* pEngine, bool bRunning);
   void SetSpriteTextVisible(CScreenEngine* pEngine, bool bVisible);
   bool HasTextToType(CScreenEngine* pEngine);

   // rRect is updated to the correct text rect
   void PaintTextLine(CHTString sCorrectText, CHTString sTypedText, SDL_Rect& rRect, TTF_Font* pTextFont, SDL_Color vUntypedTextColor,
                      SDL_Color vCorrectTextColor, SDL_Color vIncorrectTextColor, CPaintingParms* pParms, CScreenEngine* pEngine);

   int m_iSpriteNumber;
   CCollidingSpriteStateTracker m_vStateTracker;
   TTF_Font* m_pTextFont;
   
   CHTString m_sLastPaintedText;
   SDL_Rect m_vLastPaintedSpriteRect;
   SDL_Rect m_vLastPaintedTargetRect;
   SDL_Rect m_vLastPaintedTextRect;

   CHTString m_sLoadError;
};

class CGameEngine : public CScreenEngine
{
public:
   CGameEngine();
   virtual ~CGameEngine();

   virtual bool Load(CKeyedValues* pSettings, CHTString& rsError);

   virtual void OnKey(SDL_keysym vKey);
   virtual void OnMouse(SDL_Event vMouse) {}
   virtual void OnTimer();

   virtual void OnScreenStart(SDL_Surface* pScreen, bool& rbScreenNeedsRepaint);
   virtual void OnScreenEnd(SDL_Surface* pScreen);

protected:
   void UpdateWidgetInfo();
   void HandleKeystroke(char aKey);
   bool LastCharCorrect();
   bool CanUseWordForNewSprite(CHTString sWord);
   bool ShouldShowNewSprite(OUT int& riSpriteNum);
   void IncrementScore(int iAmtToIncrement);

   CHTString GetLessonTextKey(int iWidgetNum);
   CHTString GetTypedTextKey(int iWidgetNum);
   CHTString GetTargetHitKey(int iWidgetNum);
   bool IsSpriteRunning(int iWidgetNum);
   bool IsSpriteTextVisible(int iWidgetNum);
   bool SpriteTextIsTyped(int iWidgetNum);

// Lesson info
   CHTStringArray m_asWordsToType; // Is gradually emptied

// Widget info
   int m_iNumberSprites;
   int m_iSpriteCurrentlyBeingTyped; // -1 if none
   CHTBoolArray m_abSpriteRanThisIteration;
};

class CGameIterator : public CScreenIterator
{
public:
   CGameIterator();
   virtual ~CGameIterator();

protected:
   virtual bool Iterator_Init(CHTString sSettingsFile, CHTString& rsError);
   virtual CScreenEngine* Iterator_GetFirstScreen(OUT CHTString& rsDisplayFile, CHTString& rsError);
   virtual CScreenEngine* Iterator_GetNextScreen(CScreenEngine* pPrevScreen, OUT CHTString& rsDisplayFile, CHTString& rsError);
   virtual bool Iterator_OnScreenEnd(CScreenEngine* pScreen);

   CKeyedValues m_vGameSettings;
};

// END HEADER
///////////////////////////////////////

///////////////////////////////////////
// BEGIN CPP


CCollidingSprite::CCollidingSprite()
{
   m_vLastPaintedSpriteRect = SDLRectFromDims(0,0,0,0);
   m_vLastPaintedTargetRect = SDLRectFromDims(0,0,0,0);
   m_vLastPaintedTextRect = SDLRectFromDims(0,0,0,0);
}

CCollidingSprite::~CCollidingSprite()
{
}

CHTString GetBadSettingError(CHTString sSetting)
{
   return "The '"+sSetting+"' setting is missing or invalid.";
}

void CCollidingSprite::LoadSettings(CKeyedValues& rSettings)
{
// Widget Rect
   LoadRectSettings(rSettings);

   if (!rSettings.GetIntValue("SpriteNumber", m_iSpriteNumber, m_sLoadError)) return;

   CHTString sSettingVal;
   if (!rSettings.GetStringValue("TextFontPath", sSettingVal, m_sLoadError)) return;
   m_pTextFont = GetOrLoadFont(sSettingVal, 20, m_sLoadError);
   if (!m_pTextFont) return;

// Target Images
   if (!rSettings.GetStringValue("UnreachedTargetImage", sSettingVal, m_sLoadError)) return;
   SDL_Surface* pUnreachedEndImage = GetOrLoadImage(sSettingVal, m_sLoadError);
   if (!pUnreachedEndImage) return;
   CSurfaceArray apTargetDestructionImages;
   if (!LoadAndSaveImageArray(rSettings, "TargetDestructionImages", apTargetDestructionImages, m_sLoadError)) return;

// Sprite Images
   CSurfaceArray apSpriteImages;
   CSurfaceArray apSpriteDestructionImages;
   if (!LoadAndSaveImageArray(rSettings, "SpriteImages", apSpriteImages, m_sLoadError)) return;
   if (!LoadAndSaveImageArray(rSettings, "SpriteDestructionImages", apSpriteDestructionImages, m_sLoadError)) return;

   m_vStateTracker.Initialize(GetWidgetRect(), apSpriteImages, apSpriteDestructionImages, pUnreachedEndImage, apTargetDestructionImages);
}

bool CCollidingSprite::ValidateSettings(CHTString& rsElementaryError)
{
   rsElementaryError = m_sLoadError;
   return (m_sLoadError == "");
}

void CCollidingSprite::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   int iLevel = pEngine->GetIntValue("Level");
   m_vStateTracker.SetMovementSpeed(GetPixelsPerIteration(iLevel));

   CWidget::OnPaintWidget(pPaintingParms->m_pDrawingSurface);

// Actual paint
   RepaintPartialBackground(m_vLastPaintedTargetRect, pPaintingParms);
   RepaintPartialBackground(m_vLastPaintedSpriteRect, pPaintingParms);
   RepaintPartialBackground(m_vLastPaintedTextRect, pPaintingParms);

   SDL_Surface* pSurfaceToPaint;
   int iX;
   int iY;
   m_vStateTracker.GetTargetImageToDisplay(pSurfaceToPaint, iX, iY);
   PaintSurface(pSurfaceToPaint, iX, iY, m_vLastPaintedTargetRect, pPaintingParms);

   if (m_vStateTracker.IsIterating() && m_vStateTracker.GetSpriteImageToDisplay(pSurfaceToPaint, iX, iY))
   {
      PaintSurface(pSurfaceToPaint, iX, iY, m_vLastPaintedSpriteRect, pPaintingParms);

      m_vLastPaintedTextRect = m_vLastPaintedSpriteRect;
      PaintTextLine(GetTextToType(pEngine), GetTypedText(pEngine), m_vLastPaintedTextRect, m_pTextFont, SDLColorFromRGB(255,255,0), SDLColorFromRGB(0,0,0),
                    SDLColorFromRGB(255,0,0), pPaintingParms, pEngine);
      m_sLastPaintedText = GetTypedText(pEngine);

      SetSpriteTextVisible(pEngine, m_vLastPaintedTextRect.y >= -4);
   }
   else
      SetSpriteTextVisible(pEngine, false);
}

void CCollidingSprite::HandleKeyboardEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   if (m_sLastPaintedText.CompareWithCase(GetTypedText(pEngine)) == 0)
      return;
   
   if (!HasTextToType(pEngine))
      m_vStateTracker.RegisterExternalHit();

   PaintWidget(pPaintingParms, pEngine);
}

void CCollidingSprite::HandleTimerEvent(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   if (!m_vStateTracker.IsIterating() && !HasTextToType(pEngine))
      return;

   m_vStateTracker.AdvanceState();
   if (m_vStateTracker.TargetDestroyed())
      SetTargetHit(pEngine);
   SetSpriteRunning(pEngine, m_vStateTracker.IsIterating());


   PaintWidget(pPaintingParms, pEngine);
}

bool CCollidingSprite::LoadImageArray(CHTStringArray& rasImagePaths, OUT CHTArray<SDL_Surface*>& rapImages, CHTString& rsError)
{
   rapImages.clear();
   for (int iSpriteImage = 0; iSpriteImage < rasImagePaths.size(); iSpriteImage++)
   {
      SDL_Surface* pSpriteImage = GetOrLoadImage(rasImagePaths[iSpriteImage], rsError);
      if (!pSpriteImage)
      {
         EmptyPtrArray(rapImages);
         rsError = "Unable to load image number " + IntToHTString(iSpriteImage+1) + ": " + rsError;
         return false;
      }

      rapImages.Add(pSpriteImage);
   }

   return true;
}

void CCollidingSprite::RepaintPartialBackground(SDL_Rect vRectToRepaint, CPaintingParms* pPaintingParms)
{
   if (m_pBackground)
   {
      BlitClippedSurface(CHTSurface(m_pBackground, GetWidgetRect().x, GetWidgetRect().y), CHTSurface(pPaintingParms->m_pDrawingSurface, 0, 0),
                        SDLRectFromDims(vRectToRepaint.x, vRectToRepaint.y, vRectToRepaint.w, vRectToRepaint.h));
      pPaintingParms->m_aRectsToUpdate.Add(vRectToRepaint);
   }
}

bool CCollidingSprite::LoadAndSaveImageArray(CKeyedValues& rSettings, CHTString sArraySetting, CSurfaceArray& rapImages, CHTString& rsError)
{
   CHTString sSettingVal;
   if (!rSettings.GetStringValue(sArraySetting, sSettingVal, rsError))
      return false;

   CHTStringArray asImages;
   SplitDelimitedString(sSettingVal, asImages, "|");
   if (asImages.size() == 0)
   {
      m_sLoadError = GetBadSettingError(sArraySetting);
      return false;
   }
   if (!LoadImageArray(asImages, rapImages, rsError))
      return false;

   return true;
}

void CCollidingSprite::PaintSurface(SDL_Surface* pSourceSurface, int iSurfaceX, int iSurfaceY, SDL_Rect& rLastPaintedRect, CPaintingParms* pPaintingParms)
{
   if (pSourceSurface)
   {
      rLastPaintedRect = SDLRectFromDims(iSurfaceX, iSurfaceY, pSourceSurface->w, pSourceSurface->h);
      BlitClippedSurface(CHTSurface(pSourceSurface, iSurfaceX, iSurfaceY), CHTSurface(pPaintingParms->m_pDrawingSurface, 0, 0),
                         SDLRectFromDims(max(iSurfaceX, 0), max(iSurfaceY, 0), pSourceSurface->w, pSourceSurface->h));
      pPaintingParms->m_aRectsToUpdate.Add(rLastPaintedRect);
   }
}

void CCollidingSprite::PaintTextLine(CHTString sCorrectText, CHTString sTypedText, SDL_Rect& rRect, TTF_Font* pTextFont, SDL_Color vUntypedTextColor,
                                     SDL_Color vCorrectTextColor, SDL_Color vIncorrectTextColor, CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   // First draw correct text
   DrawText(sCorrectText, pPaintingParms->m_pDrawingSurface, pTextFont, vUntypedTextColor, rRect, ETextHAlign_Center, ETextVAlign_Center);
   pPaintingParms->m_aRectsToUpdate.Add(rRect);

   // Use rect from drawn correct text to determine incorrect text's position
   SDL_Rect vTypedRect = rRect;
   int iTextToPaintLength = sTypedText.Length();
   if (!LastCharCorrect(pEngine))
      iTextToPaintLength--;

   if (iTextToPaintLength > 0)
      DrawText(sCorrectText.Left(iTextToPaintLength), pPaintingParms->m_pDrawingSurface, pTextFont, vCorrectTextColor, vTypedRect, ETextHAlign_Left, ETextVAlign_Top);

   // Draw incorrect chars in different color
   if (!LastCharCorrect(pEngine))
   {
      vTypedRect.x += vTypedRect.w; // start after correctly typed text
      vTypedRect.w = 100;
      DrawText(sCorrectText[iTextToPaintLength], pPaintingParms->m_pDrawingSurface, pTextFont, vIncorrectTextColor, vTypedRect, ETextHAlign_Left, ETextVAlign_Top);
   }
}

CHTString CCollidingSprite::GetTextToType(CScreenEngine* pEngine)
{
   return pEngine->GetStringValue("LessonText" + IntToHTString(m_iSpriteNumber));
}

CHTString CCollidingSprite::GetTypedText(CScreenEngine* pEngine)
{
   return pEngine->GetStringValue("TypedText" + IntToHTString(m_iSpriteNumber));
}

bool CCollidingSprite::LastCharCorrect(CScreenEngine* pEngine)
{
   if (GetTypedText(pEngine) == "")
      return true;

   return GetTextToType(pEngine).StartsWith(GetTypedText(pEngine));
}

bool CCollidingSprite::ShouldStartIteration(CScreenEngine* pEngine)
{
   return GetTextToType(pEngine) != "";
}

void CCollidingSprite::SetTargetHit(CScreenEngine* pEngine)
{
   pEngine->SetBoolValue("TargetHit" + IntToHTString(m_iSpriteNumber), true);
}

void CCollidingSprite::SetSpriteRunning(CScreenEngine* pEngine, bool bRunning)
{
   pEngine->SetBoolValue("SpriteRunning" + IntToHTString(m_iSpriteNumber), bRunning);
}

void CCollidingSprite::SetSpriteTextVisible(CScreenEngine* pEngine, bool bVisible)
{
   pEngine->SetBoolValue("SpriteTextVisible" + IntToHTString(m_iSpriteNumber), bVisible);
}

bool CCollidingSprite::HasTextToType(CScreenEngine* pEngine)
{
   return GetTextToType(pEngine) != "" && !GetTextToType(pEngine).IsSame(GetTypedText(pEngine));
}




CGameEngine::CGameEngine()
{
   m_iNumberSprites = -1;
   m_iSpriteCurrentlyBeingTyped = -1;
}

CGameEngine::~CGameEngine()
{
}

bool CGameEngine::Load(CKeyedValues* pSettings, CHTString& rsError)
{
   if (!pSettings->GetIntValue("NumberSprites", m_iNumberSprites, rsError)) return false;
   m_abSpriteRanThisIteration.resize(m_iNumberSprites, false);

   int iMaxWords = 0;
   if (!pSettings->GetIntValue("LimitToNumberWords", iMaxWords, rsError)) return false;

   // Get master word list
   CHTString sWordListPath;
   if (!pSettings->GetStringValue("WordListPath", sWordListPath, rsError)) return false;
   MungerDataFilePath(sWordListPath);
   CTextFile vFile;
   CHTStringArray asAvailableWords;
   if (!vFile.Open(sWordListPath, false, false, rsError) || !vFile.SlurpFile(asAvailableWords, rsError) || !vFile.Close(rsError))
      return false;

   // Set up our word list
   while (m_asWordsToType.size() < iMaxWords)
      m_asWordsToType.Add(asAvailableWords[rand() % asAvailableWords.size()]);

   // Initialize sprites
   for (int iSprite = 0; iSprite < m_iNumberSprites; iSprite++)
   {
      SetStringValue(GetLessonTextKey(iSprite), "");
      SetStringValue(GetTypedTextKey(iSprite), "");

      SetBoolValue(GetTargetHitKey(iSprite), false);
   }

   SetIntValue("Score", 0);

   UpdateWidgetInfo();

   return true;
}

void CGameEngine::OnKey(SDL_keysym vKey)
{
   // TODO: discard irrelevant chars
   if (vKey.sym == SDLK_CAPSLOCK || vKey.sym == SDLK_LALT || vKey.sym == SDLK_RALT || vKey.sym == SDLK_LSHIFT ||
       vKey.sym == SDLK_RSHIFT || vKey.mod & KMOD_LALT || vKey.mod & KMOD_LALT)
      return;

   if (!isprint((char)vKey.sym))
      return;

   char aKey = TranslateKeysymToChar(vKey);
   HandleKeystroke(aKey);
}

void CGameEngine::OnTimer()
{
   UpdateWidgetInfo();
}

void CGameEngine::OnScreenStart(SDL_Surface* pScreen, bool& rbScreenNeedsRepaint)
{
   rbScreenNeedsRepaint = false;
   if (HasStringValue("PreGameMessage"))
   {
      MessageBox(GetStringValue("PreGameMessage"), pScreen, EMessageBoxType_OK);
      rbScreenNeedsRepaint = true;
   }
}

void CGameEngine::OnScreenEnd(SDL_Surface* pScreen)
{
}

void CGameEngine::UpdateWidgetInfo()
{
   // Check if all targets are hit.  (If they are, the game is over.)
   bool bAllTargetsHit = true;
   for (int iSprite = 0; iSprite < m_iNumberSprites; iSprite++)
   {
      if (!GetBoolValue(GetTargetHitKey(iSprite)))
      {
         bAllTargetsHit = false;
         break;
      }
   }
   if (bAllTargetsHit)
   {
      SetBoolValue("CompletedGame", false);
      SetStringValue("Action", "LessonComplete");
      return;
   }

   bool bAllSpritesFinished = true;
   for (int iSprite = 0; iSprite < m_iNumberSprites; iSprite++)
   {
      if (IsSpriteRunning(iSprite))
      {
         m_abSpriteRanThisIteration[iSprite] = true;
         bAllSpritesFinished = false;
      }
   }
   if (bAllSpritesFinished && m_asWordsToType.size() == 0)
   {
      SetBoolValue("CompletedGame", true);
      SetStringValue("Action", "LessonComplete");
      return;
   }

   for (int iSprite = 0; iSprite < m_iNumberSprites; iSprite++)
   {
      bool bResetSpriteText = false;
      if (SpriteTextIsTyped(iSprite) && IsSpriteRunning(iSprite))
      {
         IncrementScore(50);
         bResetSpriteText = true;
      }

      if (!IsSpriteRunning(iSprite) && GetStringValue(GetLessonTextKey(iSprite)) != "" && m_abSpriteRanThisIteration[iSprite])
      {
         IncrementScore(-70);
         bResetSpriteText = true;
      }

      if (bResetSpriteText)
      {
         SetStringValue(GetLessonTextKey(iSprite), "");
         SetStringValue(GetTypedTextKey(iSprite), "");
         if (iSprite == m_iSpriteCurrentlyBeingTyped)
            m_iSpriteCurrentlyBeingTyped = -1;
      }
   }

   int iNewSprite;
   if (ShouldShowNewSprite(iNewSprite))
   {
      if (m_asWordsToType.size() > 0)
      {
         int iNewWordOffset = 0;
         while (iNewWordOffset < m_asWordsToType.size()-1 && !CanUseWordForNewSprite(m_asWordsToType[iNewWordOffset]))
            iNewWordOffset++;
         SetStringValue(GetLessonTextKey(iNewSprite), m_asWordsToType[iNewWordOffset]);
         SetStringValue(GetTypedTextKey(iNewSprite), "");
         m_asWordsToType.RemoveAt(iNewWordOffset);
         m_abSpriteRanThisIteration[iNewSprite] = false;
      }
   }
}

void CGameEngine::HandleKeystroke(char aKey)
{
   if (m_iSpriteCurrentlyBeingTyped != -1)
   {
      CHTString sTypedValueKey = GetTypedTextKey(m_iSpriteCurrentlyBeingTyped);
      CHTString sTypedText = GetStringValue(sTypedValueKey);
      if (LastCharCorrect())
         sTypedText += CHTString(aKey);
      else
         sTypedText[sTypedText.Length()-1] = aKey;
      SetStringValue(sTypedValueKey, sTypedText);
      if (!LastCharCorrect())
         IncrementScore(-10);

      if (GetStringValue(GetLessonTextKey(m_iSpriteCurrentlyBeingTyped)) == "" || SpriteTextIsTyped(m_iSpriteCurrentlyBeingTyped))
         m_iSpriteCurrentlyBeingTyped = -1;
   }
   else
   {
      for (int iSprite = 0; iSprite < m_iNumberSprites; iSprite++)
      {
         if (GetStringValue(GetLessonTextKey(iSprite)).StartsWith(aKey) && IsSpriteTextVisible(iSprite))
         {
            m_iSpriteCurrentlyBeingTyped = iSprite;
            SetStringValue(GetTypedTextKey(iSprite), aKey);
            break;
         }
      }
   }

   UpdateWidgetInfo();
}

bool CGameEngine::LastCharCorrect()
{
   if (m_iSpriteCurrentlyBeingTyped == -1)
      return true;

   return (GetStringValue(GetLessonTextKey(m_iSpriteCurrentlyBeingTyped)).StartsWith(GetStringValue(GetTypedTextKey(m_iSpriteCurrentlyBeingTyped))));
}

bool CGameEngine::CanUseWordForNewSprite(CHTString sWord)
{
   for (int iSprite = 0; iSprite < m_iNumberSprites; iSprite++)
   {
      CHTString sSpriteWord = GetStringValue(GetLessonTextKey(iSprite));
      if (sSpriteWord != "" && sSpriteWord[0] == sWord[0])
         return false;
   }
   return true;
}

bool CGameEngine::ShouldShowNewSprite(OUT int& riSpriteNum)
{
   // We shoot for 66% of the sprites to be going at once
   CHTIntArray aiStoppedSprites;
   for (int iSprite = 0; iSprite < m_iNumberSprites; iSprite++)
   {
      if (!IsSpriteRunning(iSprite))
         aiStoppedSprites.Add(iSprite);
   }

   if (double(aiStoppedSprites.size()) / double(m_iNumberSprites) <= .34)
      return false;

   if (aiStoppedSprites.size() == 0)
      return false;

   int iNewSpriteIndex = rand() % aiStoppedSprites.size();
   riSpriteNum = aiStoppedSprites[iNewSpriteIndex];
   return true;
}

void CGameEngine::IncrementScore(int iAmtToIncrement)
{
   SetIntValue("Score", max(GetIntValue("Score")+iAmtToIncrement, 0)); // Score should never go below zero
}

CHTString CGameEngine::GetLessonTextKey(int iWidgetNum)
{
   return "LessonText"+IntToHTString(iWidgetNum);
}

CHTString CGameEngine::GetTypedTextKey(int iWidgetNum)
{
   return "TypedText"+IntToHTString(iWidgetNum);
}

CHTString CGameEngine::GetTargetHitKey(int iWidgetNum)
{
   return "TargetHit"+IntToHTString(iWidgetNum);
}

bool CGameEngine::IsSpriteRunning(int iWidgetNum)
{
   return GetBoolValue("SpriteRunning"+IntToHTString(iWidgetNum), false);
}

bool CGameEngine::IsSpriteTextVisible(int iWidgetNum)
{
   return GetBoolValue("SpriteTextVisible"+IntToHTString(iWidgetNum), false);
}

bool CGameEngine::SpriteTextIsTyped(int iWidgetNum)
{
   return GetStringValue(GetLessonTextKey(iWidgetNum)) != "" && GetStringValue(GetLessonTextKey(iWidgetNum)).IsSame(GetStringValue(GetTypedTextKey(iWidgetNum)));
}



CGameIterator::CGameIterator()
{
}

CGameIterator::~CGameIterator()
{
}

bool CGameIterator::Iterator_Init(CHTString sSettingsFile, CHTString& rsError)
{
   MungerDataFilePath(sSettingsFile);
   xmlDoc* pDoc = xmlReadFile(sSettingsFile.GetString(), NULL, 0);
   if (pDoc == NULL)
   {
      rsError = "Unable to read XML file " + sSettingsFile + ".";
      return false;
   }
   xmlNode* pRootElem = xmlDocGetRootElement(pDoc)->children;
   m_vGameSettings.LoadFromXml(pRootElem);
   return true;
}

CScreenEngine* CGameIterator::Iterator_GetFirstScreen(OUT CHTString& rsDisplayFile, CHTString& rsError)
{
   if (!m_vGameSettings.GetStringValue("DisplayFile", rsDisplayFile, rsError))
      return NULL;

   CGameEngine* pEngine = new CGameEngine;
   if (!pEngine->Load(&m_vGameSettings, rsError))
   {
      delete pEngine;
      return NULL;
   }
   // It would be nice if we could just display this message ourselves.  But if we display it
   // here, the game won't be drawn, which defeats the purpose of the message.
   CHTString sMessage;
   if (m_vGameSettings.GetStringValue("PreGameMessage", sMessage, rsError))
      pEngine->SetStringValue("PreGameMessage", sMessage);
   pEngine->SetIntValue("Level", 1);
   return pEngine;
}

CScreenEngine* CGameIterator::Iterator_GetNextScreen(CScreenEngine* pPrevScreen, OUT CHTString& rsDisplayFile, CHTString& rsError)
{
   // If the user completed the wave successfully, give 'em another wave
   if (pPrevScreen->GetBoolValue("CompletedGame"))
   {
      CGameEngine* pEngine = new CGameEngine;
      if (!pEngine->Load(&m_vGameSettings, rsError))
      {
         delete pEngine;
         return NULL;
      }
      pEngine->SetIntValue("Level", pPrevScreen->GetIntValue("Level")+1);
      pEngine->SetIntValue("Score", pPrevScreen->GetIntValue("Score"));
      return pEngine;
   }

   CHTString sMessage;
   if (m_vGameSettings.GetStringValue("PostGameMessage", sMessage, rsError))
   {
      sMessage.Replace("%score%", IntToHTString(pPrevScreen->GetIntValue("Score")));
      MessageBox(sMessage, m_pRootSurface, EMessageBoxType_OK);
   }

   return NULL;
}

bool CGameIterator::Iterator_OnScreenEnd(CScreenEngine* pScreen)
{
   return true;
}

CCollidingSpriteStateTracker::CCollidingSpriteStateTracker()
{
   m_WidgetRect = SDLRectFromDims(0,0,0,0);
   m_iPixelsPerIteration = 0;
   m_pUnreachedTargetImage = NULL;

   m_bEndReached = false;
   m_iCurrentTargetDestructionImage = 0;
   m_pCurTargetImage = NULL;

   Restart();
}

CCollidingSpriteStateTracker::~CCollidingSpriteStateTracker()
{
}

void CCollidingSpriteStateTracker::Initialize(SDL_Rect vWidgetRect, CSurfaceArray apSpriteImages, CSurfaceArray apSpriteDestructionImages,
                                              SDL_Surface* pUnreachedImage, CSurfaceArray apTargetDestructionImages)
{
   m_WidgetRect = vWidgetRect;
   m_apSpriteImages = apSpriteImages;
   m_apSpriteDestructionImages = apSpriteDestructionImages;
   m_pUnreachedTargetImage = pUnreachedImage;
   m_apTargetDestructionImages = apTargetDestructionImages;
}

void CCollidingSpriteStateTracker::AdvanceState()
{
   if (!IsIterating())
      Restart();

   m_iCurrentMovePos += m_iPixelsPerIteration; // Move sprite toward target

   // If not exploding, see if it should be (i.e. see if it hit the target)
   if (!m_bSpriteBeingDestroyed)
   {
      if (m_pCurTargetImage && m_iCurrentMovePos + m_apSpriteImages[0]->h >= m_WidgetRect.h - m_pCurTargetImage->h)
      {
         m_bEndReached = true;
         m_bSpriteBeingDestroyed = true;
         m_iCurrentSpriteImage = 0;
         m_pCurSpriteImage = NULL;
      }
   }

   // If we've hit the target, see if it should be exploding
   if (m_bEndReached)
   {
      if (m_iCurrentTargetDestructionImage < m_apTargetDestructionImages.size()-1)
         m_iCurrentTargetDestructionImage++;

      m_pCurTargetImage = m_apTargetDestructionImages[m_iCurrentTargetDestructionImage];
   }
   else
      m_pCurTargetImage = m_pUnreachedTargetImage;

   if (m_bSpriteBeingDestroyed)
   {
      // Handle exploding sprite
      if (m_iCurrentSpriteImage >= m_apSpriteDestructionImages.size())
         m_pCurSpriteImage = NULL; // We're done with this iteration.  The caller will need to call Restart to run us again.
      else
         m_pCurSpriteImage = m_apSpriteDestructionImages[m_iCurrentSpriteImage];
   }
   else
   {
      // Handle normal sprite
      if (m_iCurrentSpriteImage >= m_apSpriteImages.size())
         m_iCurrentSpriteImage = 0;

      if (m_apSpriteImages.size() > 0)
         m_pCurSpriteImage = m_apSpriteImages[m_iCurrentSpriteImage];
   }
   m_iCurrentSpriteImage++;
}

void CCollidingSpriteStateTracker::Restart()
{
   if (m_apSpriteImages.size() > 0)
      m_iCurrentMovePos = -m_apSpriteImages[0]->h; // Start the sprite off the screen
   else
      m_iCurrentMovePos = 0;

   m_bSpriteBeingDestroyed = false;
   m_iCurrentSpriteImage = 0;
   m_pCurSpriteImage = NULL;
   m_pCurTargetImage = NULL;
}

void CCollidingSpriteStateTracker::RegisterExternalHit()
{
   if (!m_bSpriteBeingDestroyed)
   {
      m_bSpriteBeingDestroyed = true;
      m_iCurrentSpriteImage = 0;
   }
}

bool CCollidingSpriteStateTracker::GetSpriteImageToDisplay(SDL_Surface*& rpImage, int& riX, int& riY)
{
   if (!IsIterating())
      return false;

   rpImage = m_pCurSpriteImage;
   if (!rpImage)
      return false;

   riX = m_WidgetRect.x;
   riY = m_WidgetRect.y + m_iCurrentMovePos;
   return true;
}

void CCollidingSpriteStateTracker::GetTargetImageToDisplay(SDL_Surface*& rpImage, int& riX, int& riY)
{
   rpImage = GetTargetImage();

   riX = m_WidgetRect.x;
   riY = m_WidgetRect.y + m_WidgetRect.h - rpImage->h; // place at bottom of widget
}

bool CCollidingSpriteStateTracker::TargetDestroyed()
{
   // The target must have been reached and all the destruction images displayed
   return m_bEndReached && m_iCurrentTargetDestructionImage == m_apSpriteDestructionImages.size()-1;
}






/////////////////////////////////////////////////////////////////////////////////////////////////

CWidget* GetCollidingSpriteWidget()
{
   return new CCollidingSprite;
}

bool DisplayCometGame(SDL_Surface* pScreen, CHTString sGameSettingsFile, CHTString& rsError)
{
   CScreenIterator* pNewIterator = new CGameIterator();
   if (!pNewIterator->Init(pScreen, sGameSettingsFile, rsError) || !pNewIterator->DisplayIteration(rsError))
      return false;

   return true;
}
