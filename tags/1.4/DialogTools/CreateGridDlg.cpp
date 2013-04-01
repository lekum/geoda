/**
 * GeoDa TM, Copyright (C) 2011-2013 by Luc Anselin - all rights reserved
 *
 * This file is part of GeoDa.
 * 
 * GeoDa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GeoDa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/xrc/xmlres.h>
#include <wx/valtext.h>
#include <wx/filedlg.h>

#include "../ShapeOperations/shp.h"
#include "../ShapeOperations/shp2cnt.h"
#include "../GeoDa.h"
#include "../TemplateCanvas.h"
#include "../ShapeOperations/ShapeFileHdr.h"
#include "CreateGridDlg.h"

IMPLEMENT_CLASS( CreateGridDlg, wxDialog )

BEGIN_EVENT_TABLE( CreateGridDlg, wxDialog )
    EVT_BUTTON( XRCID("IDCANCEL"), CreateGridDlg::OnCancelClick )
    EVT_BUTTON( XRCID("IDC_REFERENCEFILE"),
			   CreateGridDlg::OnCReferencefileClick )
    EVT_BUTTON( XRCID("IDC_BROWSE_OFILE"), CreateGridDlg::OnCBrowseOfileClick )
    EVT_BUTTON( XRCID("IDC_REFERENCEFILE2"),
			   CreateGridDlg::OnCReferencefile2Click )
    EVT_TEXT( XRCID("IDC_EDIT1"), CreateGridDlg::OnCEdit1Updated )
    EVT_TEXT( XRCID("IDC_EDIT3"), CreateGridDlg::OnCEdit3Updated )
    EVT_TEXT( XRCID("IDC_EDIT2"), CreateGridDlg::OnCEdit2Updated )
    EVT_TEXT( XRCID("IDC_EDIT4"), CreateGridDlg::OnCEdit4Updated )
    EVT_BUTTON( XRCID("ID_CREATE"), CreateGridDlg::OnCreateClick )
    EVT_RADIOBUTTON( XRCID("IDC_RADIO1"), CreateGridDlg::OnCRadio1Selected )
    EVT_RADIOBUTTON( XRCID("IDC_RADIO2"), CreateGridDlg::OnCRadio2Selected )
    EVT_RADIOBUTTON( XRCID("IDC_RADIO3"), CreateGridDlg::OnCRadio3Selected )
END_EVENT_TABLE()

CreateGridDlg::CreateGridDlg( )
{
}

CreateGridDlg::CreateGridDlg( wxWindow* parent, wxWindowID id,
							   const wxString& caption, const wxPoint& pos,
							   const wxSize& size, long style )
{
	isCreated = false;

    Create(parent, id, caption, pos, size, style);

	m_check = 1;


	m_xBot=m_yBot=m_xTop=m_yTop = 0.0;
	EnableItems();
	hasCreated = false;

	s_lower_x = "0.0";
	s_lower_y =  "0.0";
	s_top_x =  "0.0";
	s_top_y =  "0.0";

	s_row =  "2";
	s_col =  "2";

	isCreated = true;
}

bool CreateGridDlg::Create( wxWindow* parent, wxWindowID id,
							const wxString& caption, const wxPoint& pos,
							const wxSize& size, long style )
{
    SetParent(parent);
    CreateControls();
    Centre();

    return true;
}
  
void CreateGridDlg::CreateControls()
{   
    wxXmlResource::Get()->LoadDialog(this, GetParent(), "IDD_CREATE_GRID");
    m_outputfile = XRCCTRL(*this, "IDC_EDIT9", wxTextCtrl);
	m_outputfile->SetMaxLength(0);
    m_inputfile_ascii = XRCCTRL(*this, "IDC_EDIT6", wxTextCtrl);
	m_inputfile_ascii->SetMaxLength(0);
    m_lower_x = XRCCTRL(*this, "IDC_EDIT1", wxTextCtrl);
    m_upper_x = XRCCTRL(*this, "IDC_EDIT3", wxTextCtrl);
    m_lower_y = XRCCTRL(*this, "IDC_EDIT2", wxTextCtrl);
    m_upper_y = XRCCTRL(*this, "IDC_EDIT4", wxTextCtrl);
    m_inputfileshp = XRCCTRL(*this, "IDC_EDIT5", wxTextCtrl);
	m_inputfileshp->SetMaxLength(0);
    m_rows = XRCCTRL(*this, "IDC_EDIT7", wxTextCtrl);
    m_cols = XRCCTRL(*this, "IDC_EDIT8", wxTextCtrl);

    if (FindWindow(XRCID("IDC_EDIT1"))) {
        FindWindow(XRCID("IDC_EDIT1"))->
			SetValidator( wxTextValidator(wxFILTER_NUMERIC, & s_lower_x) );
	}
    if (FindWindow(XRCID("IDC_EDIT3"))) {
        FindWindow(XRCID("IDC_EDIT3"))->
			SetValidator( wxTextValidator(wxFILTER_NUMERIC, & s_top_x) );
	}
    if (FindWindow(XRCID("IDC_EDIT2"))) {
        FindWindow(XRCID("IDC_EDIT2"))->
			SetValidator( wxTextValidator(wxFILTER_NUMERIC, & s_lower_y) );
	}
    if (FindWindow(XRCID("IDC_EDIT4"))) {
        FindWindow(XRCID("IDC_EDIT4"))->
			SetValidator( wxTextValidator(wxFILTER_NUMERIC, & s_top_y) );
	}
    if (FindWindow(XRCID("IDC_EDIT7"))) {
        FindWindow(XRCID("IDC_EDIT7"))->
			SetValidator( wxTextValidator(wxFILTER_NUMERIC, & s_row) );
	}
    if (FindWindow(XRCID("IDC_EDIT8"))) {
        FindWindow(XRCID("IDC_EDIT8"))->
			SetValidator( wxTextValidator(wxFILTER_NUMERIC, & s_col) );
	}
}

void CreateGridDlg::OnCancelClick( wxCommandEvent& event )
{
	event.Skip();
	EndDialog(wxID_CANCEL);	
}

void CreateGridDlg::OnCReferencefileClick( wxCommandEvent& event )
{
   wxFileDialog dlg( this, "Input ASCII file", "", "",
                    "ASCII files (*.*)|*.*");

	wxString m_path = wxEmptyString;
	bool m_polyid = false;

    if (dlg.ShowModal() == wxID_OK) {
		m_path  = dlg.GetPath();
		wxString InFile = m_path;
		m_inputfile_ascii->SetValue(InFile);

                fn = dlg.GetFilename();
                int pos = fn.Find('.', true);
                if (pos >= 0) fn = fn.Left(pos);

		ifstream    ifl(m_path.mb_str(), ios::in);
		if (ifl.fail())  {
			wxMessageBox("File doesn't exist!");
			return;
		}

		int m_nRows;
		int m_nCols;

		ifl >> m_nRows;
		if (ifl.fail()) return;
		ifl >> m_nCols;
		if (ifl.fail()) return;
		ifl >> m_xBot;
		if (ifl.fail()) return;
		ifl >> m_yBot;
		if (ifl.fail()) return;
		ifl >> m_xTop;
		if (ifl.fail()) return;
		ifl >> m_yTop;
		if (ifl.fail()) return;

		wxString s_xBot, s_yBot, s_xTop, s_yTop;

		s_xBot.Format("%f",m_xBot);
		s_yBot.Format("%f",m_yBot);
		s_xTop.Format("%f",m_xTop);
		s_yTop.Format("%f",m_yTop);


		m_lower_x->SetValue(s_xBot);
		m_lower_y->SetValue(s_yBot);
		m_upper_x->SetValue(s_xTop);
		m_upper_y->SetValue(s_yTop);

		wxString co;
		wxString ro;

		co.Format("%d", m_nCols);
		ro.Format("%d", m_nRows);

		m_rows->SetValue(co);
		m_cols->SetValue(ro);
	}	
	EnableItems();

}

void CreateGridDlg::OnCBrowseOfileClick( wxCommandEvent& event )
{
     wxFileDialog dlg ( this, "Output Shp file", wxEmptyString, fn + ".shp",
					   "Shp files (*.shp)|*.shp",
					   wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	wxString	m_path = wxEmptyString;

    if (dlg.ShowModal() == wxID_OK) {
		m_path  = dlg.GetPath();
		wxString OutFile = m_path;
		m_outputfile->SetValue(OutFile);
	}

	EnableItems();
}

void CreateGridDlg::OnCReferencefile2Click( wxCommandEvent& event )
{
	wxFileDialog dlg( this, "Input Shp file", "", "",
					 "Shp files (*.shp)|*.shp" );

	wxString	m_path = wxEmptyString;
	bool m_polyid = false;

    if (dlg.ShowModal() == wxID_OK) {
		m_path  = dlg.GetPath();
		wxString InFile = m_path;
		m_inputfileshp->SetValue(InFile);

        fn = dlg.GetFilename();
        int pos = fn.Find('.', true);
        if (pos >= 0) fn = fn.Left(pos);
        
		iShapeFile     shp_ref(m_path, "shp");
	    char           hs_ref[ 2*GeoDaConst::ShpHeaderSize ];
	    shp_ref.read(hs_ref, 2*GeoDaConst::ShpHeaderSize);
	    ShapeFileHdr  hd_ref(hs_ref);
		m_BigBox = hd_ref.BoundingBox();
		m_xBot = m_BigBox._min().x;
		m_yBot = m_BigBox._min().y;
		m_xTop = m_BigBox._max().x;
		m_yTop = m_BigBox._max().y;

	}
	EnableItems();
}

void CreateGridDlg::OnCreateClick( wxCommandEvent& event )
{
	if (CheckBBox()) {
		CreateGrid();
		hasCreated = true;
	} else {
		wxMessageBox("Please fix the grid bounding box!");
		return;
	}
	event.Skip();
	EndDialog(wxID_OK);
}

void CreateGridDlg::OnCRadio1Selected( wxCommandEvent& event )
{
    m_check = 1;
	EnableItems();
}

void CreateGridDlg::OnCRadio2Selected( wxCommandEvent& event )
{
	m_check = 2;
	EnableItems();
}

void CreateGridDlg::OnCRadio3Selected( wxCommandEvent& event )
{
    m_check = 3;
	EnableItems();
}

void CreateGridDlg::EnableItems()
{
	FindWindow(XRCID("IDC_EDIT1"))->Enable((m_check == 1));
	FindWindow(XRCID("IDC_EDIT2"))->Enable((m_check == 1));
	FindWindow(XRCID("IDC_EDIT3"))->Enable((m_check == 1));
	FindWindow(XRCID("IDC_EDIT4"))->Enable((m_check == 1));
	FindWindow(XRCID("IDC_REFERENCEFILE"))->Enable((m_check == 2));
	FindWindow(XRCID("IDC_REFERENCEFILE2"))->Enable((m_check == 3));

	wxString m_oSHAPE = m_outputfile->GetValue();

	FindWindow(XRCID("ID_CREATE"))->Enable(m_oSHAPE.Length() > 0 &&
										   (m_xBot < m_xTop &&
											m_yBot < m_yTop));
	FindWindow(XRCID("IDC_EDIT7"))->Enable((m_check != 2));
	FindWindow(XRCID("IDC_EDIT8"))->Enable((m_check != 2));
	if (m_check != 2) m_inputfile_ascii->SetValue(wxEmptyString);
	if (m_check != 3) m_inputfileshp->SetValue(wxEmptyString);
}


bool CreateGridDlg::CheckBBox()
{
	if (m_xBot >= m_xTop || m_yBot >= m_yTop ) {
		wxString xx;
		xx.Format("Bounding Box: (%f,%f) (%f,%f)",m_xBot,m_yBot,m_xTop,m_yTop);
		wxMessageBox(xx);
		return false;
	}
	return true;
}

void CreateGridDlg::CreateGrid()
{
	FindWindow(XRCID("ID_CREATE"))->Enable(false);
	FindWindow(XRCID("IDCANCEL"))->Enable(false);

	long int m_nCols, m_nRows;
	m_rows->GetValue().ToLong(&m_nRows);
	m_cols->GetValue().ToLong(&m_nCols);

	double *x = new double[m_nCols + 1];
	double *y = new double[m_nRows + 1];
	double const x_range = m_xTop - m_xBot;
	double const y_range = m_yTop - m_yBot;
	double const d_x = x_range / m_nCols;
	double const d_y = y_range / m_nRows;

	x[0] = m_xBot;
	x[m_nCols] = m_xTop;
	int i = 0;
	for (i = 1;i < m_nCols; i++) x[i] = x[i-1] + d_x;

	y[0] = m_yBot;
	y[m_nRows] = m_yTop;
	for (i = 1;i < m_nRows; i++) y[i] = y[i-1] + d_y;

	myBox BB;
	double const eps_x = fabs(m_xTop - m_xBot) / 1000000.0;
	double const eps_y = fabs(m_yTop - m_yBot) / 1000000.0;

	BB.p1.x = m_xBot - eps_x;BB.p1.y = m_yBot - eps_y;
	BB.p2.x = m_xTop + eps_x;BB.p2.y = m_yTop + eps_y;
	wxString m_oSHAPE = m_outputfile->GetValue();

	CreateGridShapeFile(m_oSHAPE, m_nRows, m_nCols, x, y, BB); 

	m_nCount = nMaxCount;

	FindWindow(XRCID("ID_CREATE"))->Enable(true);
	FindWindow(XRCID("IDCANCEL"))->Enable(true);

	delete [] x;
	x = NULL;
	delete [] y;
	y = NULL;
}

void CreateGridDlg::OnCEdit1Updated( wxCommandEvent& event )
{
	if (!isCreated) return;
	m_lower_x->GetValue().ToDouble(&m_xBot);
	EnableItems();
    
}

void CreateGridDlg::OnCEdit2Updated( wxCommandEvent& event )
{
	if (!isCreated) return;
	m_lower_y->GetValue().ToDouble(&m_yBot);
	EnableItems();
}

void CreateGridDlg::OnCEdit3Updated( wxCommandEvent& event )
{
	if (!isCreated) return;
	m_upper_x->GetValue().ToDouble(&m_xTop);
	EnableItems();
}

void CreateGridDlg::OnCEdit4Updated( wxCommandEvent& event )
{
	if (!isCreated) return;
	m_upper_y->GetValue().ToDouble(&m_yTop);
	EnableItems();
}
