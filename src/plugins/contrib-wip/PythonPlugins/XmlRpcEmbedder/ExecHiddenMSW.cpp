// For compilers that support precompilation, includes "wx.h".
#include "ExecHiddenMSW.h"

#ifdef __WXMSW__
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/utils.h>
    #include <wx/app.h>
    #include <wx/intl.h>
    #include <wx/log.h>
    #include <wx/module.h>
#endif

#include <wx/process.h>
#include <wx/apptrait.h>
#include <wx/msw/private.h>

#include <ctype.h>

#if !defined(__GNUWIN32__) && !defined(__SALFORDC__) && !defined(__WXMICROWIN__)
    #include <direct.h>
    #ifndef __MWERKS__
        #include <dos.h>
    #endif
#endif

#if defined(__GNUWIN32__)
    #include <sys/unistd.h>
    #include <sys/stat.h>
#endif

#if !defined(__WXMICROWIN__)
    #ifndef __UNIX__
        #include <io.h>
    #endif

    #ifndef __GNUWIN32__
        #include <shellapi.h>
    #endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !(defined(_MSC_VER) && (_MSC_VER > 800))
    #include <errno.h>
#endif
#include <stdarg.h>

#define wxWM_PROC_TERMINATED (WM_USER + 10000)


extern "C" WXDLLIMPEXP_BASE HWND
wxCreateHiddenWindow(LPCTSTR * pclassname, LPCTSTR classname, WNDPROC wndproc);

static const wxChar * wxMSWEXEC_WNDCLASSNAME2 = wxT("_wxExecute_Internal_Class2");
static const wxChar * gs_classForHiddenWindow2 = NULL;


// structure describing the process we're being waiting for
struct wxExecuteData2
{
    public:
        ~wxExecuteData2()
        {
            if (!::CloseHandle(hProcess))
            {
                wxLogLastError(wxT("CloseHandle(hProcess)"));
            }
        }

        HWND       hWnd;          // window to send wxWM_PROC_TERMINATED to
        HANDLE     hProcess;      // handle of the process
#ifdef __WXMSW__
        DWORD      dwProcessId;   // pid of the process
        wxProcess * handler;
        DWORD      dwExitCode;    // the exit code of the process
#else
        uint32_t      dwProcessId;   // pid of the process
        wxProcess * handler;
        uint32_t      dwExitCode;    // the exit code of the process
#endif
        bool       state;         // set to false when the process finishes
};

//LRESULT APIENTRY _EXPORT wxExecuteWindowCbk2(HWND hWnd, UINT message,
//                                             WPARAM wParam, LPARAM lParam)
LRESULT CALLBACK wxExecuteWindowCbk2(HWND hWnd, UINT message,
                                     WPARAM wParam, LPARAM lParam)
{
    if (message == wxWM_PROC_TERMINATED)
    {
        DestroyWindow(hWnd);    // we don't need it any more
        wxExecuteData2 * const data = (wxExecuteData2 *)lParam;

        if (data->handler)
        {
            data->handler->OnTerminate((int)data->dwProcessId,
                                       (int)data->dwExitCode);
        }

        if (data->state)
        {
            // we're executing synchronously, tell the waiting thread
            // that the process finished
            data->state = 0;
        }
        else
        {
            // asynchronous execution - we should do the clean up
            delete data;
        }

        return 0;
    }
    else
    {
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
}


// thread function for the thread monitoring the process termination
#ifdef __WXMSW__
    static DWORD __stdcall wxExecuteThread2(void * arg)
#else
    static uint32_t __stdcall wxExecuteThread2(void * arg)
#endif
{
    wxExecuteData2 * const data = (wxExecuteData2 *)arg;

    if (::WaitForSingleObject(data->hProcess, INFINITE) != WAIT_OBJECT_0)
    {
        wxLogDebug(_T("Waiting for the process termination failed!"));
    }

    // get the exit code
    if (!::GetExitCodeProcess(data->hProcess, &data->dwExitCode))
    {
        wxLogLastError(wxT("GetExitCodeProcess"));
    }

    wxASSERT_MSG(data->dwExitCode != STILL_ACTIVE,
                 wxT("process should have terminated"));
    // send a message indicating process termination to the window
    ::SendMessage(data->hWnd, wxWM_PROC_TERMINATED, 0, (LPARAM)data);
    return 0;
}



long wxExecuteHidden(const wxString & cmd, int flags, wxProcess * handler)
{
    wxCHECK_MSG(!cmd.empty(), 0, wxT("empty command in wxExecute"));
#if wxUSE_THREADS
    // for many reasons, the code below breaks down if it's called from another
    // thread -- this could be fixed, but as Unix versions don't support this
    // neither I don't want to waste time on this now
    wxASSERT_MSG(wxThread::IsMain(),
                 _T("wxExecute() can be called only from the main thread"));
#endif // wxUSE_THREADS
    wxString command;
#if wxUSE_IPC
#endif // wxUSE_IPC
    {
        // no DDE
        command = cmd;
    }
    // the IO redirection is only supported with wxUSE_STREAMS
    BOOL redirect = FALSE;
    // create the process
    STARTUPINFO si;
    wxZeroMemory(si);
    si.cb = sizeof(si);

    //DM ADDED CODE HERE
    // we don't show the (console) process
    // window by default, but this can be overridden by the caller by
    // specifying wxEXEC_NOHIDE flag
    if (!(flags & wxEXEC_NOHIDE))
    {
        si.dwFlags |= STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
    }

    //END DM ADDED CODE HERE
    PROCESS_INFORMATION pi;
    uint32_t dwFlags = CREATE_SUSPENDED;
    dwFlags |= CREATE_DEFAULT_ERROR_MODE ;
    bool ok = ::CreateProcess
              (
                  NULL,         // application name (use only cmd line)
                  command.wchar_str(),  // full command line
                  NULL,         // security attributes: defaults for both
                  NULL,         //   the process and its main thread
                  redirect,     // inherit handles if we use pipes
                  dwFlags,      // process creation flags
                  NULL,         // environment (use the same)
                  NULL,         // current directory (use the same)
                  &si,          // startup info (unused here)
                  &pi           // process info
              ) != 0;

    if (!ok)
    {
        wxLogSysError(_("Execution of command '%s' failed"), command.c_str());
        return flags & wxEXEC_SYNC ? -1 : 0;
    }

    // create a hidden window to receive notification about process
    // termination
    HWND hwnd = wxCreateHiddenWindow
                (
                    &gs_classForHiddenWindow2,
                    wxMSWEXEC_WNDCLASSNAME2,
                    (WNDPROC)wxExecuteWindowCbk2
                );
    wxASSERT_MSG(hwnd, wxT("can't create a hidden window for wxExecute"));
    // Alloc data
    wxExecuteData2 * data = new wxExecuteData2;
    data->hProcess    = pi.hProcess;
    data->dwProcessId = pi.dwProcessId;
    data->hWnd        = hwnd;
    data->state       = (flags & wxEXEC_SYNC) != 0;

    if (flags & wxEXEC_SYNC)
    {
        // handler may be !NULL for capturing program output, but we don't use
        // it wxExecuteData struct in this case
        data->handler = NULL;
    }
    else
    {
        // may be NULL or not
        data->handler = handler;
    }

#ifdef __WXMSW__
    DWORD tid;
#else
    uint32_t tid;
#endif
    HANDLE hThread = ::CreateThread(NULL,
                                    0,
                                    wxExecuteThread2,
                                    (void *)data,
                                    0,
                                    &tid);

    // resume process we created now - whether the thread creation succeeded or
    // not
    if (::ResumeThread(pi.hThread) == (uint32_t) -1)
    {
        // ignore it - what can we do?
        wxLogLastError(wxT("ResumeThread in wxExecute"));
    }

    // close unneeded handle
    if (!::CloseHandle(pi.hThread))
    {
        wxLogLastError(wxT("CloseHandle(hThread)"));
    }

    if (!hThread)
    {
        wxLogLastError(wxT("CreateThread in wxExecute"));
        DestroyWindow(hwnd);
        delete data;
        // the process still started up successfully...
        return pi.dwProcessId;
    }

    ::CloseHandle(hThread);

    if (!(flags & wxEXEC_SYNC))
    {
        // clean up will be done when the process terminates
        // return the pid
        return pi.dwProcessId;
    }

    wxAppTraits * traits = wxTheApp ? wxTheApp->GetTraits() : NULL;
    wxCHECK_MSG(traits, -1, _T("no wxAppTraits in wxExecute()?"));
    void * cookie = NULL;

    if (!(flags & wxEXEC_NODISABLE))
    {
        // disable all app windows while waiting for the child process to finish
        cookie = traits->BeforeChildWaitLoop();
    }

    // wait until the child process terminates
    while (data->state)
    {
        // don't eat 100% of the CPU -- ugly but anything else requires
        // real async IO which we don't have for the moment
        ::Sleep(50);
        // we must process messages or we'd never get wxWM_PROC_TERMINATED
        //traits->AlwaysYield();
        wxYield();
    }

    if (!(flags & wxEXEC_NODISABLE))
    {
        // reenable disabled windows back
        traits->AfterChildWaitLoop(cookie);
    }

    uint32_t dwExitCode = data->dwExitCode;
    delete data;
    // return the exit code
    return dwExitCode;
}

long wxExecuteHidden(wxChar ** argv, int flags, wxProcess * handler)
{
    wxString command;

    for (;;)
    {
        command += *argv++;

        if (!*argv)
        {
            break;
        }

        command += _T(' ');
    }

    return wxExecuteHidden(command, flags, handler);
}

#endif /*__WXMSW__*/
