/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "xtra_res.h"
    #include "logmanager.h"
    #include "scrollingdialog.h"
    #include <wx/wx.h>
#endif

#include <wx/xml/xml.h>

/////////////////////////////////////////////////////////////////////////////
// Name:        xh_toolb.cpp
// Purpose:     XRC resource for wxBoxSizer
// Author:      Vaclav Slavik
// Created:     2000/08/11
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
// Modified by Ricardo Garcia for Code::Blocks
// Comment: Things would've been much easier if field m_isInside had been
//          protected instead of private! >:(
/////////////////////////////////////////////////////////////////////////////

wxToolBarAddOnXmlHandler::wxToolBarAddOnXmlHandler() :
    m_isInside(FALSE), m_isAddon(false), m_toolbar(nullptr), m_ImageSize(0)
{
    XRC_ADD_STYLE(wxTB_FLAT);
    XRC_ADD_STYLE(wxTB_DOCKABLE);
    XRC_ADD_STYLE(wxTB_VERTICAL);
    XRC_ADD_STYLE(wxTB_HORIZONTAL);
    XRC_ADD_STYLE(wxTB_TEXT);
    XRC_ADD_STYLE(wxTB_NOICONS);
    XRC_ADD_STYLE(wxTB_NODIVIDER);
    XRC_ADD_STYLE(wxTB_NOALIGN);
}

void wxToolBarAddOnXmlHandler::SetToolbarImageSize(int size)
{
    m_ImageSize = size;
    m_PathReplaceString = wxString::Format("%dx%d", size, size);
}

void wxToolBarAddOnXmlHandler::SetCurrentResourceID(const wxString & id)
{
    m_CurrentID = id;
}

// if 'param' has stock_id/stock_client, extracts them and returns true
// Taken from wxWidgets.
static bool GetStockArtAttrs(wxString & art_id, wxString & art_client, const wxXmlNode * paramNode,
                             const wxString & defaultArtClient)
{
    if (paramNode)
    {
        art_id = paramNode->GetAttribute(wxT("stock_id"), wxString());

        if (!art_id.empty())
        {
            art_id = wxART_MAKE_ART_ID_FROM_STR(art_id);
            art_client = paramNode->GetAttribute(wxT("stock_client"), wxString());

            if (art_client.empty())
            {
                art_client = defaultArtClient;
            }
            else
            {
                art_client = wxART_MAKE_CLIENT_ID_FROM_STR(art_client);
            }

            return true;
        }
    }

    return false;
}


wxBitmap wxToolBarAddOnXmlHandler::GetCenteredBitmap(const wxString & param, wxSize size,
                                                     double scaleFactor)
{
    wxXmlNode * paramNode = GetParamNode(param);

    // If there is no such tag as requested it is best to return null bitmap, so default processing
    // could have chance to work.
    if (!paramNode)
    {
        return wxNullBitmap;
    }

    wxString artId, artClient;

    if (GetStockArtAttrs(artId, artClient, paramNode, wxART_TOOLBAR))
    {
        wxBitmap stockArt = wxArtProvider::GetBitmap(artId, artClient, size * scaleFactor);

        if (stockArt.IsOk())
        {
            return stockArt;
        }
    }

    const wxString name(GetParamValue(paramNode));

    if (name.empty())
    {
        return wxArtProvider::GetBitmap("sdk/missing_icon", wxART_TOOLBAR, size * scaleFactor);
    }

    wxString finalName(name);
    wxBitmap bitmap;
#if wxCHECK_VERSION(3, 1, 6)

    if (finalName.Replace("22x22", "svg"))
    {
        finalName.Replace(".png", ".svg");
        bitmap = cbLoadBitmapBundleFromSVG(finalName, wxSize(m_ImageSize, m_ImageSize), &GetCurFileSystem()).GetBitmap(wxDefaultSize);

        // Fallback
        if (!bitmap.Ok() && name.EndsWith(".png"))
        {
            finalName = name;

            if (finalName.Replace("22x22", m_PathReplaceString))
            {
                bitmap = cbLoadBitmap(finalName, wxBITMAP_TYPE_PNG, &GetCurFileSystem());
            }
        }
    }
    else
    {
        bitmap = cbLoadBitmap(finalName, wxBITMAP_TYPE_PNG, &GetCurFileSystem());
    }

#else
    finalName.Replace("22x22", m_PathReplaceString);

    if (finalName.Contains(".svg"))
    {
        // Just in case something was coded up incorrectly
        finalName.Replace(".svg", ".png");
    }

    bitmap = cbLoadBitmap(finalName, wxBITMAP_TYPE_PNG, &GetCurFileSystem());
#endif

    if (!bitmap.Ok())
    {
        LogManager * logger = Manager::Get()->GetLogManager();
        logger->LogError(wxString::Format("(%s) Failed to load image: '%s'", m_CurrentID, finalName));
        return wxArtProvider::GetBitmap("sdk/missing_icon", wxART_TOOLBAR, size * scaleFactor);
    }

    int bw = bitmap.GetWidth();
    int bh = bitmap.GetHeight();

    if (size * scaleFactor == wxSize(bw, bh))
    {
        return bitmap;
    }

    LogManager * logger = Manager::Get()->GetLogManager();
    const wxString msg = wxString::Format("(%s): Image \"%s\" with size [%dx%d] doesn't match "
                                          "requested size [%dx%d] resizing (scale factor %.3f)!",
                                          m_CurrentID, finalName, bw, bh,
                                          wxRound(size.x * scaleFactor), wxRound(size.y * scaleFactor),
                                          scaleFactor);
    logger->LogWarning(msg);
    wxImage image = bitmap.ConvertToImage();
    int w = size.GetWidth();
    int h = size.GetHeight();
    int x = (w - bw) / 2;
    int y = (h - bh) / 2;

    // If the image is bigger than the current size our code for resizing would do overflow the
    // buffers. Until the code is made to handle such cases just rescale the image.
    if (size.x < bw || size.y < bh)
    {
        image.Rescale(size.x, size.y, wxIMAGE_QUALITY_HIGH);
    }
    else
        if (image.HasAlpha()) // Resize doesn't handle Alpha... :-(
        {
            const unsigned char * data = image.GetData();
            const unsigned char * alpha = image.GetAlpha();
            unsigned char * rgb = (unsigned char *) calloc(w * h, 3);
            unsigned char * a = (unsigned char *) calloc(w * h, 1);

            // copy Data/Alpha from smaller bitmap to larger bitmap
            for (int row = 0; row < bh; row++)
            {
                memcpy(rgb + ((row + y) * w + x) * 3, data + (row * bw) * 3, bw * 3);
                memcpy(a + ((row + y) * w + x), alpha + (row * bw), bw);
            }

            image = wxImage(w, h, rgb, a);
        }
        else
        {
            image.Resize(size, wxPoint(x, y));
        }

    return wxBitmap(image);
}

wxObject * wxToolBarAddOnXmlHandler::DoCreateResource()
{
    wxToolBar * toolbar = nullptr;

    if (m_class == _T("tool"))
    {
        wxCHECK_MSG(m_toolbar, nullptr, _("Incorrect syntax of XRC resource: tool not within a toolbar!"));
        const wxSize bitmapSize = m_toolbar->GetToolBitmapSize();
#ifdef __WXMSW__
        const double scaleFactor = 1.0;
#else
        const double scaleFactor = cbGetContentScaleFactor(*m_toolbar);
#endif // __WXMSW__

        if (GetPosition() != wxDefaultPosition)
        {
            m_toolbar->AddTool(GetID(),
                               wxEmptyString,
                               GetCenteredBitmap(_T("bitmap"), bitmapSize, scaleFactor),
                               GetCenteredBitmap(_T("bitmap2"), bitmapSize, scaleFactor),
                               wxITEM_NORMAL,
                               GetText(_T("tooltip")),
                               GetText(_T("longhelp")));
        }
        else
        {
            wxItemKind kind = wxITEM_NORMAL;

            if (GetBool(_T("radio")))
            {
                kind = wxITEM_RADIO;
            }

            if (GetBool(_T("toggle")))
            {
                wxASSERT_MSG(kind == wxITEM_NORMAL,
                             _("can't have both toggleable and radion button at once"));
                kind = wxITEM_CHECK;
            }

            m_toolbar->AddTool(GetID(),
                               GetText(_T("label")),
                               GetCenteredBitmap(_T("bitmap"), bitmapSize, scaleFactor),
                               GetCenteredBitmap(_T("bitmap2"), bitmapSize, scaleFactor),
                               kind,
                               GetText(_T("tooltip")),
                               GetText(_T("longhelp")));
        }

        if (GetBool(_T("disabled")))
        {
            m_toolbar->EnableTool(GetID(), false);
        }

        return m_toolbar; // must return non-nullptr
    }
    else
        if (m_class == _T("separator"))
        {
            wxCHECK_MSG(m_toolbar, nullptr, _("Incorrect syntax of XRC resource: separator not within a toolbar!"));
            m_toolbar->AddSeparator();
            return m_toolbar; // must return non-nullptr
        }
        else /*<object class="wxToolBar">*/
        {
            m_isAddon = (m_class == _T("wxToolBarAddOn"));

            if (m_isAddon)
            {
                // special case: Only add items to toolbar
                toolbar = (wxToolBar *)m_instance;
                // XRC_MAKE_INSTANCE(toolbar, wxToolBar);
            }
            else
            {
                int style = GetStyle(_T("style"), wxNO_BORDER | wxTB_HORIZONTAL);
#ifdef __WXMSW__

                if (!(style & wxNO_BORDER))
                {
                    style |= wxNO_BORDER;
                }

#endif

                // XRC_MAKE_INSTANCE(toolbar, wxToolBar)
                if (m_instance)
                {
                    toolbar = wxStaticCast(m_instance, wxToolBar);
                }

                if (!toolbar)
                {
                    toolbar = new wxToolBar;
                }

                toolbar->Create(m_parentAsWindow,
                                GetID(),
                                GetPosition(),
                                GetSize(),
                                style,
                                GetName());
                wxSize margins = GetSize(_T("margins"));

                if (!(margins == wxDefaultSize))
                {
                    toolbar->SetMargins(margins.x, margins.y);
                }

                long packing = GetLong(_T("packing"), -1);

                if (packing != -1)
                {
                    toolbar->SetToolPacking(packing);
                }

                long separation = GetLong(_T("separation"), -1);

                if (separation != -1)
                {
                    toolbar->SetToolSeparation(separation);
                }
            }

            wxXmlNode * children_node = GetParamNode(_T("object"));

            if (!children_node)
            {
                children_node = GetParamNode(_T("object_ref"));
            }

            if (children_node == nullptr)
            {
                return toolbar;
            }

            m_isInside = TRUE;
            m_toolbar = toolbar;
            wxXmlNode * n = children_node;

            while (n)
            {
                if ((n->GetType() == wxXML_ELEMENT_NODE) &&
                        (n->GetName() == _T("object") || n->GetName() == _T("object_ref")))
                {
                    wxObject * created = CreateResFromNode(n, toolbar, nullptr);
                    wxControl * control = wxDynamicCast(created, wxControl);

                    if (!IsOfClass(n, _T("tool")) &&
                            !IsOfClass(n, _T("separator")) &&
                            control != nullptr &&
                            control != toolbar)
                    {
                        //Manager::Get()->GetLogManager()->DebugLog(F(_T("control=%p, parent=%p, toolbar=%p"), control, control->GetParent(), toolbar));
                        toolbar->AddControl(control);
                    }
                }

                n = n->GetNext();
            }

            toolbar->Realize();
            m_isInside = FALSE;
            m_toolbar = nullptr;

            if (!m_isAddon)
            {
                if (m_parentAsWindow && !GetBool(_T("dontattachtoframe")))
                {
                    wxFrame * parentFrame = wxDynamicCast(m_parent, wxFrame);

                    if (parentFrame)
                    {
                        parentFrame->SetToolBar(toolbar);
                    }
                }
            }

            m_isAddon = false;
            return toolbar;
        }
}

bool wxToolBarAddOnXmlHandler::CanHandle(wxXmlNode * node)
{
    // NOTE (mandrav#1#): wxXmlResourceHandler::IsOfClass() doesn't work in unicode (2.6.2)
    // Don't ask why. It does this and doesn't work for our custom handler:
    //    return node->GetPropVal(wxT("class"), wxEmptyString) == classname;
    //
    // This works though:
    //    return node->GetPropVal(wxT("class"), wxEmptyString).Matches(classname);
    //
    // Don't ask me why... >:-|
    bool istbar = node->GetAttribute(wxT("class"), wxEmptyString).Matches(_T("wxToolBarAddOn"));
    bool istool = node->GetAttribute(wxT("class"), wxEmptyString).Matches(_T("tool"));
    bool issep = node->GetAttribute(wxT("class"), wxEmptyString).Matches(_T("separator"));
    return ((!m_isInside && istbar) ||
            (m_isInside && istool) ||
            (m_isInside && issep));
}



IMPLEMENT_DYNAMIC_CLASS(wxScrollingDialogXmlHandler, wxDialogXmlHandler)

wxScrollingDialogXmlHandler::wxScrollingDialogXmlHandler() : wxDialogXmlHandler()
{
}

wxObject * wxScrollingDialogXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(dlg, wxScrollingDialog);
    dlg->Create(m_parentAsWindow,
                GetID(),
                GetText(wxT("title")),
                wxDefaultPosition, wxDefaultSize,
                GetStyle(wxT("style"), wxDEFAULT_DIALOG_STYLE),
                GetName());

    if (HasParam(wxT("size")))
    {
        dlg->SetClientSize(GetSize(wxT("size"), dlg));
    }

    if (HasParam(wxT("pos")))
    {
        dlg->Move(GetPosition());
    }

    if (HasParam(wxT("icon")))
    {
        dlg->SetIcon(GetIcon(wxT("icon"), wxART_FRAME_ICON));
    }

    SetupWindow(dlg);
    CreateChildren(dlg);

    if (GetBool(wxT("centered"), false))
    {
        dlg->Centre();
    }

    return dlg;
}

bool wxScrollingDialogXmlHandler::CanHandle(wxXmlNode * node)
{
    return IsOfClass(node, wxT("wxScrollingDialog"));
}
