/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "wxspropertygridmanager.h"
#include "wxspropertycontainer.h"

#include <manager.h>
#include <logmanager.h>

IMPLEMENT_CLASS(wxsPropertyGridManager, wxPropertyGridManager)

wxsPropertyGridManager::wxsPropertyGridManager(
    wxWindow * parent,
    wxWindowID id,
    const wxPoint & pos,
    const wxSize & size,
    long style,
    const char * name):
    wxPropertyGridManager(parent, id, pos, size, style, name),
    MainContainer(nullptr)
{
    Singleton = this;
    PropertiesList = nullptr;
}

wxsPropertyGridManager::~wxsPropertyGridManager()
{
    PGIDs.Clear();
    PGEntries.Clear();
    PGIndexes.Clear();
    PGContainers.Clear();
    PGContainersSet.clear();

    for (size_t i = 0; i < GetPageCount(); i++)
    {
        ClearPage(i);
    }

    PreviousIndex = -1;
    PreviousProperty = nullptr;

    if (Singleton == this)
    {
        Singleton = nullptr;
    }

    DeleteTemporaryPropertiesList();
}

void wxsPropertyGridManager::OnChange(wxPropertyGridEvent & event)
{
    wxPGId ID = event.GetProperty();

    for (size_t i = PGIDs.Count(); i-- > 0;)
    {
        if (PGIDs[i] == ID)
        {
            wxsPropertyContainer * Container = PGContainers[i];

            if (!PGEntries[i]->PGRead(Container, this, ID, PGIndexes[i]))
            {
                wxString ErrorMsg;
                ErrorMsg << "wxSmith: Couldn't read value from wxsPropertyGridManager"
                         << ", propgrid name=" << PGEntries[i]->GetPGName()
                         << ", date name="     << PGEntries[i]->GetDataName()
                         << ", type name="     << PGEntries[i]->GetTypeName();
                Manager::Get()->GetLogManager()->DebugLogError(ErrorMsg);
            }

            // Notifying about property change
            Container->NotifyPropertyChangeFromPropertyGrid();

            // Notifying about sub property change
            if (Container != MainContainer && MainContainer != nullptr)
            {
                MainContainer->OnSubPropertyChanged(Container);
            }

            Update(nullptr);
            return;
        }
    }

    // Did not found changed id, it's time to say to container
    MainContainer->OnExtraPropertyChanged(this, ID);
}

void wxsPropertyGridManager::OnDoubleClick(wxPropertyGridEvent & event)
{
    // We are only interested in the double click event from the wxPropertyGridPage of "Events" (page index 1)
    // So, we are going to filter out the events come from the wxPropertyGridPage of "Properties" (page index 0)
    // loop the PGIDs member is another kind of filtering (see wxsPropertyGridManager::OnChange()), since
    // normally the properties were stored in the page 0.
    int pageIndex = GetSelectedPage();

    if (pageIndex != 1)  //wxPropertyGridPage of "Events"
    {
        return;
    }

    // No need to call any notification of change function here, since double click of the event name
    // just jump to the function handler body of the event. We call the OnExtraPropertyChanged()
    // function to emulate an event handler change, it in-fact does not add a new event handler.
    wxPGId ID = event.GetProperty();
    MainContainer->OnExtraPropertyChanged(this, ID);
}

void wxsPropertyGridManager::Update(wxsPropertyContainer * PC)
{
    if (PC && PGContainersSet.find(PC) == PGContainersSet.end())
    {
        // This container is not used here
        return;
    }

    for (size_t i = PGIDs.Count(); i-- > 0;)
    {
        PGEntries[i]->PGWrite(PGContainers[i], this, PGIDs[i], PGIndexes[i]);
    }
}

void wxsPropertyGridManager::UnbindAll()
{
    // TODO: Remove all extra pages, leave only first one
    PGIDs.Clear();
    PGEntries.Clear();
    PGIndexes.Clear();
    PGContainers.Clear();
    PGContainersSet.clear();

    for (size_t i = 0; i < GetPageCount(); i++)
    {
        ClearPage(i);
    }

    PreviousIndex = -1;
    PreviousProperty = nullptr;
    SetNewMainContainer(nullptr);
}

void wxsPropertyGridManager::UnbindPropertyContainer(wxsPropertyContainer * PC, bool doFreeze)
{
    if (PGContainersSet.find(PC) == PGContainersSet.end())
    {
        // This container is not used here
        return;
    }

    if (PC == MainContainer)
    {
        // Main container unbinds all
        UnbindAll();
        return;
    }

    if (doFreeze)
    {
        Freeze();
    }

    for (size_t i = PGIDs.Count(); i-- > 0;)
    {
        if (PGContainers[i] == PC)
        {
            // before deleting the property, make sure all children are hidden or we can get a crash
            // we do this by recursively hiding the property and it's children
            // should fix http://forums.codeblocks.org/index.php/topic,21893.0.html
            PGIDs[i]->Hide(true);
            DeleteProperty(PGIDs[i]);
            PGIDs.RemoveAt(i);
            PGEntries.RemoveAt(i);
            PGIndexes.RemoveAt(i);
            PGContainers.RemoveAt(i);
        }
    }

    // in some cases the Thaw leads to a crash, so we have to disable Freeze-Thaw until we find
    // another workaround or are sure, that we do not need it.
    if (doFreeze)
    {
        Thaw();
    }

    // If there are no properties, we have unbinded main property container
    if (!PGIDs.Count())
    {
        for (size_t i = 0; i < GetPageCount(); i++)
        {
            ClearPage(i);
        }

        SetNewMainContainer(nullptr);
    }
}

long wxsPropertyGridManager::Register(wxsPropertyContainer * Container, wxsProperty * Property, wxPGId Id, long Index)
{
    if (!Property)
    {
        return -1;
    }

    if (Property != PreviousProperty)
    {
        PreviousIndex = -1;
    }

    if (Index < 0)
    {
        Index = ++PreviousIndex;
    }

    PGEntries.Add(Property);
    PGIDs.Add(Id);
    PGIndexes.Add(Index);
    PGContainers.Add(Container);
    PGContainersSet.insert(Container);
    return Index;
}

void wxsPropertyGridManager::NewPropertyContainerStart()
{
    UnbindAll();
    DeleteTemporaryPropertiesList();
}

void wxsPropertyGridManager::NewPropertyContainerAddProperty(wxsProperty * Property, wxsPropertyContainer * Container)
{
    TemporaryPropertiesList * NewItem = new TemporaryPropertiesList;
    NewItem->Property = Property;
    NewItem->Container = Container;
    NewItem->Priority = Property->GetPriority();
    int Priority = NewItem->Priority;
    TemporaryPropertiesList * Prev = nullptr, *Search;

    for (Search = PropertiesList; Search && Search->Property->GetPriority() >= Priority; Prev = Search, Search = Search->Next);

    NewItem->Next = Search;
    (Prev ? Prev->Next : PropertiesList) = NewItem;
}

void wxsPropertyGridManager::NewPropertyContainerFinish(wxsPropertyContainer * Container)
{
    SelectPage(0);  // SelectPage() has three overloads, is 0 an int or a pointer to wxPropertyGridPage?

    while (PropertiesList)
    {
        TemporaryPropertiesList * Next = PropertiesList->Next;
        PropertiesList->Property->PGCreate(PropertiesList->Container, this, GetGrid()->GetRoot());
        delete PropertiesList;
        PropertiesList = Next;
    }

    SetNewMainContainer(Container);
}

void wxsPropertyGridManager::SetNewMainContainer(wxsPropertyContainer * Container)
{
    MainContainer = Container;
    OnContainerChanged(MainContainer);
}

void wxsPropertyGridManager::DeleteTemporaryPropertiesList()
{
    while (PropertiesList)
    {
        TemporaryPropertiesList * Next = PropertiesList->Next;
        delete PropertiesList;
        PropertiesList = Next;
    }
}

void wxsPropertyGridManager::StoreSelected(SelectionData * Data)
{
    if (!Data)
    {
        Data = &LastSelection;
    }

    Data->m_PageIndex = GetSelectedPage();
    wxPGId Selected = GetSelection();

    if (Selected)
    {
        Data->m_PropertyName = GetPropertyName(Selected);
    }
    else
    {
        Data->m_PropertyName.Clear();
    }
}

void wxsPropertyGridManager::RestoreSelected(const SelectionData * Data)
{
    if (!Data)
    {
        Data = &LastSelection;
    }

    if (Data->m_PageIndex < 0)
    {
        return;
    }

    if (Data->m_PageIndex >= (int)GetPageCount())
    {
        return;
    }

    if (Data->m_PropertyName.empty())
    {
        return;
    }

    SelectPage(Data->m_PageIndex);

    // avoid assert message with wx2.9
    if (GetPropertyByName(Data->m_PropertyName))
    {
        SelectProperty(Data->m_PropertyName);
    }
}


wxsPropertyGridManager * wxsPropertyGridManager::Singleton = nullptr;

BEGIN_EVENT_TABLE(wxsPropertyGridManager, wxPropertyGridManager)
    EVT_PG_CHANGED(-1, wxsPropertyGridManager::OnChange)
    EVT_PG_DOUBLE_CLICK(-1, wxsPropertyGridManager::OnDoubleClick)
END_EVENT_TABLE()
