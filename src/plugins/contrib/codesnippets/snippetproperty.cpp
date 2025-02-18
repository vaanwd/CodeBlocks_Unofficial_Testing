/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
    Parts Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
// ----------------------------------------------------------------------------
//  SnippetProperty.cpp                                         //(pecan 2006/9/12)
// ----------------------------------------------------------------------------

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
    #include <wx/datetime.h>
#endif
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/dnd.h>

//-#if defined(BUILDING_PLUGIN)
#include "sdk.h"
//-#endif

#include "codesnippetswindow.h"
#include "snippetitemdata.h"
#include "menuidentifiers.h"
#include "snippetsconfig.h"
#include "snippetproperty.h"
////-#include "defsext.h"
////-#include "dragscrollevent.h"
#include "version.h"
//#include "wxscintilla/include/wx/wxscintilla.h"   //svn5785
#include <wx/wxscintilla.h>                         //svn5785

BEGIN_EVENT_TABLE(SnippetProperty, SnippetPropertyForm)
    EVT_BUTTON(wxID_OK,             SnippetProperty::OnOk)
    EVT_BUTTON(wxID_CANCEL,         SnippetProperty::OnCancel)
    EVT_BUTTON(ID_SNIPPETBUTTON,    SnippetProperty::OnSnippetButton)
    EVT_BUTTON(ID_FILESELECTBUTTON, SnippetProperty::OnFileSelectButton)
    //-EVT_LEAVE_WINDOW(               SnippetProperty::OnLeaveWindow)

END_EVENT_TABLE()

// ----------------------------------------------------------------------------
class SnippetDropTarget : public wxTextDropTarget
// ----------------------------------------------------------------------------
{
        // Drop target used to place dragged data into Properties dialog

    public:
        SnippetDropTarget(SnippetProperty * window) : m_Window(window) {}
        ~SnippetDropTarget() {}
        bool OnDropText(wxCoord x, wxCoord y, const wxString & data);
    private:
        SnippetProperty * m_Window;
};
// ----------------------------------------------------------------------------
bool SnippetDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString & data)
// ----------------------------------------------------------------------------
{
    // Put dragged text into SnippetTextCtrl
    wxUnusedVar(x);
    wxUnusedVar(y);
#ifdef LOGGING
    LOGIT(_T("Dragged Data[%s]"), data.GetData());
#endif //LOGGING
    //m_Window->m_SnippetEditCtrl->WriteText(data);
    m_Window->m_SnippetEditCtrl->AddText(data);
    return true;
} // end of OnDropText
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  SnippetProperty methods
// ----------------------------------------------------------------------------
SnippetProperty::SnippetProperty(wxTreeCtrl * pTree, wxTreeItemId itemId, wxSemaphore * pWaitSem)
// ----------------------------------------------------------------------------
    : SnippetPropertyForm(pTree->GetParent(), -1, wxT("Snippet Properties"))
{
    InitSnippetProperty(pTree, itemId, pWaitSem);
}
// ----------------------------------------------------------------------------
void SnippetProperty::InitSnippetProperty(wxTreeCtrl * pTree, wxTreeItemId itemId, wxSemaphore * pWaitSem)
// ----------------------------------------------------------------------------
{
    // ctor initialization
    // The scintilla Editor is allocated by SnippetPropertyForm
    m_pWaitingSemaphore = pWaitSem;
    m_nScrollWidthMax = 0;
    // Move dialog into midpoint of parent window
    wxPoint mousePosn = ::wxGetMousePosition();
    this->Move(mousePosn.x, mousePosn.y);
    this->SetSize(mousePosn.x, mousePosn.y, 460, 260);
    //SetSize(460, 260);
    GetConfig()->CenterChildOnParent(this);
    m_pTreeCtrl = pTree;
    m_TreeItemId = itemId;
    // Initialize the properties fields
    m_ItemLabelTextCtrl->SetValue(pTree->GetItemText(itemId));
    m_ItemLabelTextCtrl->Connect(wxEVT_COMMAND_TEXT_ENTER,
                                 wxCommandEventHandler(SnippetProperty::OnOk), NULL, this);
    m_SnippetEditCtrl->SetText(wxT("Enter text or Filename"));
    m_SnippetEditCtrl->SetFocus();
    wxColour txtBackground = m_ItemLabelTextCtrl->GetBackgroundColour();
    //m_SnippetEditCtrl->SetBackgroundColour(txtBackground);
    m_SnippetEditCtrl->StyleSetBackground(wxSCI_STYLE_DEFAULT, txtBackground);
    m_SnippetEditCtrl->StyleClearAll();

    // Get the item
    if ((m_pSnippetDataItem = (SnippetTreeItemData *)(pTree->GetItemData(itemId))))
    {
        // Check that we're using the correct item type
        if (m_pSnippetDataItem->GetType() != SnippetTreeItemData::TYPE_SNIPPET)
        {
            // This shouldn't happen since the context menu only shows
            // properties on TYPE_SNIPPET
            return;
        }

        wxString snippetText = m_pSnippetDataItem->GetSnippetString() ;

        if (not snippetText.IsEmpty())
        {
            GetSnippetEditCtrl()-> SetText(snippetText);
            GetSnippetEditCtrl()->SetSavePoint();
            // SetText() marked the file as modified
            // Unmarked it by saving to a dummy file
            //#if defined(__WXMSW__)
            //    m_SnippetEditCtrl->SaveFile(wxT("nul"));
            //#else
            //    m_SnippetEditCtrl->SaveFile(wxT("/dev/null"));
            //#endif
            // reset the undo history to avoid undoing to a blank page
            m_SnippetEditCtrl->EmptyUndoBuffer();
        }//if

        // reset horiz scroll max width
        //-m_nScrollWidthMax = GetSnippetEditCtrl()->GetLongestLinePixelWidth();
        //-GetSnippetEditCtrl()->SetScrollWidth(m_nScrollWidthMax);
    }//if

    SetDropTarget(new SnippetDropTarget(this));
}//SnippetProperty ctor
// ----------------------------------------------------------------------------
SnippetProperty::~SnippetProperty()
// ----------------------------------------------------------------------------
{
    //dtor
}
// ----------------------------------------------------------------------------
// edit events
// ----------------------------------------------------------------------------
//  Dialog events
// ----------------------------------------------------------------------------
void SnippetProperty::OnOk(wxCommandEvent & event)
// ----------------------------------------------------------------------------
{
    wxUnusedVar(event);
    LOGIT(_T("SnippetProperty::OnOK"));
    // set data to edited snippet
    m_pSnippetDataItem->SetSnippetString(m_SnippetEditCtrl->GetText());
    // label may have been edited
    m_pTreeCtrl->SetItemText(m_TreeItemId, m_ItemLabelTextCtrl->GetValue());

    if (m_pWaitingSemaphore)
    {
        m_pWaitingSemaphore->Post();
    }

    m_retCode = wxID_OK;
}
// ----------------------------------------------------------------------------
void SnippetProperty::OnCancel(wxCommandEvent & event)
// ----------------------------------------------------------------------------
{
    wxUnusedVar(event);
    LOGIT(_T("SnippetProperty::OnCancel"));

    if (m_pWaitingSemaphore)
    {
        m_pWaitingSemaphore->Post();
    }

    m_retCode = wxID_CANCEL;
}
// ----------------------------------------------------------------------------
void SnippetProperty::OnSnippetButton(wxCommandEvent & event)
// ----------------------------------------------------------------------------
{
    // Snippet button clicked from OpenAsFile context menu
    wxUnusedVar(event);

    if (GetActiveMenuId() == idMnuConvertToFileLink)
    {
        // let user choose a file to hold snippet
        wxString ChosenFileName = wxFileSelector(wxT("Choose a Link target"));

        if (not ChosenFileName.IsEmpty())
        {
            m_SnippetEditCtrl-> SetText(ChosenFileName);
        }

        return;
    }

    // Snippet button clicked from menubar edit(Menu) or properties context(Mnu)
    if ((g_activeMenuId == idMnuProperties)    //note: mnu vs menu
            //-#if defined(__WXMSW__) && !defined(BUILDING_PLUGIN)
            //-#if !defined(BUILDING_PLUGIN)
            //-    || ( g_activeMenuId == idMenuProperties)
            //-#endif
       )
    {
        if (GetConfig()->SettingsExternalEditor.IsEmpty())
        {
            wxMessageBox(wxT("Use Menu/Settings/Options to specify an external editor."));
            return;
        }

        // let user edit the snippet text
        // write text to temp file
        // invoke the external editor
        // read text back into snippet
        if (IsSnippetFile())
        {
            InvokeEditOnSnippetFile();
        }
        else
        {
            InvokeEditOnSnippetText();
        }
    }//fi
}
// ----------------------------------------------------------------------------
void SnippetProperty::OnFileSelectButton(wxCommandEvent & event)
// ----------------------------------------------------------------------------
{
    // Properties File Select button clicked
    wxUnusedVar(event);
    // let choose a file name to insert into snippet property
    wxString ChosenFileName = wxFileSelector(wxT("Choose a file"));

    if (not ChosenFileName.IsEmpty())
    {
        m_SnippetEditCtrl-> SetText(ChosenFileName);
    }
}//OnFileSelectButton
// ----------------------------------------------------------------------------
void SnippetProperty::InvokeEditOnSnippetText()
// ----------------------------------------------------------------------------
{
    //Exec Edit Snippet Text
    wxFileName tmpFileName = wxFileName::CreateTempFileName(wxEmptyString);
#ifdef LOGGING
    LOGIT(_T("fakename:[%s]"), tmpFileName.GetFullPath().GetData());
#endif //LOGGING
    wxFile tmpFile(tmpFileName.GetFullPath(), wxFile::write);

    if (not tmpFile.IsOpened())
    {
        // Let user know that attempt to edit file failed
        wxMessageBox(wxT("Open failed for:") + tmpFileName.GetFullPath());
        return ;
    }

    wxString snippetData(GetSnippetString());
    tmpFile.Write(csU2C(snippetData), snippetData.Length());
    tmpFile.Close();
    // Invoke the external editor on the temporary file
    // file name must be surrounded with quotes when using wxExecute
    wxString externalEditor = GetConfig()->SettingsExternalEditor;

    if (externalEditor == _T("Enter filename of external editor"))
    {
        wxMessageBox(wxT("No external editor specified.\n Check settings.\n"));
        return;
    }

    //wxString execString = GetConfig()->SettingsExternalEditor + wxT(" \"") + tmpFileName.GetFullPath() + wxT("\"");
    wxString execString = GetConfig()->SettingsExternalEditor + wxT(" \"") + tmpFileName.GetFullPath() + wxT("\"");
#ifdef LOGGING
    LOGIT(_T("Properties wxExecute[%s]"), execString.GetData());
#endif //LOGGING
    // Invoke the external editor and wait for its termination
    ::wxExecute(execString, wxEXEC_SYNC);
    // Read the edited data back into the snippet text
    tmpFile.Open(tmpFileName.GetFullPath(), wxFile::read);

    if (not tmpFile.IsOpened())
    {
        wxMessageBox(wxT("Abort. Error reading temp data file."));
        return;
    }

    unsigned long fileSize = tmpFile.Length();
#ifdef LOGGING
    LOGIT(_T("New file length[%d]"), fileSize);
#endif //LOGGING
    // check the data for validity
    char pBuf[fileSize + 1];
    size_t nResult = tmpFile.Read(pBuf, fileSize);

    if (wxInvalidOffset == (int)nResult)
    {
        wxMessageBox(wxT("InvokeEditOnSnippetText()\nError reading temp file"));
    }

    pBuf[fileSize] = 0;
    tmpFile.Close();
#ifdef LOGGING
    //LOGIT( _T("pBuf[%s]"), pBuf );
#endif //LOGGING
    // convert data back to internal format
    snippetData = csC2U(pBuf);
#ifdef LOGGING
    //LOGIT( _T("snippetData[%s]"), snippetData.GetData() );
#endif //LOGGING
    // delete the temporary file
    ::wxRemoveFile(tmpFileName.GetFullPath());
    // update Tree item
    m_SnippetEditCtrl-> SetText(snippetData);
}
// ----------------------------------------------------------------------------
void SnippetProperty::InvokeEditOnSnippetFile()
// ----------------------------------------------------------------------------
{
    // Open as file
    if (not IsSnippetFile())
    {
        return;
    }

    // If snippet is file, open it
    wxString FileName = GetConfig()->GetSnippetsTreeCtrl()->GetSnippetFileLink();
    // we have an actual file name, not just text
    wxString pgmName = GetConfig()->SettingsExternalEditor;

    if (pgmName.IsEmpty())
    {
#if defined(__WXMSW__)
        pgmName = wxT("notepad");
#elif defined(__UNIX__)
        pgmName = wxT("gedit");
#endif
    }

    // file name must be surrounded with quotes when using wxExecute
    wxString execString = pgmName + wxT(" \"") + FileName + wxT("\"");
#ifdef LOGGING
    LOGIT(_T("InvokeEditOnSnippetFile[%s]"), execString.GetData());
#endif //LOGGING
    ::wxExecute(execString);
}//InvokeEditOnSnippetFile
