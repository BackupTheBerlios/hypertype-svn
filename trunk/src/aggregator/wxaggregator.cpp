#include <wx/wx.h>
#include <wx/grid.h>
#include "wxaggregator.h"

#include "grid_wx.h"

BEGIN_EVENT_TABLE( wxaggregatorFrame, wxFrame )
   EVT_BUTTON(BuildButton_ID, wxaggregatorFrame::OnBuild)
   EVT_KEY_DOWN(wxaggregatorFrame::OnKey)
END_EVENT_TABLE()

IMPLEMENT_APP(wxaggregatorapp)

wxString* HTStringArrayToWXStringArray(const CHTStringArray& rasArray_c)
{
   wxString* pWXArray = new wxString[rasArray_c.size()];
   for (int iString = 0; iString < rasArray_c.size(); iString++)
      pWXArray[iString] = wxString(rasArray_c[iString].GetString(), wxConvUTF8);

   return pWXArray;
}

bool wxaggregatorapp::OnInit()
{
   wxaggregatorFrame *pTheFrame = new wxaggregatorFrame(wxT( "HyperType data aggregator" ), wxPoint(50,50), wxSize(550,340));

   pTheFrame->Show(TRUE);
   SetTopWindow(pTheFrame);
   return TRUE;
}

void wxaggregatorFrame::OnKey(wxKeyEvent& rEvent)
{
}

wxaggregatorFrame::wxaggregatorFrame( const wxString& title, const wxPoint& pos, const wxSize& size ) : wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
   CreateStatusBar();

   // Initialize
   CHTString sWarning;
   m_Aggregator.Initialize(sWarning);
   SetStatusText(wxString(sWarning, wxConvUTF8));

   CHTStringArray asTmp;
   m_Aggregator.GetUserFilterOptions(asTmp);
   int iNumUserOptions = asTmp.size();
   m_asUserOptions = HTStringArrayToWXStringArray(asTmp);

   m_Aggregator.GetDateFilterOptions(asTmp);
   int iNumDateOptions = asTmp.size();
   m_asDateOptions = HTStringArrayToWXStringArray(asTmp);

   m_Aggregator.GetGroupingOptions(asTmp);
   int iNumGroupingOptions = asTmp.size();
   m_asGroupingOptions = HTStringArrayToWXStringArray(asTmp);

   // Create controls
   wxPanel* pMainPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_BORDER);

   m_pDateSubsetChoice = new wxChoice(pMainPanel, DateSubsetChoice_ID, wxPoint(10,10), wxSize(-1,-1), iNumDateOptions, m_asDateOptions);
   wxStaticText* pDateSubsetLabel = new wxStaticText(pMainPanel, -1, wxT("&Date Subset"), wxPoint(10,10));
   m_pUserSubsetChoice = new wxChoice(pMainPanel, UserSubsetChoice_ID, wxPoint(100,10), wxSize(-1,-1), iNumUserOptions, m_asUserOptions);
   wxStaticText* pUserSubsetLabel = new wxStaticText(pMainPanel, -1, wxT("&User Subset"), wxPoint(10,10));
   m_pFirstGroupingChoice = new wxChoice(pMainPanel, FirstGroupingChoice_ID, wxPoint(200,10), wxSize(-1,-1), iNumGroupingOptions, m_asGroupingOptions);
   wxStaticText* pFirstGroupingLabel = new wxStaticText(pMainPanel, -1, wxT("&First Grouping"), wxPoint(10,10));
   m_pSecondGroupingChoice = new wxChoice(pMainPanel, SecondGroupingChoice_ID, wxPoint(300,10), wxSize(-1,-1), iNumGroupingOptions, m_asGroupingOptions);
   wxStaticText* pSecondGroupingLabel = new wxStaticText(pMainPanel, -1, wxT("&Second Grouping"), wxPoint(10,10));
   m_pTotalsOnlyCheckbox = new wxCheckBox(pMainPanel, -1, wxT("&Totals Only?"), wxPoint(400, 10));
   m_pBuildButton = new wxButton(pMainPanel, BuildButton_ID, wxT("&Build"), wxPoint(500, 10));
   m_pMainGrid = new wxGrid(pMainPanel, MainGrid_ID, wxPoint(10,70), wxSize(600,500), 0, wxPanelNameStr);
   int iFlag = m_pMainGrid->GetWindowStyleFlag();
   cout << iFlag << endl;
   iFlag &= (~wxWANTS_CHARS);
   cout << iFlag << endl;
   m_pMainGrid->SetWindowStyleFlag(iFlag);
   m_pMainGrid->CreateGrid(0, 0);
   //m_pMainGrid->SetEvtHandlerEnabled(false);

   // Create sizers
   wxGridSizer* pChoiceControlSizer = new wxGridSizer(0, 4, 0, 0);
   pChoiceControlSizer->Add(pDateSubsetLabel, 0, wxTOP|wxLEFT, 10);
   pChoiceControlSizer->Add(pUserSubsetLabel, 0, wxTOP|wxLEFT, 10);
   pChoiceControlSizer->Add(pFirstGroupingLabel, 0, wxTOP|wxLEFT, 10);
   pChoiceControlSizer->Add(pSecondGroupingLabel, 0, wxTOP|wxLEFT, 10);
   pChoiceControlSizer->Add(m_pDateSubsetChoice, 0, wxLEFT, 10);
   pChoiceControlSizer->Add(m_pUserSubsetChoice, 0, wxLEFT, 10);
   pChoiceControlSizer->Add(m_pFirstGroupingChoice, 0, wxLEFT, 10);
   pChoiceControlSizer->Add(m_pSecondGroupingChoice, 0, wxLEFT, 10);

   wxBoxSizer* pControlsSizer = new wxBoxSizer(wxHORIZONTAL);
   pControlsSizer->Add(pChoiceControlSizer, 0, wxALIGN_LEFT);
   pControlsSizer->Add(m_pTotalsOnlyCheckbox, 0, wxLEFT|wxRIGHT|wxALIGN_BOTTOM, 10);
   pControlsSizer->Add(m_pBuildButton, 0, wxLEFT|wxRIGHT|wxALIGN_BOTTOM, 10);

   wxBoxSizer* pGridSizer = new wxBoxSizer(wxHORIZONTAL);
   pGridSizer->Add(m_pMainGrid, 1, wxEXPAND|wxALL, 10);

   wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
   pMainSizer->Add(pControlsSizer, 0, wxALIGN_LEFT);
   pMainSizer->Add(pGridSizer, 1, wxEXPAND|wxALL, 10);

   pMainPanel->SetSizer(pMainSizer);
   pMainSizer->SetSizeHints(this);
   m_pBuildButton->SetDefault();
}

bool wxaggregatorFrame::Build(CHTString& rsError)
{
   if (!m_Aggregator.Build_CustomSettings(m_pUserSubsetChoice->GetStringSelection().c_str(), m_pDateSubsetChoice->GetStringSelection().c_str(),
        m_pFirstGroupingChoice->GetStringSelection().c_str(), m_pSecondGroupingChoice->GetStringSelection().c_str(), m_pTotalsOnlyCheckbox->GetValue(), rsError))
   {
      return false;
   }

   m_Aggregator.PopulateDataGrid(*m_pMainGrid);
   return true;
}

void wxaggregatorFrame::OnBuild(wxCommandEvent&)
{
   CHTString sError;
   if (!Build(sError))
   {
      wxMessageDialog dlg(this, wxString(sError, wxConvUTF8), wxT("Error"), wxOK);
      dlg.ShowModal();
   }
}


