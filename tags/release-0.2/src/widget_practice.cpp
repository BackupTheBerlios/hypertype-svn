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

#include <assert.h>

#include "font_aid.h"
#include "OS.h"
#include "lesson.h"
#include "screen.h"
#include "widget_practice.h"
#include "widget_text.h"

class CScatteredCharLinePainter
{
public:
   CScatteredCharLinePainter();
   ~CScatteredCharLinePainter();

   void Initialize(TTF_Font* pFont, int iLineTop, int iLineLeft, SDL_Color vTextColor);

   void PaintChars(CHTString sOffsetCalculationText, CHTString sPaintableText, CPaintingParms* pPaintingParms, CHTIntArray aiIndicesOfCharsToPaint);

protected:
   CHTString m_sLineText;
   int m_iTop;
   int m_iLeft;
   TTF_Font* m_pFont;
   SDL_Color m_TextColor;
   CHTIntArray m_aiPaintedCharIndices;
};

CScatteredCharLinePainter::CScatteredCharLinePainter()
{
   m_pFont = NULL;
   m_iTop = -1;
   m_iLeft = -1;
}

CScatteredCharLinePainter::~CScatteredCharLinePainter()
{
}

void CScatteredCharLinePainter::Initialize(TTF_Font* pFont, int iLineTop, int iLineLeft, SDL_Color vTextColor)
{
   m_pFont = pFont;
   m_iTop = iLineTop;
   m_iLeft = iLineLeft;
   m_TextColor = vTextColor;
}

void CScatteredCharLinePainter::PaintChars(CHTString sOffsetCalculationText, CHTString sPaintableText, CPaintingParms* pPaintingParms, CHTIntArray aiIndicesOfCharsToPaint)
{
   if (!m_sLineText.IsSame(sPaintableText))
   {
      m_aiPaintedCharIndices.clear();
      m_sLineText = sPaintableText;
   }

   for (int iCharIndex = 0; iCharIndex < aiIndicesOfCharsToPaint.size(); iCharIndex++)
   {
      int iIndexOfCurChar = aiIndicesOfCharsToPaint[iCharIndex];
      if (m_aiPaintedCharIndices.LinearFind(iIndexOfCurChar) == -1)
      {
         int iCharOffset = CalcTextWidth(m_pFont, sOffsetCalculationText.Left(iIndexOfCurChar));
         SDL_Rect vRect;
         vRect.x = m_iLeft + iCharOffset;
         vRect.y = m_iTop;
         vRect.w = 100; // (that's a big character!)
         vRect.h = 100;
         DrawText(m_sLineText[iIndexOfCurChar], pPaintingParms->m_pDrawingSurface, m_pFont, m_TextColor, vRect, ETextHAlign_Left, ETextVAlign_Top);
         pPaintingParms->m_aRectsToUpdate.Add(vRect);
      }
   }

   m_aiPaintedCharIndices = aiIndicesOfCharsToPaint;
}



CPracticeWidget::CPracticeWidget() : CWidget()
{
   m_sFont = "fonts/VeraMono.ttf";
   m_iDisplayedTextOffset = 0;
}

CPracticeWidget::~CPracticeWidget()
{
   EmptyPtrArray(m_aLessonTextLines);
   EmptyPtrArray(m_aTypedTextLines);
   EmptyPtrArray(m_aBadCharPainters);
}

void CPracticeWidget::PaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   CWidget::OnPaintWidget(pPaintingParms->m_pDrawingSurface);
   InternalPaintWidget(pPaintingParms, pEngine, true);
}

bool CPracticeWidget::WantsKeyboardEvents()
{
   return true;
}

void CPracticeWidget::HandleKeyboardEvent(const SDL_Event& rEvent, CPaintingParms* pPaintingParms, CScreenEngine* pEngine)
{
   InternalPaintWidget(pPaintingParms, pEngine, false);
}

void CPracticeWidget::LoadSettings(CKeyedValues& rSettings)
{
   LoadRectSettings(rSettings);
}

CTextWidget* CPracticeWidget::CreateTextLine(int iTop, int iHeight, int iLeft, int iWidth, CHTString& rsError)
{
   CTextWidget* pWidget = new CTextWidget;
   pWidget->SetTop(iTop);
   pWidget->SetLeft(iLeft);
   pWidget->SetWidth(iWidth);
   pWidget->SetHeight(iHeight);
   pWidget->SetShouldWrap(false);
   pWidget->SetHAlign(ETextHAlign_Left);
   pWidget->SetVAlign(ETextVAlign_Bottom);
   if (!pWidget->SetFont(m_sFont, 14, rsError))
   {
      delete pWidget;
      return NULL;
   }

   return pWidget;
}

bool CPracticeWidget::ValidateSettings(CHTString& rsElementaryError)
{
   if (!ValidateRectSettings(true, rsElementaryError))
      return false;

   int iTextHeight = 0;
   {
      CTextWidget vHeightTest;
      if (!vHeightTest.SetFont(m_sFont, 14, rsElementaryError))
         return false;
      iTextHeight = vHeightTest.GetTextHeight(); // Double height to account for both lesson and typed text
   }

   int iUsedHeight = 0;
   const int iTextMargin = 10;
   while ((int)GetWidgetRect().h-iUsedHeight > iTextHeight*2)
   {
      {
         CTextWidget* pWidget = CreateTextLine(m_iWidgetTop+iUsedHeight, iTextHeight, m_iWidgetLeft+iTextMargin, m_iWidgetWidth-iTextMargin, rsElementaryError);
         if (!pWidget)
            return false;
         m_aLessonTextLines.Add(pWidget);
      }
      iUsedHeight += iTextHeight;

      {
         CTextWidget* pWidget = CreateTextLine(m_iWidgetTop+iUsedHeight, iTextHeight, m_iWidgetLeft+iTextMargin, m_iWidgetWidth-iTextMargin, rsElementaryError);
         if (!pWidget)
            return false;
         m_aTypedTextLines.Add(pWidget);

         CScatteredCharLinePainter* pBadCharPainter = new CScatteredCharLinePainter;
         TTF_Font* pFont = GetOrLoadFont(m_sFont, 14, rsElementaryError);
         if (!pFont) return false;
         pBadCharPainter->Initialize(pFont, m_iWidgetTop+iUsedHeight, m_iWidgetLeft+iTextMargin, SDLColorFromRGB(255, 0, 0));
         m_aBadCharPainters.Add(pBadCharPainter);
      }
      iUsedHeight += iTextHeight*2;
   }

   return true;
}

void CPracticeWidget::PaintIncorrectChars(CPaintingParms* pPaintingParms, CScreenEngine* pEngine, bool bForcePaint)
{
   for (int iTextLine = 0; iTextLine < m_aTypedTextLines.size(); iTextLine++)
   {
      CHTString sTypedText = pEngine->GetStringValue(FormatString("TypedTextLine%i", m_iDisplayedTextOffset+iTextLine), false);
      CHTString sLessonText = pEngine->GetStringValue(FormatString("LessonTextLine%i", m_iDisplayedTextOffset+iTextLine), false);
      CHTString sBadChars;

      CHTIntArray aiIndicesOfBadChars;
      for (int iChar = 0; iChar < sTypedText.Length(); iChar++)
      {
         if (iChar >= sLessonText.Length() || sTypedText[iChar] != sLessonText[iChar])
            aiIndicesOfBadChars.Add(iChar);
      }
      sTypedText.Replace(" ", "_");
      m_aBadCharPainters[iTextLine]->PaintChars(sLessonText, sTypedText, pPaintingParms, aiIndicesOfBadChars);
   }
}

void CPracticeWidget::InternalPaintWidget(CPaintingParms* pPaintingParms, CScreenEngine* pEngine, bool bForcePaint)
{
   for (int iTextLine = 0; iTextLine < m_aLessonTextLines.size(); iTextLine++)
   {
      CHTString sCurLineText = pEngine->GetStringValue(FormatString("LessonTextLine%i", m_iDisplayedTextOffset+iTextLine), false);
      m_aLessonTextLines[iTextLine]->UpdatePaintedText(sCurLineText, pPaintingParms, bForcePaint);
   }

   for (int iTextLine = 0; iTextLine < m_aTypedTextLines.size(); iTextLine++)
   {
      CHTString sCurLineText = pEngine->GetStringValue(FormatString("TypedTextLine%i", m_iDisplayedTextOffset+iTextLine), false);
      m_aTypedTextLines[iTextLine]->UpdatePaintedText(sCurLineText, pPaintingParms, bForcePaint);
   }

   PaintIncorrectChars(pPaintingParms, pEngine, bForcePaint);

   EMistakeHandling eMistakeHandling;
   if (!TextToMistakeHandling(pEngine->GetStringValue("MistakeHandling"), eMistakeHandling))
   {
      assert(false);
      return;
   }

   if (pEngine->GetBoolValue("LastKeyCorrect") || eMistakeHandling != EMistakeHandling_MustCorrect)
   {  // check for need to scroll screen
      int iLastTypedLine = m_aTypedTextLines.size()-1;
      if (m_aTypedTextLines[iLastTypedLine]->GetDisplayedText() != "" &&
          m_aTypedTextLines[iLastTypedLine]->GetDisplayedText().Length() == m_aLessonTextLines[iLastTypedLine]->GetDisplayedText().Length())
      {
         if (pEngine->GetStringValue("TypedText").Length() == pEngine->GetStringValue("LessonText").Length())
            return; // done w/ lesson -- kick out.
         m_iDisplayedTextOffset += iLastTypedLine+1;
         PaintWidget(pPaintingParms, pEngine);
      }

      if (pEngine->GetIntValue("LastTypedLine") < m_iDisplayedTextOffset)
      {
         CHTString sTypedLineText = pEngine->GetStringValue(FormatString("TypedTextLine%i", pEngine->GetIntValue("LastTypedLine")), false);
         CHTString sLessonLineText = pEngine->GetStringValue(FormatString("LessonTextLine%i", pEngine->GetIntValue("LastTypedLine")), false);
         if (sTypedLineText.Length() < sLessonLineText.Length())
         {
            m_iDisplayedTextOffset = pEngine->GetIntValue("LastTypedLine");
            PaintWidget(pPaintingParms, pEngine);
         }
      }
   }
}

