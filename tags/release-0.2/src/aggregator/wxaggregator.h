
#ifndef _WXAGGREGATOR_H_
#define _WXAGGREGATOR_H_

#include "typing_history_aggregator.h"
#include <wx/grid.h>

#include "grid.h"

class wxaggregatorapp : public wxApp
{
public:
   virtual bool OnInit();
};

class wxaggregatorFrame : public wxFrame
{
public:
   wxaggregatorFrame( const wxString& title, const wxPoint& pos, const wxSize& pos );

protected:

   // The main build function.
   bool Build(CHTString& rsError);
   void OnBuild(wxCommandEvent&);
   void OnKey(wxKeyEvent& rEvent);

   wxString* m_asUserOptions;
   wxString* m_asDateOptions;
   wxString* m_asGroupingOptions;

   // Aggregation objects
   CTypingHistoryAggregator m_Aggregator;

protected:
   wxString GetChoice(int iControlID);

   // Controls
   wxGrid* m_pMainGrid;
   wxChoice* m_pDateSubsetChoice;
   wxChoice* m_pUserSubsetChoice;
   wxChoice* m_pFirstGroupingChoice;
   wxChoice* m_pSecondGroupingChoice;
   wxCheckBox* m_pTotalsOnlyCheckbox;
   wxButton* m_pBuildButton;

private:
   DECLARE_EVENT_TABLE();
};

enum
{
   DateSubsetChoice_ID = 100,
   UserSubsetChoice_ID,
   FirstGroupingChoice_ID,
   SecondGroupingChoice_ID,
   BuildButton_ID,
   MainGrid_ID,
};

#endif // _WXAGGREGATOR_H_
