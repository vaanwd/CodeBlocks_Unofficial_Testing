/////////////////////////////////////////////////////////////////////////////
// Name:        Keybinder.h
// Purpose:     Classes for binding keypresses to commands.
// Author:      Aleksandras Gluchovas
// Modified by: Francesco Montorsi (6/1/2004)
// Created:     2000/02/10
// Copyright:   (c) Aleksandras Gluchovas and (c) Francesco Montorsi
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

// Modified Keybinder for CodeBlocks KeyBnder v2.0 2019/04/8

#define NOT !

#ifndef __KEYBINDER_G__
#define __KEYBINDER_G__

// required includes
#include "wx/panel.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/treectrl.h"
#include "wx/menu.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/textctrl.h"
#include "wx/combobox.h"
#include "wx/app.h"

//-#include "debugging.h"

// The maximum number of shortcuts associated with each wxCmd.
//-#define wxCMD_MAX_SHORTCUTS                3
// Change to 2 on 2019/04/5 as one menu shortcut and one global shortcut
#define wxCMD_MAX_SHORTCUTS                2

#ifndef wxID_INVALID
    // A value used to represent an invalid id...
    #define wxID_INVALID -1
#endif
// -----------------------2006/12/29------
//  defines for wxWidgets 2.8.0
//  Allow expected PgUp/PgDn behavior under 2.8.0
//  even when 2.6 compatibility turned off
// ---------------------------------------
//#if  wxCHECK_VERSION(2, 8, 0)
//#if not WXWIN_COMPATIBILITY_2_6
//    const wxKeyCode WXK_PRIOR = WXK_PAGEUP;
//    const wxKeyCode WXK_NUMPAD_PRIOR = WXK_NUMPAD_PAGEUP;
//    const wxKeyCode WXK_NEXT = WXK_PAGEDOWN ;
//    const wxKeyCode WXK_NUMPAD_NEXT = WXK_NUMPAD_PAGEDOWN ;
//#endif //WXWIN_COMPATIBILITY_2_6
//#endif //wxCHECK_VERSION(2, 8, 0)

// defined later...
class wxConfigBase;



//! Identifies a keypress, that is a key binding.
//! This class can be used to represent key combinations like:
//!   Ctrl+F      Alt+Ctrl+Shift+K      Shift+C     .....
//! that is, combinations of [ALT, CTRL, SHIFT]+[ASCII character].
//! An array of wxKeyBind is contained in each wxCmd... see wxCmd
//! for more info.
//!
//! \note This class implements a lot of static utilities which
//!       are used by the following classes.
//!
//-v0.1- class WXDLLIMPEXP_KEYBINDER wxKeyBind
// ----------------------------------------------------------------------------
class wxKeyBind
// ----------------------------------------------------------------------------
{
    protected:

        //! One of wxACCEL_ALT, wxACCEL_CTRL, wxACCEL_SHIFT, or wxACCEL_CMD flags.
        int m_nFlags;

        //! One of the wxWidgets key code which defines the key shortcut.
        int m_nKeyCode;

    public:

        wxKeyBind()
        {
            m_nKeyCode = m_nFlags = -1;
        }

        wxKeyBind(int flags, int keycode)
        {
            Set(flags, keycode);
        }

        wxKeyBind(const wxKeyBind & tocopy)
        {
            DeepCopy(tocopy);
        }

        wxKeyBind(const wxString & key)
        {
            m_nFlags = StringToKeyModifier(key);

            //Handle Ctrl-- and Ctrl-+ keys
            if (!key.IsEmpty() && key.Last() == '-')
            {
                m_nKeyCode = '-';
            }
            else
                if (!key.IsEmpty() && key.Last() == '+')
                {
                    m_nKeyCode = '+';
                }
                else
                {
                    m_nKeyCode = StringToKeyCode(key.AfterLast('+').AfterLast('-'));
                }
        }

        virtual void DeepCopy(const wxKeyBind & p)
        {
            m_nFlags = p.m_nFlags;
            m_nKeyCode = p.m_nKeyCode;
        }

        virtual ~wxKeyBind() {}

        // Compare two wxKeyBinds       //v0.4.13
        bool operator==(const wxKeyBind & tocomp)
        {
            if (m_nFlags   != tocomp.m_nFlags)
            {
                return false;
            }

            if (m_nKeyCode != tocomp.m_nKeyCode)
            {
                return false;
            }

            return true;
        }


    public:

        //! Sets the key binding keycode and flags.
        void Set(int flags, int keycode)
        {
            m_nFlags = flags;
            m_nKeyCode = keycode;
        }

        //! Sets the key binding keycode and flags.
        //! \note This function is like #DeepCopy but uses a reference instead
        //!       of a pointer allowing such syntaxes:
        //!                mybind->Set(wxKeyBind("CTRL+ENTER"));
        void Set(const wxKeyBind & key)
        {
            m_nFlags = key.m_nFlags;
            m_nKeyCode = key.m_nKeyCode;
        }

        //! Returns TRUE if the given key event matches this key binding.
        bool MatchKey(const wxKeyEvent & key) const;

        //! Returns TRUE if the given wxKeyBind object is equivalent to this.
        bool Match(const wxKeyBind & key) const
        {
            if (m_nFlags == key.m_nFlags && m_nKeyCode == key.m_nKeyCode)
            {
                return TRUE;
            }

            return FALSE;
        }



        // Getters
        // ------------------

        int GetKeyCode() const
        {
            return m_nKeyCode;
        }

        int GetModifiers() const
        {
            return m_nFlags;
        }

        // Returns the string which describes this key combination.
        wxString GetStr() const
        {
            return KeyModifierToString(m_nFlags) +
                   KeyCodeToString(m_nKeyCode);
        }

        wxAcceleratorEntry GetAccelerator(int cmdId) const
        {
            return wxAcceleratorEntry(GetModifiers(), GetKeyCode(), cmdId);
        }


    public:        // static utilities

        static wxString NumpadKeyCodeToString(int keyCode);
        static wxString KeyCodeToString(int keyCode);
        static wxString KeyModifierToString(int keyModifier);

        static int StringToKeyCode(const wxString & keyName);
        static int StringToKeyModifier(const wxString & keyModifier);

        static int GetKeyModifier(wxKeyEvent & event);
        static wxString GetKeyStrokeString(wxKeyEvent & event);
};//wxKeyBind




//! The maximum number of wxCmd-derived classes.
#define wxCMD_MAX_TYPES                16

//! This is a string used by wxCmd::Load/Save to read/write
//! wxCmds into wxConfig objects.
#define wxCMD_CONFIG_PREFIX            wxT("bind")


//! Represents one of the commands which are handled by a wxKeyBinder.
//! This is an abstract class so it cannot be allocated directly: it
//! is used instead as base class for specific type of wxCmd.
//!
//!
//! The command has up to wxCMD_MAX_SHORTCUTS associated key shortcuts.
//! Besides, the command has three other properties: a description string,
//! a name string and an ID which should be unique for each wxKeyBinder.
//!
//-v0.1-class WXDLLIMPEXP_KEYBINDER wxCmd
// ----------------------------------------------------------------------------
class wxCmd
// ----------------------------------------------------------------------------
{
        // wxKeyBinder must be allowed to call #Exec()
        friend class wxKeyBinder;

    protected:

        //! The shortcuts.
        wxKeyBind m_keyShortcut[wxCMD_MAX_SHORTCUTS];
        int m_nShortcuts;        //!< The numner of valid entries of m_keyShortcut.

        //! The name of this command. This should be a short string.
        wxString m_strName;

        //! The description associated with this command.
        wxString m_strDescription;

        //! The ID which identifies this command.
        int m_nId;

    protected:        // static

        //! The wxCmd-derived class creation function.
        //! Such a function is required into wxCmd::Load.
        //-v0.3 typedef wxCmd *(*wxCmdCreationFnc)(int id);
        typedef wxCmd * (*wxCmdCreationFnc)(wxString cmdName, int id);

        //! A registered type of wxCmd-derived class.
        typedef struct
        {
            int type;
            wxCmdCreationFnc cmdCreateFnc;
        } wxCmdType;

        //! The array of registered command types.
        static wxCmdType m_arrCmdType[wxCMD_MAX_TYPES];

        //! The number of registered command types.
        static int m_nCmdTypes;


    public:        // static

        //! Creates a new command of the given type with the given ID.
        //! This function is used mainly in wxCmd::Load.
        static wxCmd * CreateNew(int type, int id, bool updateMnu = true);
        static wxCmd * CreateNew(wxString cmdName, int type, int id, bool updateMnu = true); //+v0.3

        //! Adds a new command type to our static list.
        static void AddCmdType(int type, wxCmdCreationFnc fnc);

        //! Returns the wxCmdType associated with the given type ID.
        //! Returns NULL if no such type of wxCmd is registered.
        static wxCmdType * FindCmdType(int type);

    public:

        wxCmd(const wxKeyBind & first, int id,
              const wxString & name = wxEmptyString,
              const wxString & desc = wxEmptyString)
        {
            m_strName = name;
            m_strDescription = desc;
            m_nShortcuts = 1;
            m_keyShortcut[0] = first;
            m_nId = id;
        }

        wxCmd(int id = wxID_INVALID,
              const wxString & name = wxEmptyString,
              const wxString & desc = wxEmptyString)
        {
            m_strName = name;
            m_strDescription = desc;
            m_nId = id;
            m_nShortcuts = 0;
        }

        virtual void DeepCopy(const wxCmd * cmd)
        {
            m_strName = cmd->m_strName;
            m_strDescription = cmd->m_strDescription;
            m_nId = cmd->m_nId;
            m_nShortcuts = cmd->m_nShortcuts;

            for (int i = 0; i < m_nShortcuts; i++)
            {
                m_keyShortcut[i].DeepCopy(cmd->m_keyShortcut[i]);
            }
        }

        virtual wxCmd * Clone() const = 0;

        // Destructor
        virtual ~wxCmd() {}

        // Compare two wxCmds       //v0.4.13
        bool operator==(const wxCmd & tocomp)
        {
            if (m_strName != tocomp.m_strName)
            {
                return false;
            }

            if (m_strDescription != tocomp.m_strDescription)
            {
                return false;
            }

            if (m_nId != tocomp.m_nId)
            {
                return false;
            }

            if (m_nShortcuts != tocomp.m_nShortcuts)
            {
                return false;
            }

            for (int i = 0; i < m_nShortcuts; i++)
                if (m_keyShortcut[i] == tocomp.m_keyShortcut[i])
                {
                    continue;
                }
                else
                {
                    return false;
                }

            return true;
        }

    public:

        // Add/Remove functions
        // ----------------------------

        //! Adds the given key binding to this command.
        void AddShortcut(const wxKeyBind & key, bool updateMnu = true)
        {
            if (m_nShortcuts >= wxCMD_MAX_SHORTCUTS)
            {
                return;
            }

            m_keyShortcut[m_nShortcuts++] = key;

            if (updateMnu)
            {
                Update();
            }
        }

        //! Builds and adds a key binding to this command.
        void AddShortcut(int flags, int keycode, bool updateMnu = true)
        {
            if (m_nShortcuts >= wxCMD_MAX_SHORTCUTS)
            {
                return;
            }

            wxKeyBind key(flags, keycode);
            AddShortcut(key, updateMnu);
            // update is called by the previous call
        }

        //! Builds and adds a key binding to this command using the
        //! given key description.
        void AddShortcut(const wxString & key, bool updateMnu = true)
        {
            if (m_nShortcuts >= wxCMD_MAX_SHORTCUTS)
            {
                return;
            }

            if (key.IsEmpty())
            {
                return;
            }

            //-v0.1- m_keyShortcut[m_nShortcuts++] = wxKeyBind(key);
            m_keyShortcut[m_nShortcuts++] = wxKeyBind(key);

            //m_keyShortcut[m_nShortcuts++] = key;
            if (updateMnu)
            {
                Update();
            }
        }

        //! Removes the n-th key shortcut associated to this command.
        void RemoveShortcut(int n, bool updateMnu = true)
        {
            //-for (int i=n; i < m_nShortcuts; i++)    // shift array left
            //pecan 2006/03/25 fix next stmt, avoid array overflow
            for (int i = n; i < (m_nShortcuts - 1); i++) // shift array left //pecan 2006/03/25
            {
                m_keyShortcut[i] = m_keyShortcut[i + 1];
            }

            m_nShortcuts--;

            if (updateMnu)
            {
                Update();
            }
        }

        //! Removes all the shortcuts associates to this command.
        void RemoveAllShortcuts(bool updateMnu = true)
        {
            m_nShortcuts = 0;

            if (updateMnu)
            {
                Update();
            }
        }



        // Miscellaneous
        // ---------------------

        //! Returns TRUE if at least one of the key bindings matches
        //! the given key event.
        bool MatchKey(const wxKeyEvent & ev) const
        {
            for (int i = 0; i < m_nShortcuts; i++)
                if (m_keyShortcut[i].MatchKey(ev))
                {
                    return TRUE;
                }

            return FALSE;
        }

        //! Returns TRUE if this command is bind to a wxKeyBind object
        //! identic to the given one.
        //! If "n" is not NULL, it is set to the index of the entry
        //! which contains the given shortcut key (if there is such
        //! a shortcut registered in this class); otherwise it is
        //! left untouched.
        bool IsBindTo(const wxKeyBind & key, int * n = NULL) const
        {
            for (int i = 0; i < m_nShortcuts; i++)
            {
                if (m_keyShortcut[i].Match(key))
                {
                    if (n)
                    {
                        *n = i;
                    }

                    return TRUE;
                }
            }

            return FALSE;
        }

        //! Saves the command into the given wxConfig object
        //! in the given key entry.
        //! \param bCleanOld If TRUE, this function will erase the given key
        //!        before writing into it.
        bool Save(wxConfigBase * p, const wxString & key = wxEmptyString, bool bCleanOld = FALSE) const;

        //! Loads from the given wxConfig's key a command.
        //! Returns FALSE if the given key does not contain info about
        //! this command's type.
        bool Load(wxConfigBase * p, const wxString & key = wxEmptyString);
        bool LoadFromString(const wxString & cfgCmdString);

        // Getters
        // ---------------------

        wxKeyBind * GetShortcut(int n)
        {
            return &m_keyShortcut[n];
        }
        const wxKeyBind * GetShortcut(int n)    const
        {
            return &m_keyShortcut[n];
        }

        wxAcceleratorEntry GetAccelerator(int n) const
        {
            return GetShortcut(n)->GetAccelerator(m_nId);
        }

        int GetId() const
        {
            return m_nId;
        }

        int GetShortcutCount() const
        {
            return m_nShortcuts;
        }

        wxString GetDescription() const
        {
            return m_strDescription;
        }

        wxString GetName() const
        {
            return m_strName;
        }

        wxArrayString GetShortcutsList() const
        {
            wxArrayString arr;

            for (int i = 0; i < m_nShortcuts; i++)
            {
                arr.Add(m_keyShortcut[i].GetStr());
            }

            return arr;
        }

        //! Returns the type ID for this wxCmd-derived class.
        virtual int GetType() const = 0;


    protected:

        //! Called after an Add/Remove function is called.
        //! Default implementation does nothing.
        virtual void Update(wxMenuItem * /*pMnuItem*/ = 0) {}

        //! Executes the command.
        //! - obj ("origin") is the object which generated the event that
        //!   was recognized as a shortcut key associated to this class.
        //! - "client" is the event handler which should receive the
        //!   event associated with this command.
        virtual void Exec(wxObject * obj, wxEvtHandler * client) = 0;
};//wxCmd




//! Defines a wxObjArray-like array of wxCmd.
//! However, we cannot use the WX_DECLARE_OBJARRAY macro
//! because wxCmd is an abstract class and thus we need
//! to keep simple pointers stored, not the objects themselves.
//-v0.1-class WXDLLIMPEXP_KEYBINDER wxCmdArray
// ----------------------------------------------------------------------------
class wxCmdArray
// ----------------------------------------------------------------------------
{
        wxArrayPtrVoid m_arr;

    public:
        wxCmdArray() {}
        wxCmdArray(const wxCmdArray & tocopy)
        {
            DeepCopy(tocopy);
        }
        virtual ~wxCmdArray()
        {
            Clear();
        }

        void DeepCopy(const wxCmdArray & arr)
        {
            Clear();

            for (int i = 0; i < arr.GetCount(); i++)
            {
                Add(arr.Item(i)->Clone());
            }
        }

        wxCmdArray & operator=(const wxCmdArray & tocopy)
        {
            DeepCopy(tocopy);
            return *this;
        }
        bool operator==(const wxCmdArray & tocomp)  //v0.4.13
        {
            if ((not m_arr.GetCount()) || (not tocomp.m_arr.GetCount()))
            {
                return false;
            }

            if (m_arr.GetCount() != tocomp.m_arr.GetCount())
            {
                return false;
            }

            for (size_t i = 0; i < m_arr.GetCount(); i++)
                if (*((wxCmd *)m_arr.Item(i)) == *((wxCmd *)tocomp.m_arr.Item(i)))
                {
                    continue;
                }
                else
                {
                    return false;
                }

            return true;
        }

        void Add(wxCmd * p)
        {
            m_arr.Add(p);
        }
        void Remove(int n);
        void Clear();

        int GetCount() const
        {
            return m_arr.GetCount();
        }
        wxCmd * Item(int n) const
        {
            return (wxCmd *)m_arr.Item(n);
        }

};//wxCmdArray


class wxKeyBinder;


// Define the wxADD_KEYBINDER_SUPPORT: you should use it inside a protected
// block of the windows which want to use the wxKeyBinder facilities
#ifdef __WXMSW__

// This is a virtual function used in wxMSW which can be used to allow
// or disable the message preprocessing and thus the preprocessing of
// keyshortcuts; to add keybinder support, the preprocessing must be
// disabled: wxKeyBinder will care of checking the keypresses for
// eventual hotkeys...
#define wxADD_KEYBINDER_SUPPORT()                                        \
    virtual bool MSWShouldPreProcessMessage(WXMSG*) { return FALSE; }
#else

// the other ports doesn't use MSWShouldPreProcessMessage...
#define wxADD_KEYBINDER_SUPPORT()            /* expand to nothing */

#endif


//! This class contains an array of wxCmd and exposes some utilities
//! to add/remove/edit them and their wxKeyBind.
//!
// ----------------------------------------------------------------------------
class wxKeyBinder : public wxObject
// ----------------------------------------------------------------------------
{
    public:

    protected:

        //! The array of wxCmd-derived classes.
        wxCmdArray m_arrCmd;

    protected:


        //! Returns the index of the first command with the given ID.
        int FindCmd(int id) const
        {
            for (int i = 0; i < (int)m_arrCmd.GetCount(); i++)
                if (id == m_arrCmd.Item(i)->GetId())
                {
                    return i;
                }

            return -1;
        }

        //! Returns the index of the first command that contains the
        //! given key binding.
        int FindCmdBindTo(const wxKeyBind & key, int * n = NULL) const
        {
            for (int i = 0; i < (int)m_arrCmd.GetCount(); i++)
                if (m_arrCmd.Item(i)->IsBindTo(key, n))
                {
                    return i;
                }

            return -1;
        }

        //! Returns the index of the first command which matches the
        //! given key event.
        int FindMatchingCmd(const wxKeyEvent & key) const
        {
            for (int i = 0; i < (int)m_arrCmd.GetCount(); i++)
                if (m_arrCmd.Item(i)->MatchKey(key))
                {
                    return i;
                }

            return -1;
        }
        // Returns index to wxCmd whos name matches the text
        int FindMatchingName(wxString menuItemLabel)
        {
            wxCmdArray * pCmdArray = GetArray();

            //LOGIT( _T("menuItemLabel[%s]"),menuItemLabel.c_str() );
            for (int i = 0; i < GetCmdCount(); i++)
            {
                wxCmd * pCmd = pCmdArray->Item(i);

                //LOGIT( _T("pCmd->Getname()[%s]"),pCmd->GetName().c_str() );
                if (pCmd->GetName() == menuItemLabel)
                {
                    return i;
                }
            }

            return -1;
        }

    public:

        wxKeyBinder() {}
        wxKeyBinder(const wxKeyBinder & tocopy) : wxObject(tocopy)
        {
            DeepCopy(tocopy);
        }

        virtual ~wxKeyBinder() {}

    public:        // miscellaneous

        //! Deep copies the given object.
        void DeepCopy(const wxKeyBinder & p)
        {
            m_arrCmd.DeepCopy(p.m_arrCmd);
            // NEVER COPY THE ARRAY OF THE ATTACHED WINDOWs:
            // WE ARE NOT ATTACHED TO THE WINDOWS OF THE GIVEN BINDER !!
            // m_arrAttachedWnd = p->m_arrAttachedWnd;
        }

        wxKeyBinder & operator=(const wxKeyBinder & tocopy)
        {
            DeepCopy(tocopy);
            return *this;
        }
        //v0.4.13 compare two wxCmd arrays        //v0.4.13
        bool operator==(const wxKeyBinder & tocomp)
        {
            for (int i = 0; i < (int)m_arrCmd.GetCount(); i++)
                if (*m_arrCmd.Item(i) == *tocomp.m_arrCmd.Item(i))
                {
                    continue;
                }
                else
                {
                    return false;
                }

            return true;
        }

        //! Resets everything associated with this class.
        void Reset()
        {
            m_arrCmd.Clear();
        }
        // Recursively update sub menu items
        void UpdateSubMenu(wxMenu * pMenu);
        //! Updates all the commands contained.
        void UpdateAllCmd(wxMenuBar * pMnuBar);

        //! Imports the wxMenuCmd created importing them from
        //! the given menu bar.
        void ImportMenuBarCmd(wxMenuBar * p);

        //! Saves the array of keybindings into the given wxConfig object.
        //! All the keybindings will be saved into subkeys of the given key.
        //! \param bCleanOld If TRUE, this function will erase the given key
        //!        before writing into it.
        bool Save(wxConfigBase * p, const wxString & key = wxEmptyString, bool bCleanOld = FALSE) const;

        //! Loads from the given wxConfig object a set of keybindings.
        bool Load(wxConfigBase * p, const wxString & key = wxEmptyString);
        bool LoadFromString(const wxString & cfgCmdString);

        //+v0.3 get name and description from loaded commands
        bool GetNameandDescription(wxConfigBase * p, const wxString & key, wxString & strName, wxString & strDescription);

        // Merge dynamically changed menu items into the wxKeyProfileArray
        int  MergeDynamicMenuItems(wxMenuBar * pMenuBar);       //v0.4.25
        int  MergeSubMenu(wxMenu * pMenu, int & modified);      //v0.4.25

        // Add functions
        // -------------------

        void AddCmd(wxCmd * p)
        {
            m_arrCmd.Add(p);
        }

        void AddShortcut(int id, const wxString & key, bool updateMnu = true)
        {
            wxCmd * p = GetCmd(id);

            if (p)
            {
                p->AddShortcut(key, updateMnu);
            }
        }

        void AddShortcut(int id, const wxKeyBind & key, bool updateMnu = true)
        {
            wxCmd * p = GetCmd(id);

            if (p)
            {
                p->AddShortcut(key, updateMnu);
            }
        }

        // Remove functions
        // -------------------

        void RemoveCmd(wxCmd * p)                               //v0.4.25
        {
            m_arrCmd.Remove(FindCmd(p->GetId()));
        }


        // Getters
        // -------------------

        int GetCmdCount() const
        {
            return m_arrCmd.GetCount();
        }

        wxCmd * GetMatchingCmd(const wxKeyEvent & key) const
        {
            int i = FindMatchingCmd(key);

            if (i != -1)
            {
                return m_arrCmd.Item(i);
            }

            return NULL;
        }

        wxCmd * GetCmd(int id) const
        {
            int i = FindCmd(id);

            if (i != -1)
            {
                return m_arrCmd.Item(i);
            }

            return NULL;
        }

        wxCmd * GetCmdBindTo(const wxString & key, int * n = NULL) const
        {
            int i = FindCmdBindTo(wxKeyBind(key), n);

            if (i != -1)
            {
                return m_arrCmd.Item(i);
            }

            return NULL;
        }

        wxKeyBind * GetShortcut(int id, int n) const
        {
            wxCmd * p = GetCmd(id);

            if (p)
            {
                return p->GetShortcut(n);
            }

            return NULL;
        }

        wxString GetShortcutStr(int id, int n) const
        {
            wxKeyBind * p = GetShortcut(id, n);

            if (p)
            {
                return p->GetStr();
            }

            return wxEmptyString;
        }

        wxArrayString GetShortcutsList(int id) const
        {
            wxCmd * p = GetCmd(id);

            if (p)
            {
                return p->GetShortcutsList();
            }

            return wxArrayString();
        }

        wxCmdArray * GetArray()
        {
            return &m_arrCmd;
        }
        const wxCmdArray * GetArray() const
        {
            return &m_arrCmd;
        }

        // returns True if window actually exists
        wxWindow * winExists(wxWindow *);
        wxWindow * FindWindowRecursively(const wxWindow * parent, const wxWindow * handle);
        void      GetMenuItemAccStr(wxMenuItem * pMenuItem, wxString & MenuItemKeyStr);


    private:
        DECLARE_CLASS(wxKeyBinder)

};//wxKeyBinder
// ----------------------------------------------------------------------------



//! A little extension of the wxKeyBinder class.
//! wxKeyProfile adds a NAME and a DESCRIPTION to the wxKeyBinder class;
//! these are useful when you want to support more than one wxKeyBinder in
//! your application to give to the user the possibility to choose among
//! different keymapping schemes...
//-v0.1-class WXDLLIMPEXP_KEYBINDER wxKeyProfile : public wxKeyBinder
// ----------------------------------------------------------------------------
class wxKeyProfile : public wxKeyBinder
// ----------------------------------------------------------------------------
{
    protected:

        //! The name of this profile.
        wxString m_strName;

        //! The description of this profile.
        wxString m_strDescription;

    public:
        wxKeyProfile(const wxString & name = wxEmptyString,
                     const wxString & desc = wxEmptyString)
            : wxKeyBinder(), m_strName(name), m_strDescription(desc) {}

        wxKeyProfile(const wxKeyProfile & tocopy) : wxKeyBinder(tocopy)
        {
            DeepCopy(tocopy);
        }

        virtual ~wxKeyProfile() {}


        void DeepCopy(const wxKeyProfile & p)
        {
            wxKeyBinder::DeepCopy(p);
            m_strName = p.m_strName;
            m_strDescription = p.m_strDescription;
        }

        wxKeyProfile & operator=(const wxKeyProfile & tocopy)
        {
            DeepCopy(tocopy);
            return *this;
        }

        //v0.4.13 compare two keyprofile array contents     //v0.4.13
        bool operator==(const wxKeyProfile & tocomp)
        {
            if (m_strName != tocomp.m_strName)
            {
                return false;
            }

            if (m_strDescription != tocomp.m_strDescription)
            {
                return false;
            }

            // compare wxKeyBinders contained within this wxKeyProfile
            if (not(m_arrCmd == tocomp.m_arrCmd))
            {
                return false;
            }

            return true;
        }//operator==

    public:        // miscellaneous

        void SetName(const wxString & name)
        {
            m_strName = name;
        }
        void SetDesc(const wxString & str)
        {
            m_strDescription = str;
        }

        wxString GetName() const
        {
            return m_strName;
        }
        wxString GetDesc() const
        {
            return m_strDescription;
        }

        bool Save(wxConfigBase * p, const wxString & key = wxEmptyString, bool bCleanOld = FALSE) const;
        bool Load(wxConfigBase * p, const wxString & key = wxEmptyString);


    private:
        DECLARE_CLASS(wxKeyProfile)
};//wxKeyProfile




//! This is a string used by wxKeyProfileArray::Load/Save to read/write
//! wxKeyProfileArrays into wxConfig objects.
#define wxKEYPROFILE_CONFIG_PREFIX            wxT("keyprof")


//! An array of wxKeyProfiles.
//! This is an handy way to hold a set of pointers to wxKeyProfiles
//! and to retrieve the profiles from a wxKeyConfigPanel after they
//! have been added/removed/edited by the user.
//!
//! This class also holds a variable (#m_nSelected) which makes it easier for
//! the owner of the array to manage more than one key profile
//! (this variables has sense only assuming that only one key profile at
//!  time is selected into app's windows...).
//! Without this additional field, you would have to declare another variable
//! to use together with wxKeyProfileArray.
//! However, please note that this variable is *not* updated in any way by
//! wxKeyProfileArray itself: this is still owner's task which can use the
//! #SetSelProfile and #GetSelProfile functions to access this variable.
//-v0.1-class WXDLLIMPEXP_KEYBINDER wxKeyProfileArray
// ----------------------------------------------------------------------------
class wxKeyProfileArray
// ----------------------------------------------------------------------------
{
        //! The array of pointers to the wxKeyProfiles.
        //! Since this array hold pointers, instances of classes derived
        //! from wxKeyProfile can be used as well with wxKeyProfileArray.
        wxArrayPtrVoid m_arr;

        //! The index of the currently selected profile.
        int m_nSelected;

    public:
        wxKeyProfileArray()
        {
            m_nSelected = -1;
        }
        wxKeyProfileArray(const wxKeyProfileArray & tocopy)
        {
            DeepCopy(tocopy);
        }
        virtual ~wxKeyProfileArray()
        {
            Cleanup();
        }


        //! \name wxArray-like functions.
        //! We cannot use wxArrayPtrVoid as base class (it does not have
        //! a virtual destructor) and so we are forced to re-implement the
        //! wxArray functions... @{

        int GetCount() const
        {
            return m_arr.GetCount();
        }
        int GetSelProfileIdx() const
        {
            return m_nSelected;
        }
        wxKeyProfile * Item(int n)
        {
            return (wxKeyProfile *)m_arr.Item(n);
        }
        wxKeyProfile * GetSelProfile()
        {
            wxASSERT(m_nSelected >= 0 && m_nSelected < GetCount());
            return Item(m_nSelected);
        }
        void Add(wxKeyProfile * p)
        {
            m_arr.Add(p);
        }
        void Clear()
        {
            m_arr.Clear();
        }
        void Remove(wxKeyProfile * p)
        {
            m_arr.Remove(p);
        }
        void SetSelProfile(int n)
        {
            wxASSERT(n < GetCount());
            m_nSelected = n;
        }
        bool IsEmpty() const
        {
            return m_arr.IsEmpty();
        }

        const wxKeyProfile * Item(int n) const
        {
            return (wxKeyProfile *)m_arr.Item(n);
        }
        const wxKeyProfile * GetSelProfile() const
        {
            return Item(m_nSelected);
        }
        void RemoveAt(size_t i, size_t count = 1)
        {
            m_arr.RemoveAt(i, count);
        }
        void Insert(wxKeyProfile * p, int n)
        {
            m_arr.Insert(p, n);
        }

        //@}


        //! Copies the given array.
        void DeepCopy(const wxKeyProfileArray & p)
        {
            Cleanup();

            for (int i = 0; i < p.GetCount(); i++)
            {
                Add(new wxKeyProfile(*p.Item(i)));
            }

            m_nSelected = p.m_nSelected;
        }

        wxKeyProfileArray & operator=(const wxKeyProfileArray & tocopy)
        {
            DeepCopy(tocopy);
            return *this;
        }
        //v0.4.13 compare two wxKeyProfileArray contents        ///v0.4.13
        bool operator==(const wxKeyProfileArray & tocomp)
        {
            if ((not GetCount()) || (not tocomp.GetCount()))
            {
                return false;
            }

            const wxKeyProfile * p1 = Item(0);
            const wxKeyProfile * p2 = tocomp.Item(0);

            // compare the wxKeyProfile name and description
            if (not((wxKeyProfile &)*p1 == (wxKeyProfile &)*p2))
            {
                return false;
            }

            return true;
        }

        //! Deletes all the objects of the array.
        //! Unlike #Clear() this function also deletes the objects and
        //! does not only detach them from this array.
        void Cleanup()
        {
            for (int i = 0; i < GetCount(); i++)
            {
                delete Item(i);
            }

            Clear();
        }

        //! Updates all the wxCmds contained.
        void UpdateAllCmd(wxMenuBar * pMnuBar)
        {
            for (int i = 0; i < GetCount(); i++)
            {
                Item(i)->UpdateAllCmd(pMnuBar);
            }
        }

        //! Stores the wxKeyProfiles into the given wxConfig object.
        //! \param bCleanOld If TRUE, this function will save all the mcStyleArrays
        //!        it contains and then will delete any other group which starts with
        //!        the mgSTYLES_CONFIG_PREFIX string.
        bool Save(wxConfigBase * p, const wxString & key = wxEmptyString, bool bCleanOld = FALSE) const;

        //! Loads from the given wxConfig object all the wxKeyProfiles found.
        //! The keyprofiles entries are detected using the wxKEYPROFILE_CONFIG_PREFIX
        //! prefix string.
        bool Load(wxConfigBase * p, const wxString & key = wxEmptyString);

        int MergeDynamicMenuItems(wxMenuBar * pMnuBar)
        {
            int changed = 0;

            for (int i = 0; i < GetCount(); i++)
            {
                changed += Item(i)->MergeDynamicMenuItems(pMnuBar);
            }

            return changed;
        }

};//wxKeyProfileArray

//! A special text control where the user keypresses are displayed
//! as the contained text.
//! This class is used by wxKeyConfigPanel class to allow the user
//! to input a shortcut key which can be associated with a wxCmd.
//!
//! Example: the user presses CTRL+SHIFT+A inside this window;
//!          the wxKeyMonitorTextCtrl displays exactly the string
//!          "Ctrl+Shift+A"
//!
//class WXDLLIMPEXP_KEYBINDER wxKeyMonitorTextCtrl : public wxTextCtrl
// ----------------------------------------------------------------------------
class wxKeyMonitorTextCtrl : public wxTextCtrl
// ----------------------------------------------------------------------------
{
    public:
        wxKeyMonitorTextCtrl(
            wxWindow * parent,
            wxWindowID id,
            const wxString & value = wxEmptyString,
            const wxPoint & pos = wxDefaultPosition,
            const wxSize & size = wxDefaultSize,
            long style = wxTE_PROCESS_ENTER,
            const wxValidator & validator = wxDefaultValidator,
            const wxString & name = wxTextCtrlNameStr) :
            wxTextCtrl(parent, id, value, pos, size, style, validator, name)
        {
            validCmdPrefixes = wxT("Ctrl-XCtrl-Alt-Shift-");
        }

        virtual ~wxKeyMonitorTextCtrl() {}

    public:

        //! Handles the keypresses and changes accordingly
        //! the text displayed in text ctrl.
        void OnKey(wxKeyEvent &);

        //! Returns TRUE if this window contains a valid key combination.
        bool IsValidKeyComb() const
        {
            //-return !GetValue().IsEmpty() && GetValue().Last() != '+';
            if (GetValue().IsEmpty())
            {
                return false;
            }

            //Handle Ctrl-- and Ctrl-+ keys
            if ((GetValue().Last() == '-') && (GetValue()[GetValue().Length() - 2] != '-'))
            {
                return false;
            }

            return true;
        }

    private:
        wxString validCmdPrefixes; //= wxT("CtrlAltShift");

        DECLARE_CLASS(wxKeyMonitorTextCtrl)
        DECLARE_EVENT_TABLE()
};

// IDs used by wxKeyConfigPanel
#define wxKEYBINDER_BASEID                  30000        // start from this value

#define wxKEYBINDER_COMMANDS_BOX_ID         wxKEYBINDER_BASEID+1
#define wxKEYBINDER_BINDINGS_BOX_ID         wxKEYBINDER_BASEID+2
#define wxKEYBINDER_KEY_FIELD_ID            wxKEYBINDER_BASEID+3
#define wxKEYBINDER_ASSIGN_KEY_ID           wxKEYBINDER_BASEID+4
#define wxKEYBINDER_REMOVE_KEY_ID           wxKEYBINDER_BASEID+5
#define wxKEYBINDER_REMOVEALL_KEY_ID        wxKEYBINDER_BASEID+6
#define wxKEYBINDER_KEYPROFILES_ID          wxKEYBINDER_BASEID+7
#define wxKEYBINDER_CATEGORIES_ID           wxKEYBINDER_BASEID+8
#define wxKEYBINDER_ADD_PROFILEBTN_ID       wxKEYBINDER_BASEID+9
#define wxKEYBINDER_REMOVE_PROFILEBTN_ID    wxKEYBINDER_BASEID+10

#define wxKEYBINDER_SELECTED_POSTFIX        wxT(" (selected)")

//! \name wxKeyConfigPanel BUILD flags
//! These flags affect the way a wxKeyConfigPanel is created.
//! See wxKeyConfigPanel::wxKeyConfigPanel. @{

//! If specified, the wxKeyConfigPanel will use a wxTreeCtrl to group
//! the keybindings.
//! \note This flag must be specified when wxKEYBINDER_USE_LISTBOX and
//!       viceversa.
#define wxKEYBINDER_USE_TREECTRL                2

//! If specified, the wxKeyConfigPanel will use a wxComboBox+wxListBox
//! to group the keybindings.
//! \note This flag must be specified when wxKEYBINDER_USE_TREECTRL and
//!       viceversa.
#define wxKEYBINDER_USE_LISTBOX                    4

//! If specified, an "Apply" and a "Cancel" button will be added in the
//! bottom part of the wxKeyConfigPanel.
#define wxKEYBINDER_SHOW_APPLYBUTTON            8

//! If specified a "Add new" and a "Remove" button will be added next to
//! key profiles combobox.
#define wxKEYBINDER_SHOW_ADDREMOVE_PROFILE        16

//! If specified, the user will be able to edit the name of the currently
//! selected profile.
#define wxKEYBINDER_ENABLE_PROFILE_EDITING        32

//! The default build flags ofr a wxKeyConfigPanel.
#define wxKEYBINDER_DEFAULT_BUILDMODE            wxKEYBINDER_SHOW_APPLYBUTTON |            \
    wxKEYBINDER_USE_TREECTRL |                \
    wxKEYBINDER_SHOW_ADDREMOVE_PROFILE |    \
    wxKEYBINDER_ENABLE_PROFILE_EDITING

//@}


//! A powerful keybinding editing panel.
//! This is a flexible panel since it can be configured through the
//! "build flags" and thus can be built in various different ways and
//! because it is conceived to be eventually used as a base class of
//! another panel which can modify its behaviour just overloading one of
//! its various functions.
//! The panel contains 4 main controls:
//!
//! - The tree control with all the commands whose shortcut keys can be
//!   changed by the given keybinder. The items in the tree control
//!   can be added through the #ImportXXXXXX() functions.
//!   If the wxKEYBINDER_USE_TREECTRL build flag is not used
//!   (and wxKEYBINDER_USE_LISTBOX must be set in this case), a
//!   combobox and a list box will be used instead of a wxTreeCtrl.
//!   The command hierarchy is still retained also using
//!   wxKEYBINDER_USE_LISTBOX even if a wxTreeCtrl looks better if
//!   you have deep-nested menu items.
//!
//! - The listbox with all the key combinations associated with the
//!   selected command
//!
//! - A wxKeyMonitorTextCtrl and the buttons which can be used by the
//!   user to remove & add new key combinations for the selected command.
//!
//! - The description field which can be used to view the help string
//!   attached to the selected command.
//!
//! If you want to use this panel you must:
//!
//! 1) create it as all other wxPanels.
//!
//! 2) use the #AddProfiles() or the #AddProfile() functions to init the
//!    panel with the original keybindings.
//!
//! 3) call one of the ImportXXXX() functions in order to create the
//!    tree control which will be used by the user to edit the key bindings.
//!    This is a step different from step #2 because wxKeyProfile does not
//!    contain hierarchical informations about the commands; also, you can
//!    fill the tree control of the panel with only some of the commands
//!    contained in the profiles you added with step #2: that way the user
//!    will be able to modify the keybindings only of the commands shown
//!    in the tree control which can thus be used as a "command filter".
//!    However, if you just want to make user-editable all the commands of
//!    a wxKeyProfile (and you don't care about hierarchical disposition),
//!    you can just use the #ImportKeyProfileCmd function.
//!
//! 4) when the user has finished to edit the key bindings, the
//!    #ApplyChanges() function is called by wxKeyConfigPanel and the
//!    temporary contents of the wxKeyBinder edited by the user are copied
//!    in the original wxKeyProfile.
//!    You can then retrieve the new array of wxKeyProfile (the user could
//!    have added/removed profiles) using #GetProfiles(). The last selected
//!    profile can be obtained using #GetSelProfile().
//!
//class /*-v0.1-WXDLLIMPEXP_KEYBINDER*/ wxKeyConfigPanel : public wxPanel

// ----------------------------------------------------------------------------
class wxKeyConfigPanel : public wxPanel
// ----------------------------------------------------------------------------
{
    public:

        //! Constructs this panel and links it with the given key binder.
        wxKeyConfigPanel(wxWindow * parent,
                         int buildMode = wxKEYBINDER_DEFAULT_BUILDMODE,
                         wxWindowID id = -1,
                         const wxPoint & pos = wxDefaultPosition,
                         const wxSize & size = wxDefaultSize,
                         long style = wxTAB_TRAVERSAL,
                         const wxString & name = wxT("wxKeyConfigPanel"));

        virtual ~wxKeyConfigPanel();
        wxButton * m_pOK; // the OK button


    public:

        // import commands (to call BEFORE ShowModal):
        // they affect the tree control only

        //! Adds to the tree of user-editable commands, the menuitems
        //! contained in the given menubar.
        //!
        //! If the root of the tree control doesn't exist (because none
        //! of the ImportXXXX functions have been called yet), this
        //! function creates it with the "rootname" label.
        virtual void ImportMenuBarCmd(wxMenuBar * menuitems,
                                      const wxString & rootname = wxT("Menu bar"));

        //! Adds to the tree of user-editable commands, the raw list of
        //! the commmands contained in the n-th key profile.
        //! Instead of a tree structure (like the one generated by
        //! #ImportMenuBarCmd), this function creates a raw list of the
        //! commands contained in the key binder since a wxKeyBinder does
        //! not contain hierarchical info about the keybindings.
        virtual void ImportKeyProfileCmd(const wxKeyProfile & toimport,
                                         const wxString & rootname = wxT("Commands"));


    public:

        // keyprofile utilities (to call BEFORE ShowModal):
        // they affect the keybindings only

        //! Adds a new keyprofile to the keyprofile combo box.
        //! \note This function does not select the given keyprofile
        //!       (except in the case it's the first keyprofile added).
        //!       To force the selection of a profile, you should use the
        //!       SetSelProfile() function.
        virtual void AddProfile(const wxKeyProfile & p);

        //! Adds all the profiles stored in the given array to the keyprofile
        //! combo box copying them into the panel for user modifications.
        //! \note The profile which is marked as selected into the given
        //!       wxKeyProfileArray is selected (see wxKeyProfileArray::m_nSelected).
        virtual void AddProfiles(const wxKeyProfileArray & arr);

        //! Forces the panel to select the n-th profile in the keyprofile combo box.
        virtual void SetSelProfile(int n);


    public:        // output-access utilities (to call AFTER ShowModal)

        //! Returns the n-th key profile of the profile combo box.
        wxKeyProfile * GetProfile(int n) const
        {
            wxASSERT(m_pKeyProfiles);
            return (wxKeyProfile *)m_pKeyProfiles->GetClientData(n);
        }

        //! Returns the currently selected key profile using #GetSelProfileIdx().
        //! This is the profile owned by the profile combo box.
        wxKeyProfile * GetSelProfile() const
        {
            int n = GetSelProfileIdx();
            return (n >= 0 ? GetProfile(n) : NULL);
        }

        //! Returns the index of the currently selected key profile.
        //! The currently selected key profile is the profile which is currently
        //! displayed by the keyprofile combobox; thus, this function needs the
        //! #m_pKeyProfiles combobox created (and initialized) to work.
        //! \note This function won't use wxComboBox::GetSelection() because
        //!       when the label of the key profile has been edited, the function
        //!       returns always -1... we will use the #m_nCurrentProf variable.
        int GetSelProfileIdx() const
        {
            wxASSERT(m_pKeyProfiles);
            return m_nCurrentProf;
        }

        //! Returns an array containing the pointers to the keyprofiles edited
        //! by this dialog. This function is typically used after the ShowModal()
        //! function has been called to retrieve the user-edited keyprofiles
        //! in case the user chose OK (and not cancel).
        //! \note The returned array has a valid profile selected which coincide
        //!       with the last keyprofile selected in the combobox; i.e. you can
        //!       use the wxKeyProfileArray::GetSelProfile function to retrieve
        //!       the profile the user has chosen to select.
        wxKeyProfileArray GetProfiles() const;


    public:        // miscellaneous

        //! Copies the internal key binder used to allow a sort of
        //! "undo" feature into the original binder set through the
        //! #SetBinder function.
        virtual void ApplyChanges();

        //! Enables the key profiles combo box and thus you should check
        //! the keyprofiles.
        //! \note The key profiles are enabled by default.
        void EnableKeyProfiles(bool bEnable = TRUE);

        //! Disables the key profiles combo box.
        //! The profile used will be the first given.
        void DisableKeyProfiles()
        {
            EnableKeyProfiles(FALSE);
        }


    protected:        // event handlers

        //! Updates the wxKeyMonitorTextCtrl control...
        void OnIdle(wxIdleEvent & event);

        void OnTreeCommandSelected(wxTreeEvent & event);
        void OnListCommandSelected(wxCommandEvent & event);

        void OnBindingSelected(wxCommandEvent & event);
        void OnProfileSelected(wxCommandEvent & event);
        void OnCategorySelected(wxCommandEvent & event);

        void OnProfileEditing(wxCommandEvent & event);
        void OnApplyChanges(wxCommandEvent & event);
        void OnAssignKey(wxCommandEvent & event);
        void OnRemoveKey(wxCommandEvent & event);
        void OnRemoveAllKey(wxCommandEvent & event);
        void OnAddProfile(wxCommandEvent & event);
        void OnRemoveProfile(wxCommandEvent & event);

        //! Handles the notifications received from the wxKeyMonitorTextCtrl.
        void OnKeyPressed(wxCommandEvent & event);


    protected:      // build functions; these ones can be overridden to
        // customize wxKeyConfigPanel appearances

        virtual void BuildCtrls();
        virtual wxSizer * BuildColumn1();
        virtual wxSizer * BuildColumn2();
        virtual wxSizer * BuildMain(wxSizer *, wxSizer *, bool);


    protected:        // utilities

        virtual void UpdateButtons();
        virtual void UpdateDesc();
        virtual void FillInBindings();
        virtual void Reset();
        virtual void AddRootIfMissing(const wxString & rootname);


        //! Returns the tree item ID of the currently selected command.
        wxTreeItemId GetSelCmdId() const;

        //! Returns the control with the list of the customizable commands.
        wxControl * GetMainCtrl() const;

        //! Returns TRUE if the #m_pCommandsTree control is being used.
        //! Returns FALSE if the #m_pCommandsList and #m_pCategories are being used.
        bool IsUsingTreeCtrl() const
        {
            return (m_nBuildMode & wxKEYBINDER_USE_TREECTRL) != 0;
        }

        //! Returns TRUE if the currently selected command is valid.
        bool IsSelectedValidCmd() const;

        //! Returns the name of the currently selected command.
        wxString GetSelCmdStr() const;

        //! Returns the selected command.
        wxCmd * GetSelCmd() const;

        //! Hides/Shows the given sizer detaching it from the main sizer
        //! of this window or prepending it to the main sizer.
        virtual void ShowSizer(wxSizer * toshow, bool show);


    protected:        // members

        //! The construction mode.
        int m_nBuildMode;

        //! If TRUE, a keyprofiles combobox will be shown to allow the
        //! user to choose the keyprofile to modify.
        bool m_bEnableKeyProfiles;

        //! The currently selected command.
        wxCmd * m_pCurrCmd;

        //! The temporary keybinder used while interactively editing shortcuts
        //! to allow to eventually discard the changes. See #m_bProfileHasBeenModified.
        wxKeyProfile m_kBinder;

        //! The index of the currently selected profile.
        int m_nCurrentProf;

        //! TRUE if the current profile has been modified by the user.
        //! This is used to detect changes when the user choose to edit
        //! another profile using the m_pKeyProfiles control: if the current
        //! profile has been modified, a dialog will ask to the user if he wants
        //! to save its changes or not...
        bool m_bProfileHasBeenModified;

    protected:        // the subwindows of this dialog

        //! The special text control used to intercept keypresses.
        wxKeyMonitorTextCtrl * m_pKeyField;

        wxButton * m_pAssignBtn;
        wxButton * m_pRemoveBtn;
        wxButton * m_pRemoveAllBtn;

        // used when wxKEYBINDER_USE_TREECTRL is in the build flags
        wxTreeCtrl * m_pCommandsTree;

        // used when wxKEYBINDER_USE_LISTBOX is in the build flags
        wxComboBox * m_pCategories;
        wxListBox * m_pCommandsList;

        wxListBox * m_pBindings;
        wxComboBox * m_pKeyProfiles;
        wxSizer * m_pKeyProfilesSizer;

#ifdef __WXGTK__
        // on wxGTK wxStaticText does not implement line wrapping
        // (which is required !!) so we will use a wxTextCtrl (on wxGTK)
        wxTextCtrl * m_pDescLabel;
#else
        wxStaticText * m_pDescLabel;
#endif
        wxStaticText * m_pCurrCmdField;

    private:
        DECLARE_CLASS(wxKeyConfigPanel)
        DECLARE_EVENT_TABLE()
};



#endif // __KEYBINDER_G__

