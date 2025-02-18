/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
*/
#define DAP_DEBUG_ENABLE 1

// System include files
#include <algorithm>
#include <tinyxml2.h>

#include <wx/app.h>
#include <wx/event.h>
#include <wx/xrc/xmlres.h>
#include <wx/wxscintilla.h>
#ifndef __WX_MSW__
    #include <dirent.h>
    #include <stdlib.h>
#endif
#ifdef __MINGW64__
    #include <debugapi.h>
#endif // __MINGW64__

// CB include files (not DAP)
#include "sdk.h"
#include "cbdebugger_interfaces.h"
#include "cbproject.h"
#include "compilercommandgenerator.h"
#include "compilerfactory.h"
#include "configurationpanel.h"
#include "configmanager.h"
#include "infowindow.h"
#include "macrosmanager.h"
#include "manager.h"
#include "pipedprocess.h"
#include "projectmanager.h"

// DAP debugger includes
#include "dlg_ProjectOptions.h"
#include "dlg_SettingsOptions.h"
#include "dlg_WatchEdit.h"
#include "debugger_logger.h"
#include "plugin.h"
#include "DAP_Debugger_State.h"
#include "DAP_Breakpoints.h"

// DAP protocol includes
#include "dap.hpp"

namespace
{
//XML file root tag for data
static const char * XML_CFG_ROOT_TAG = "Debugger_layout_file";

int const id_gdb_poll_timer = wxNewId();

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
// this auto-registers the plugin
PluginRegistrant<Debugger_DAP> reg("debugger_dap");

wxString GetLibraryPath(const wxString & oldLibPath, Compiler * compiler, ProjectBuildTarget * target, cbProject * project)
{
    if (compiler && target)
    {
        wxString newLibPath;
        const wxString libPathSep = platform::windows ? ";" : ":";
        newLibPath << "." << libPathSep;
        CompilerCommandGenerator * generator = compiler->GetCommandGenerator(project);
        newLibPath << GetStringFromArray(generator->GetLinkerSearchDirs(target), libPathSep);
        delete generator;

        if (newLibPath.Mid(newLibPath.Length() - 1, 1) != libPathSep)
        {
            newLibPath << libPathSep;
        }

        newLibPath << oldLibPath;
        return newLibPath;
    }
    else
    {
        return oldLibPath;
    }
}

} // anonymous namespace

// events handling
BEGIN_EVENT_TABLE(Debugger_DAP, cbDebuggerPlugin)
    EVT_IDLE(Debugger_DAP::OnIdle)
    EVT_TIMER(id_gdb_poll_timer, Debugger_DAP::OnTimer)

END_EVENT_TABLE()

// constructor
Debugger_DAP::Debugger_DAP() :
    cbDebuggerPlugin("DAP", "debugger_dap"),
    m_pProject(nullptr),
    m_dapPid(0)
{
    if (!Manager::LoadResource("debugger_dap.zip"))
    {
        NotifyMissingFile("debugger_dap.zip");
    }

    m_pTerminalMgr = nullptr;
    Debugger_State::SetState(Debugger_State::eDAPState::NotConnected);
    m_pLogger = new dbg_DAP::LogPaneLogger(this);
    // bind the client events
    m_dapClient.Bind(wxEVT_DAP_STOPPED_EVENT,                   &Debugger_DAP::OnStopped,               this);
    m_dapClient.Bind(wxEVT_DAP_INITIALIZED_EVENT,               &Debugger_DAP::OnInitializedEvent,      this);
    m_dapClient.Bind(wxEVT_DAP_INITIALIZE_RESPONSE,             &Debugger_DAP::OnInitializeResponse,    this);
    m_dapClient.Bind(wxEVT_DAP_EXITED_EVENT,                    &Debugger_DAP::OnExited,                this);
    m_dapClient.Bind(wxEVT_DAP_TERMINATED_EVENT,                &Debugger_DAP::OnTerminated,            this);
    m_dapClient.Bind(wxEVT_DAP_STACKTRACE_RESPONSE,             &Debugger_DAP::OnStackTrace,            this);
    m_dapClient.Bind(wxEVT_DAP_SCOPES_RESPONSE,                 &Debugger_DAP::OnScopes,                this);
    m_dapClient.Bind(wxEVT_DAP_VARIABLES_RESPONSE,              &Debugger_DAP::OnVariables,             this);
    m_dapClient.Bind(wxEVT_DAP_OUTPUT_EVENT,                    &Debugger_DAP::OnOutput,                this);
    m_dapClient.Bind(wxEVT_DAP_BREAKPOINT_LOCATIONS_RESPONSE,   &Debugger_DAP::OnBreakpointLocations,   this);
    m_dapClient.Bind(wxEVT_DAP_LOST_CONNECTION,                 &Debugger_DAP::OnConnectionError,       this);
    m_dapClient.Bind(wxEVT_DAP_SET_SOURCE_BREAKPOINT_RESPONSE,  &Debugger_DAP::OnBreakpointDataSet,     this);
    m_dapClient.Bind(wxEVT_DAP_SET_FUNCTION_BREAKPOINT_RESPONSE, &Debugger_DAP::OnBreakpointFunctionSet, this);
    m_dapClient.Bind(wxEVT_DAP_LAUNCH_RESPONSE,                 &Debugger_DAP::OnLaunchResponse,        this);
    m_dapClient.Bind(wxEVT_DAP_RUN_IN_TERMINAL_REQUEST,         &Debugger_DAP::OnRunInTerminalRequest,  this);
    m_dapClient.Bind(wxEVT_DAP_LOG_EVENT,                       &Debugger_DAP::OnDapLog,                this);
    m_dapClient.Bind(wxEVT_DAP_MODULE_EVENT,                    &Debugger_DAP::OnDapModuleEvent,        this);
    m_dapClient.Bind(wxEVT_DAP_CONFIGURARIONE_DONE_RESPONSE,    &Debugger_DAP::OnConfigurationDoneResponse,  this);
    m_dapClient.Bind(wxEVT_DAP_THREADS_RESPONSE,                &Debugger_DAP::OnThreadResponse,         this);
    m_dapClient.Bind(wxEVT_DAP_STOPPED_ON_ENTRY_EVENT,          &Debugger_DAP::OnStopOnEntryEvent,      this);
    m_dapClient.Bind(wxEVT_DAP_PROCESS_EVENT,                   &Debugger_DAP::OnProcessEvent,          this);
    m_dapClient.Bind(wxEVT_DAP_BREAKPOINT_EVENT,                &Debugger_DAP::OnBreakpointEvent,       this);
    m_dapClient.Bind(wxEVT_DAP_CONTINUED_EVENT,                 &Debugger_DAP::OnContinuedEvent,       this);
    m_dapClient.Bind(wxEVT_DAP_DEBUGPYWAITINGFORSERVER_EVENT,   &Debugger_DAP::OnDebugPYWaitingForServerEvent,  this);
    m_dapClient.SetWantsLogEvents(true); // send use log events
    pDAPBreakpoints = new DBG_DAP_Breakpoints(this, m_pLogger, &m_dapClient);
    pDAPCallStack = new DBG_DAP_CallStack(this, m_pLogger);
    pDAPWatches = new DBG_DAP_Watches(this, m_pLogger, &m_dapClient);
}

// destructor
Debugger_DAP::~Debugger_DAP()
{
    // unbind the client events
    m_dapClient.Unbind(wxEVT_DAP_STOPPED_EVENT,                   &Debugger_DAP::OnStopped,                 this);
    m_dapClient.Unbind(wxEVT_DAP_INITIALIZED_EVENT,               &Debugger_DAP::OnInitializedEvent,        this);
    m_dapClient.Unbind(wxEVT_DAP_INITIALIZE_RESPONSE,             &Debugger_DAP::OnInitializeResponse,      this);
    m_dapClient.Unbind(wxEVT_DAP_EXITED_EVENT,                    &Debugger_DAP::OnExited,                  this);
    m_dapClient.Unbind(wxEVT_DAP_TERMINATED_EVENT,                &Debugger_DAP::OnTerminated,              this);
    m_dapClient.Unbind(wxEVT_DAP_STACKTRACE_RESPONSE,             &Debugger_DAP::OnStackTrace,              this);
    m_dapClient.Unbind(wxEVT_DAP_SCOPES_RESPONSE,                 &Debugger_DAP::OnScopes,                  this);
    m_dapClient.Unbind(wxEVT_DAP_VARIABLES_RESPONSE,              &Debugger_DAP::OnVariables,               this);
    m_dapClient.Unbind(wxEVT_DAP_OUTPUT_EVENT,                    &Debugger_DAP::OnOutput,                  this);
    m_dapClient.Unbind(wxEVT_DAP_BREAKPOINT_LOCATIONS_RESPONSE,   &Debugger_DAP::OnBreakpointLocations,     this);
    m_dapClient.Unbind(wxEVT_DAP_LOST_CONNECTION,                 &Debugger_DAP::OnConnectionError,         this);
    m_dapClient.Unbind(wxEVT_DAP_SET_SOURCE_BREAKPOINT_RESPONSE,  &Debugger_DAP::OnBreakpointDataSet,       this);
    m_dapClient.Unbind(wxEVT_DAP_SET_FUNCTION_BREAKPOINT_RESPONSE, &Debugger_DAP::OnBreakpointFunctionSet,  this);
    m_dapClient.Unbind(wxEVT_DAP_LAUNCH_RESPONSE,                 &Debugger_DAP::OnLaunchResponse,          this);
    m_dapClient.Unbind(wxEVT_DAP_RUN_IN_TERMINAL_REQUEST,         &Debugger_DAP::OnRunInTerminalRequest,    this);
    m_dapClient.Unbind(wxEVT_DAP_LOG_EVENT,                       &Debugger_DAP::OnDapLog,                  this);
    m_dapClient.Unbind(wxEVT_DAP_MODULE_EVENT,                    &Debugger_DAP::OnDapModuleEvent,          this);
    m_dapClient.Unbind(wxEVT_DAP_CONFIGURARIONE_DONE_RESPONSE,    &Debugger_DAP::OnConfigurationDoneResponse,  this);
    m_dapClient.Unbind(wxEVT_DAP_THREADS_RESPONSE,                &Debugger_DAP::OnThreadResponse,           this);
    m_dapClient.Unbind(wxEVT_DAP_STOPPED_ON_ENTRY_EVENT,          &Debugger_DAP::OnStopOnEntryEvent,        this);
    m_dapClient.Unbind(wxEVT_DAP_PROCESS_EVENT,                   &Debugger_DAP::OnProcessEvent,            this);
    m_dapClient.Unbind(wxEVT_DAP_BREAKPOINT_EVENT,                &Debugger_DAP::OnBreakpointEvent,         this);
    m_dapClient.Unbind(wxEVT_DAP_CONTINUED_EVENT,                 &Debugger_DAP::OnContinuedEvent,         this);
    m_dapClient.Unbind(wxEVT_DAP_DEBUGPYWAITINGFORSERVER_EVENT,   &Debugger_DAP::OnDebugPYWaitingForServerEvent,  this);

    if (m_dapPid != 0)
    {
        wxKill(m_dapPid);
        m_dapPid = 0;
    }
}

void Debugger_DAP::OnAttachReal()
{
    Manager::Get()->GetLogManager()->DebugLog(wxString::Format("%s %d", __PRETTY_FUNCTION__, __LINE__));
    m_timer_poll_debugger.SetOwner(this, id_gdb_poll_timer);
    pDAPBreakpoints->OnAttachReal();
    // Do no use cbEVT_PROJECT_OPEN as the project may not be active!!!!
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,  new cbEventFunctor<Debugger_DAP, CodeBlocksEvent>(this, &Debugger_DAP::OnProjectOpened));
}

void Debugger_DAP::OnReleaseReal(bool appShutDown)
{
    // Do not log anything as we are closing
    DAPDebuggerResetData(dbg_DAP::ResetDataType::ResetData_All);
    pDAPBreakpoints->OnReleaseReal(appShutDown);

    if (m_pTerminalMgr) //remove the Shell Terminals Notebook from its dockable window and delete it
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pTerminalMgr;
        Manager::Get()->ProcessEvent(evt);
        m_pTerminalMgr->Destroy();
        m_pTerminalMgr = nullptr;
    }
}

void Debugger_DAP::GetCurrentPosition(wxString & filename, int & line)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
}

bool Debugger_DAP::SupportsFeature(cbDebuggerFeature::Flags flag)
{
    switch (flag)
    {
        case cbDebuggerFeature::Breakpoints:
        case cbDebuggerFeature::Callstack:
        case cbDebuggerFeature::Watches:
            return true;

        case cbDebuggerFeature::SetNextStatement:
        case cbDebuggerFeature::RunToCursor:
        case cbDebuggerFeature::CPURegisters:
        case cbDebuggerFeature::Disassembly:
        case cbDebuggerFeature::ExamineMemory:
        case cbDebuggerFeature::Threads:
        case cbDebuggerFeature::ValueTooltips:
        default:
            return false;
    }
}

cbDebuggerConfiguration * Debugger_DAP::LoadConfig(const ConfigManagerWrapper & config)
{
    return new dbg_DAP::DebuggerConfiguration(config);
}

dbg_DAP::DebuggerConfiguration & Debugger_DAP::GetActiveConfigEx()
{
    return static_cast<dbg_DAP::DebuggerConfiguration &>(GetActiveConfig());
}

cbConfigurationPanel * Debugger_DAP::GetProjectConfigurationPanel(wxWindow * parent, cbProject * project)
{
    dbg_DAP::DebuggerOptionsProjectDlg * dlg = new dbg_DAP::DebuggerOptionsProjectDlg(parent, this, project);
    return dlg;
}

void Debugger_DAP::OnIdle(wxIdleEvent & event)
{
    event.Skip();
}

void Debugger_DAP::OnTimer(wxTimerEvent & /*event*/)
{
    wxWakeUpIdle();
}

void Debugger_DAP::UpdateDebugDialogs(bool bClearAllData)
{
    pDAPBreakpoints->UpdateDebugDialogs(bClearAllData);
    pDAPCallStack->UpdateDebugDialogs(bClearAllData);
    pDAPWatches->UpdateDebugDialogs(bClearAllData);

    if (bClearAllData)
    {
        cbExamineMemoryDlg * pDialogExamineMemory = Manager::Get()->GetDebuggerManager()->GetExamineMemoryDialog();

        if (pDialogExamineMemory)
        {
            pDialogExamineMemory->SetBaseAddress("");
            pDialogExamineMemory->Clear();
        }

        cbCPURegistersDlg * pDialogCPURegisters = Manager::Get()->GetDebuggerManager()->GetCPURegistersDialog();

        if (pDialogCPURegisters)
        {
            pDialogCPURegisters->Clear();
        }

        cbDisassemblyDlg * pDialogDisassembly = Manager::Get()->GetDebuggerManager()->GetDisassemblyDialog();

        if (pDialogDisassembly)
        {
            cbStackFrame sf;
            pDialogDisassembly->Clear(sf);
        }
    }
}

bool Debugger_DAP::Debug(bool breakOnEntry)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("starting debugger"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    StartType start_type = breakOnEntry ? StartTypeStepInto : StartTypeRun;
    return !EnsureBuildUpToDate(start_type);
}

// "=========================================================================================="
// "   ____    ___    __  __   ____    ___   _       _____   ____        ___      __  _____   "
// "  / ___|  / _ \  |  \/  | |  _ \  |_ _| | |     | ____| |  _ \      |_ _|    / / |  ___|  "
// " | |     | | | | | |\/| | | |_) |  | |  | |     |  _|   | |_) |      | |    / /  | |_     "
// " | |___  | |_| | | |  | | |  __/   | |  | |___  | |___  |  _ <       | |   / /   |  _|    "
// "  \____|  \___/  |_|  |_| |_|     |___| |_____| |_____| |_| \_\     |___| /_/    |_|      "
// "                                                                                          "
// "=========================================================================================="

bool Debugger_DAP::SelectCompiler(cbProject & project, Compiler *& compiler,
                                  ProjectBuildTarget *& target, long pid_to_attach)
{
    // select the build target to debug
    target = NULL;
    compiler = NULL;
    wxString active_build_target = project.GetActiveBuildTarget();

    if (pid_to_attach == 0)
    {
        if (!project.BuildTargetValid(active_build_target, false))
        {
            int tgtIdx = project.SelectTarget();

            if (tgtIdx == -1)
            {
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Selecting target cancelled"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
                return false;
            }

            target = project.GetBuildTarget(tgtIdx);
            active_build_target = target->GetTitle();
        }
        else
        {
            target = project.GetBuildTarget(active_build_target);
        }

        // make sure it's not a commands-only target
        if (target->GetTargetType() == ttCommandsOnly)
        {
            cbMessageBox(_("The selected target is only running pre/post build step commands\n"
                           "Can't debug such a target..."), _("Information"), wxICON_INFORMATION);
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("The selected target is only running pre/post build step commands,Can't debug such a target... ")), dbg_DAP::LogPaneLogger::LineType::Error);
            return false;
        }

        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Selecting target: %s"), target->GetTitle()), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        // find the target's compiler (to see which debugger to use)
        compiler = CompilerFactory::GetCompiler(target ? target->GetCompilerID() : project.GetCompilerID());
    }
    else
    {
        compiler = CompilerFactory::GetDefaultCompiler();
    }

    return true;
}

bool Debugger_DAP::CompilerFinished(bool compilerFailed, StartType startType)
{
    pDAPBreakpoints->CompilerFinished(compilerFailed, startType);

    if (!compilerFailed && (startType != StartTypeUnknown))
    {
        ProjectManager & project_manager = *Manager::Get()->GetProjectManager();
        cbProject * project = project_manager.GetActiveProject();

        if (project)
        {
            return StartDebugger(project, startType) == 0;
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no active project"), dbg_DAP::LogPaneLogger::LineType::Error);
        }
    }

    return false;
}

// "===================================================================================="
// " ____    ____     ___        _   _____    ____   _____       ___      __  _____     "
// " |  _ \  |  _ \   / _ \      | | | ____|  / ___| |_   _|     |_ _|    / / |  ___|   "
// " | |_) | | |_) | | | | |  _  | | |  _|   | |       | |        | |    / /  | |_      "
// " |  __/  |  _ <  | |_| | | |_| | | |___  | |___    | |        | |   / /   |  _|     "
// " |_|     |_| \_\  \___/   \___/  |_____|  \____|   |_|       |___| /_/    |_|       "
// "                                                                                    "
// "===================================================================================="

void Debugger_DAP::OnProjectOpened(CodeBlocksEvent & event)
{
    // allow others to catch this
    event.Skip();

    if (GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::PersistDebugElements))
    {
        LoadStateFromFile(event.GetProject());
    }
}

void Debugger_DAP::CleanupWhenProjectClosed(cbProject * project)
{
    if (GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::PersistDebugElements))
    {
        SaveStateToFile(project);
    }

    pDAPBreakpoints->CleanupWhenProjectClosed(project);
    pDAPCallStack->CleanupWhenProjectClosed(project);
    pDAPWatches->CleanupWhenProjectClosed(project);

    if (!project)
    {
        UpdateDebugDialogs(true);
    }

    ClearLog();

    if (m_pTerminalMgr) //remove the Shell Terminals Notebook from its dockable window and delete it
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pTerminalMgr;
        Manager::Get()->ProcessEvent(evt);
        m_pTerminalMgr->Destroy();
        m_pTerminalMgr = nullptr;
    }
}

// "========================================================================================================="
// "  _          _      _   _   _   _    ____   _   _        __    ____    _____      _      ____    _____   "
// " | |        / \    | | | | | \ | |  / ___| | | | |      / /   / ___|  |_   _|    / \    |  _ \  |_   _|  "
// " | |       / _ \   | | | | |  \| | | |     | |_| |     / /    \___ \    | |     / _ \   | |_) |   | |    "
// " | |___   / ___ \  | |_| | | |\  | | |___  |  _  |    / /      ___) |   | |    / ___ \  |  _ <    | |    "
// " |_____| /_/   \_\  \___/  |_| \_|  \____| |_| |_|   /_/      |____/    |_|   /_/   \_\ |_| \_\   |_|    "
// "                                                                                                         "
// "========================================================================================================="

int Debugger_DAP::StartDebugger(cbProject * project, StartType start_type)
{
    Compiler * compiler;
    ProjectBuildTarget * target;
    SelectCompiler(*project, compiler, target, 0);

    if (!compiler)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no compiler found!"), dbg_DAP::LogPaneLogger::LineType::Error);
        return 2;
    }

    if (!target)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no target found!"), dbg_DAP::LogPaneLogger::LineType::Error);
        return 3;
    }

    // is debugger accessible, i.e. can we find it?
    wxString dap_debugger = GetActiveConfigEx().GetDAPExecutable(true);

    if (!wxFileExists(dap_debugger))
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Cannot find dap_debugger. Currently set to: %s"), dap_debugger), dbg_DAP::LogPaneLogger::LineType::Error);
        return 4;
    }

    wxString dap_port_number = GetActiveConfigEx().GetDAPPortNumber();

    if (dap_port_number.IsEmpty())
    {
        dap_port_number = "12345";
    }

    wxString debuggee, working_dir;

    if (!GetDebuggee(debuggee, working_dir, target))
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot find debuggee!"), dbg_DAP::LogPaneLogger::LineType::Error);
        return 6;
    }

    bool bConsole = target->GetTargetType() == ttConsoleOnly;
    // ---------------------------------------------------------------------------------------------------------------------
    int res = LaunchDebugger(project, dap_debugger, debuggee, dap_port_number, working_dir, 0, bConsole, start_type);
    // ---------------------------------------------------------------------------------------------------------------------

    if (res != 0)
    {
        return res;
    }

#ifndef __WXMSW__

    // Windows spawns a command window and as such does not require the terminal manager
    if (bConsole && !m_pTerminalMgr)
    {
        m_pTerminalMgr = new DAPTerminalManager(Manager::Get()->GetAppWindow(), wxID_ANY, _("DAP Terminal I/O"));
        CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
        evt.name = _T("DAPWindow");
        evt.title = _("DAP Terminal Display");
        evt.pWindow = m_pTerminalMgr;
        evt.dockSide = CodeBlocksDockEvent::dsFloating;
        evt.desiredSize.Set(400, 300);
        evt.floatingSize.Set(400, 300);
        evt.minimumSize.Set(200, 150);
        evt.stretch = true;
        Manager::Get()->ProcessEvent(evt);
    }
    else
    {
        if (m_pTerminalMgr) //remove the Shell Terminals Notebook from its dockable window and delete it
        {
            CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
            evt.pWindow = m_pTerminalMgr;
            Manager::Get()->ProcessEvent(evt);
            m_pTerminalMgr->Destroy();
            m_pTerminalMgr = nullptr;
        }
    }

#endif //__WXMSW__
    pDAPBreakpoints->SetDebuggee(debuggee);
    m_pProject = project;
    pDAPBreakpoints->SetProject(project);
    pDAPWatches->SetProject(project);
    return 0;
}

void Debugger_DAP::LaunchDAPDebugger(cbProject * project, const wxString & dap_debugger, const wxString & dap_port_number)
{
    Compiler * compiler;
    ProjectBuildTarget * target;
    SelectCompiler(*project, compiler, target, 0);
    wxString dapStartCmd = wxString::Format("%s --port %s", dap_debugger, dap_port_number);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("dapStartCmd: %s"), dapStartCmd), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    // Setup the environment
    wxFileName debuggerFN(dap_debugger);
    wxString wdir = debuggerFN.GetPath();

    if (wdir.empty())
    {
        wdir = m_pProject ? m_pProject->GetBasePath() : _T(".");
    }

    wxExecuteEnv execEnv;
    execEnv.cwd = wdir;
    // Read the current environment variables and then make changes to them.
    wxGetEnvMap(&execEnv.env);
#ifndef __WXMAC__
    wxString oldLibPath;
    wxGetEnv(CB_LIBRARY_ENVVAR, &oldLibPath);
    wxString newLibPath = GetLibraryPath(oldLibPath, compiler, target, project);
    execEnv.env[CB_LIBRARY_ENVVAR] = newLibPath;

    if (HasDebugLog())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                 __LINE__,
                                 wxString::Format(_("setting execEnv.env[\"%s\"]=%s"), CB_LIBRARY_ENVVAR, newLibPath),
                                 dbg_DAP::LogPaneLogger::LineType::Debug);
    }

#endif
    wxString newPythonHomeSetting = GetActiveConfigEx().GetDAPPythonHomeEnvSetting();

    if (!newPythonHomeSetting.IsEmpty())
    {
        execEnv.env["PYTHONHOME"] = newPythonHomeSetting;

        if (HasDebugLog())
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                     __LINE__,
                                     wxString::Format(_("setting execEnv.env[PYTHONHOME]=%s"), newPythonHomeSetting),
                                     dbg_DAP::LogPaneLogger::LineType::Debug);
        }
    }

    if (HasDebugLog())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                 __LINE__,
                                 wxString::Format(_("\nexecEnv.cwd: %s"), execEnv.cwd),
                                 dbg_DAP::LogPaneLogger::LineType::Debug);

        for (wxEnvVariableHashMap::iterator it = execEnv.env.begin(); it != execEnv.env.end(); ++it)
        {
            m_pLogger->LogDAPMsgType("",
                                     __LINE__,
                                     wxString::Format("execEnv.env[%s]=%s", it->first, it->second),
                                     dbg_DAP::LogPaneLogger::LineType::Debug);
        }
    }

    // start the dap_debugger process
    // NOTE: If the debugger does not start check the PYTHONHOME environment variable is set correctly!!!!
    m_dapPid = wxExecute(dapStartCmd, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER | wxEXEC_SHOW_CONSOLE, NULL, &execEnv);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("finished dapStartCmd: %s , m_dapPid: %ld"), dapStartCmd, m_dapPid), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

int Debugger_DAP::LaunchDebugger(cbProject * project,
                                 const wxString & dap_debugger,
                                 const wxString & debuggee,
                                 const wxString & dap_port_number,
                                 const wxString & working_dir,
                                 int pid,
                                 bool bConsole,
                                 StartType start_type)
{
    // Reset the client and data
    DAPDebuggerResetData(dbg_DAP::ResetDataType::ResetData_Startup);

    if (dap_debugger.IsEmpty())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Cannot debug as no debugger executable found (full path)!"), dbg_DAP::LogPaneLogger::LineType::Error);
        return 1;
    }

    if (dap_port_number.IsEmpty())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("dap_debugger is empty!!!!"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        return 2;
    }

    if (!debuggee.IsEmpty())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("DEBUGGEE: %s"), debuggee), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    }

    wxBusyCursor cursor;

    if (GetActiveConfigEx().GetFlag(dbg_DAP::DebuggerConfiguration::RunDAPServer))
    {
        LaunchDAPDebugger(project, dap_debugger, dap_port_number);
    }
    else
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("You have configured the debugger so you need to manually run the DAP server on port %s"), dap_port_number), dbg_DAP::LogPaneLogger::LineType::UserDisplay);

        if (dap_port_number.IsEmpty())
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("dap_debugger: %s  port: %s"), dap_debugger, dap_port_number), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("dap_debugger: %s  port: <empty>"), dap_debugger), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }

        if (!debuggee.IsEmpty())
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("DEBUGGEE: %s"), debuggee), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
    }

    // For this we use socket transport. But you may choose
    // to write your own transport that implements the dap::Transport interface
    // This is useful when the user wishes to use stdin/out for communicating with
    // the dap and not over socket
    dap::SocketTransport * transport = new dap::SocketTransport();
    wxString connection = wxString::Format("tcp://127.0.0.1:%s", dap_port_number);

    if (!transport->Connect(connection, 5))
    {
        if (m_dapPid != 0)
        {
            wxKill(m_dapPid);
            m_dapPid = 0;
        }

        wxMessageBox("Failed to connect to DAP server", "DAP Debugger Plugin", wxICON_ERROR | wxOK | wxCENTRE);
        return 1;
    }

    Debugger_State::SetState(Debugger_State::eDAPState::Connected);
    // construct new client with the transport
    m_dapClient.SetTransport(transport);
    // This part is done in mode **sync**
    Debugger_State::SetState(Debugger_State::eDAPState::Running);
    // Create the DAP debuggee command line including any parameters
    Compiler * compiler;
    ProjectBuildTarget * target;
    SelectCompiler(*project, compiler, target, 0);
    wxString debugeeArgs = target->GetExecutionParameters();
    m_DAP_DebuggeeStartCMD.clear();
    m_DAP_DebuggeeStartCMD.push_back(static_cast<wxString>(debuggee));

    if (!debugeeArgs.IsEmpty())
    {
        Manager::Get()->GetMacrosManager()->ReplaceMacros(debugeeArgs);
        wxArrayString debugeeArgsArray = wxSplit(debugeeArgs, ' ');

        for (wxString param : debugeeArgsArray)
        {
            m_DAP_DebuggeeStartCMD.push_back(param);
        }
    }

    // The protocol starts by us sending an initialize request
    dap::InitializeRequestArguments initArgs;
    initArgs.linesStartAt1 = true;
    initArgs.clientID = "CB_DAP_Plugin";
    initArgs.clientName = "CB_DAP_Plugin";
    initArgs.supportsRunInTerminalRequest = true;
    m_dapClient.Initialize(&initArgs);
    //    wxString directorySearchPaths = wxEmptyString;
    //    const wxArrayString& pdirs = ParseSearchDirs(project);
    //    for (size_t i = 0; i < pdirs.GetCount(); ++i)
    //    {
    //        directorySearchPaths.Append(pdirs[i]);
    //        directorySearchPaths.Append(wxPATH_SEP);
    //    }
    //
    //    if (!directorySearchPaths.IsEmpty())
    //    {
    //        DoSendCommand(wxString::Format("directory %s", directorySearchPaths));
    //    }
    m_timer_poll_debugger.Start(20);
    return 0;
}

// "====================================================================================================="
// "  ____    _____   ____    _   _    ____        ____    ___    _   _   _____   ____     ___    _      "
// " |  _ \  | ____| | __ )  | | | |  / ___|      / ___|  / _ \  | \ | | |_   _| |  _ \   / _ \  | |     "
// " | | | | |  _|   |  _ \  | | | | | |  _      | |     | | | | |  \| |   | |   | |_) | | | | | | |     "
// " | |_| | | |___  | |_) | | |_| | | |_| |     | |___  | |_| | | |\  |   | |   |  _ <  | |_| | | |___  "
// " |____/  |_____| |____/   \___/   \____|      \____|  \___/  |_| \_|   |_|   |_| \_\  \___/  |_____| "
// "                                                                                                     "
// "====================================================================================================="

bool Debugger_DAP::RunToCursor(const wxString & filename, int line, const wxString & line_text)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    pDAPBreakpoints->RunToCursor(filename, line, line_text);

    if (Debugger_State::IsRunning())
    {
        return false;
    }
    else
    {
        return Debug(false);
    }
}

void Debugger_DAP::SetNextStatement(const wxString & filename, int line)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);

    if (IsStopped())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("-break-insert -t & -exec-jump for filename:=>%s<= line:%d", filename, line), dbg_DAP::LogPaneLogger::LineType::Command);
        //        AddStringCommand(wxString::Format("-break-insert -t %s:%d", filename.c_str(), line));
    }
}

void Debugger_DAP::Continue()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::Continue", dbg_DAP::LogPaneLogger::LineType::Debug);
    m_dapClient.Continue();
}

void Debugger_DAP::Next()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::Next", dbg_DAP::LogPaneLogger::LineType::Command);
    m_dapClient.Next();
}

void Debugger_DAP::NextInstruction()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "NextInstruction", dbg_DAP::LogPaneLogger::LineType::Command);
    // m_dapClient.Next();
}

void Debugger_DAP::StepIntoInstruction()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Functionality not supported yet!", dbg_DAP::LogPaneLogger::LineType::Command);
    //m_dapClient.StepIn();
}

void Debugger_DAP::Step()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "", dbg_DAP::LogPaneLogger::LineType::Error);
    m_dapClient.StepIn();
}

void Debugger_DAP::StepOut()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "", dbg_DAP::LogPaneLogger::LineType::Command);
    m_dapClient.StepOut();
}

void Debugger_DAP::Break()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, "Debugger_DAP::Break", dbg_DAP::LogPaneLogger::LineType::Command);
    m_dapClient.Pause();
}

void Debugger_DAP::Stop()
{
    if (!IsRunning())
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("stop debugger failed as not running!!!"), dbg_DAP::LogPaneLogger::LineType::Error);
        return;
    }

    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("stop debugger"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    DAPDebuggerResetData(dbg_DAP::ResetDataType::ResetData_Normal);
    UpdateDebugDialogs(false);
}

bool Debugger_DAP::IsRunning() const
{
    return (Debugger_State::IsRunning());
}

bool Debugger_DAP::IsStopped() const
{
    return (Debugger_State::IsStopped());
}

bool Debugger_DAP::IsBusy() const
{
    return (Debugger_State::IsBusy());
}

void Debugger_DAP::SetExitCode(int code)
{
    m_exit_code = code;
}

int Debugger_DAP::GetExitCode() const
{
    return m_exit_code;
}

//  "================================================================================================"
//  "      ____                          _                      _           _                        "
//  "     | __ )   _ __    ___    __ _  | | __  _ __     ___   (_)  _ __   | |_   ___                "
//  "     |  _ \  | '__|  / _ \  / _` | | |/ / | '_ \   / _ \  | | | '_ \  | __| / __|               "
//  "     | |_) | | |    |  __/ | (_| | |   <  | |_) | | (_) | | | | | | | | |_  \__ \               "
//  "     |____/  |_|     \___|  \__,_| |_|\_\ | .__/   \___/  |_| |_| |_|  \__| |___/               "
//  "                                          |_|                                                   "
//  "                                                                                                "
//  "================================================================================================"

int Debugger_DAP::GetBreakpointsCount() const
{
    return pDAPBreakpoints->GetBreakpointsCount();
}

cb::shared_ptr<cbBreakpoint> Debugger_DAP::GetBreakpoint(int index)
{
    return pDAPBreakpoints->GetBreakpoint(index);
}

cb::shared_ptr<const cbBreakpoint> Debugger_DAP::GetBreakpoint(int index) const
{
    return pDAPBreakpoints->GetBreakpoint(index);
}

cb::shared_ptr<cbBreakpoint> Debugger_DAP::AddBreakpoint(const wxString & filename, int line)
{
    return pDAPBreakpoints->AddBreakpoint(filename, line);
}

cb::shared_ptr<cbBreakpoint> Debugger_DAP::UpdateOrAddBreakpoint(const wxString & filename, const int line, const bool bEnable, const int id)
{
    return pDAPBreakpoints->UpdateOrAddBreakpoint(filename, line, bEnable, id);
}

void Debugger_DAP::DeleteBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint)
{
    pDAPBreakpoints->DeleteBreakpoint(breakpoint);
}

void Debugger_DAP::DeleteAllBreakpoints()
{
    pDAPBreakpoints->DeleteAllBreakpoints();
}

cb::shared_ptr<cbBreakpoint> Debugger_DAP::AddDataBreakpoint(const wxString & dataExpression)
{
    return pDAPBreakpoints->AddDataBreakpoint(dataExpression);
}

void Debugger_DAP::UpdateBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint)
{
    pDAPBreakpoints->UpdateBreakpoint(breakpoint);
}

bool Debugger_DAP::ShiftAllFileBreakpoints(const wxString & editorFilename, int startline, int lines)
{
    return pDAPBreakpoints->ShiftAllFileBreakpoints(editorFilename, startline, lines);
}

void Debugger_DAP::EnableBreakpoint(cb::shared_ptr<cbBreakpoint> breakpoint, bool bEnable)
{
    pDAPBreakpoints->EnableBreakpoint(breakpoint, bEnable);
}

// "=============================================================================================="
// "    __        __          _            _                                                      "
// "    \ \      / /   __ _  | |_    ___  | |__     ___   ___                                     "
// "     \ \ /\ / /   / _` | | __|  / __| | '_ \   / _ \ / __|                                    "
// "      \ V  V /   | (_| | | |_  | (__  | | | | |  __/ \__ \                                    "
// "       \_/\_/     \__,_|  \__|  \___| |_| |_|  \___| |___/                                    "
// "                                                                                              "
// "=============================================================================================="

void Debugger_DAP::UpdateDAPWatches(int updateType)
{
    pDAPWatches->UpdateDAPWatches(updateType);
}

cb::shared_ptr<cbWatch> Debugger_DAP::AddWatch(const wxString & symbol, bool update)
{
    return pDAPWatches->AddWatch(symbol, update);
}

cb::shared_ptr<cbWatch> Debugger_DAP::AddWatch(dbg_DAP::DAPWatch * watch, cb_unused bool update)
{
    return pDAPWatches->AddWatch(watch, update);
}

void Debugger_DAP::DeleteWatch(cb::shared_ptr<cbWatch> watch)
{
    pDAPWatches->DeleteWatch(watch);
}

bool Debugger_DAP::HasWatch(cb::shared_ptr<cbWatch> watch)
{
    return pDAPWatches->HasWatch(watch);
}

void Debugger_DAP::ShowWatchProperties(cb::shared_ptr<cbWatch> watch)
{
    pDAPWatches->ShowWatchProperties(watch);
}

bool Debugger_DAP::SetWatchValue(cb::shared_ptr<cbWatch> watch, const wxString & value)
{
    return pDAPWatches->SetWatchValue(watch, value);
}

void Debugger_DAP::ExpandWatch(cb::shared_ptr<cbWatch> watch)
{
    pDAPWatches->ExpandWatch(watch);
}

void Debugger_DAP::CollapseWatch(cb::shared_ptr<cbWatch> watch)
{
    pDAPWatches->CollapseWatch(watch);
}

void Debugger_DAP::UpdateWatch(cb_unused cb::shared_ptr<cbWatch> watch)
{
    pDAPWatches->UpdateWatch(watch);
}

void Debugger_DAP::DoWatches()
{
    pDAPWatches->DoWatches();
}

// "================================================================================================"
// "     __  __                                               ____                                  "
// "    |  \/  |   ___   _ __ ___     ___    _ __   _   _    |  _ \    __ _   _ __     __ _    ___  "
// "    | |\/| |  / _ \ | '_ ` _ \   / _ \  | '__| | | | |   | |_) |  / _` | | '_ \   / _` |  / _ \ "
// "    | |  | | |  __/ | | | | | | | (_) | | |    | |_| |   |  _ <  | (_| | | | | | | (_| | |  __/ "
// "    |_|  |_|  \___| |_| |_| |_|  \___/  |_|     \__, |   |_| \_\  \__,_| |_| |_|  \__, |  \___| "
// "                                                |___/                             |___/         "
// "================================================================================================"

cb::shared_ptr<cbWatch> Debugger_DAP::AddMemoryRange(uint64_t llAddress, uint64_t llSize, const wxString & symbol, bool update)
{
    return pDAPWatches->AddMemoryRange(llAddress, llSize, symbol, update);
}

// "===================================================================================================="
// "     ____    _____      _       ____   _  __       __    _____   ____       _      __  __   _____   "
// "    / ___|  |_   _|    / \     / ___| | |/ /      / /   |  ___| |  _ \     / \    |  \/  | | ____|  "
// "    \___ \    | |     / _ \   | |     | ' /      / /    | |_    | |_) |   / _ \   | |\/| | |  _|    "
// "     ___) |   | |    / ___ \  | |___  | . \     / /     |  _|   |  _ <   / ___ \  | |  | | | |___   "
// "    |____/    |_|   /_/   \_\  \____| |_|\_\   /_/      |_|     |_| \_\ /_/   \_\ |_|  |_| |_____|  "
// "                                                                                                    "
// "===================================================================================================="

int Debugger_DAP::GetStackFrameCount() const
{
    return pDAPCallStack->GetStackFrameCount();
}

cb::shared_ptr<const cbStackFrame> Debugger_DAP::GetStackFrame(int index) const
{
    return pDAPCallStack->GetStackFrame(index);
}

void Debugger_DAP::SwitchToFrame(int number)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("SwitchToFrame: %d"), number), dbg_DAP::LogPaneLogger::LineType::Debug);
    pDAPCallStack->SwitchToFrame(number);
}

int Debugger_DAP::GetActiveStackFrame() const
{
    return pDAPCallStack->GetActiveStackFrame();
}

// "==============================================================================================="
// "     _____   _                                 _                                               "
// "    |_   _| | |__    _ __    ___    __ _    __| |  ___                                         "
// "      | |   | '_ \  | '__|  / _ \  / _` |  / _` | / __|                                        "
// "      | |   | | | | | |    |  __/ | (_| | | (_| | \__ \                                        "
// "      |_|   |_| |_| |_|     \___|  \__,_|  \__,_| |___/                                        "
// "                                                                                               "
// "==============================================================================================="

int Debugger_DAP::GetThreadsCount() const
{
    return pDAPCallStack->GetThreadsCount();
}

cb::shared_ptr<const cbThread> Debugger_DAP::GetThread(int index) const
{
    return pDAPCallStack->GetThread(index);
}

bool Debugger_DAP::SwitchToThread(int thread_number)
{
    return pDAPCallStack->SwitchToThread(thread_number);
}

// "==============================================================================================="
// "     _____                   _     _____   _                                                   "
// "    |_   _|   ___     ___   | |   |_   _| (_)  _ __    ___                                     "
// "      | |    / _ \   / _ \  | |     | |   | | | '_ \  / __|                                    "
// "      | |   | (_) | | (_) | | |     | |   | | | |_) | \__ \                                    "
// "      |_|    \___/   \___/  |_|     |_|   |_| | .__/  |___/                                    "
// "                                              |_|                                              "
// ================================================================================================"

bool Debugger_DAP::ShowValueTooltip(int style)
{
    //    if (!m_pProcess || !IsStopped())
    return false;
    //    if (!m_State.HasDriver() || !m_State.GetDriver()->IsDebuggingStarted())
    //        return false;
    //
    //    if (!GetActiveConfigEx().GetFlag(DebuggerConfiguration::EvalExpression))
    //        return false;
    //    if (style != wxSCI_C_DEFAULT && style != wxSCI_C_OPERATOR && style != wxSCI_C_IDENTIFIER &&
    //        style != wxSCI_C_WORD2 && style != wxSCI_C_GLOBALCLASS  && style != wxSCI_C_WXSMITH &&
    //        style != wxSCI_F_IDENTIFIER)
    //    {
    //        return false;
    //    }
    //    return true;
}

void Debugger_DAP::OnValueTooltip(const wxString & token, const wxRect & evalRect)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    m_State.GetDriver()->EvaluateSymbol(token, evalRect);
}

void Debugger_DAP::AddTooltipWatch(const wxString & symbol, wxRect const & rect)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    cb::shared_ptr<dbg_DAP::DAPWatch> w(new dbg_DAP::DAPWatch(m_pProject, m_pLogger, symbol, true));
    //    m_watches.push_back(w);
    //
    //    if (IsRunning())
    //    {
    //        m_actions.Add(new dbg_DAP::DAPWatchCreateTooltipAction(w, m_watches, m_pLogger, rect));
    //    }
}

//void Debugger_DAP::OnValueTooltip(const wxString & token, const wxRect & evalRect)
//{
//    AddTooltipWatch(token, evalRect);
//}
//
//bool Debugger_DAP::ShowValueTooltip(int style)
//{
//    if (!IsRunning() || !IsStopped())
//    {
//        return false;
//    }
//
//    if (style != wxSCI_C_DEFAULT && style != wxSCI_C_OPERATOR && style != wxSCI_C_IDENTIFIER && style != wxSCI_C_WORD2)
//    {
//        return false;
//    }
//
//    return true;
//}

// "================================================================================================"
// "     ____                                                                                       "
// "    |  _ \   _ __    ___     ___    ___   ___   ___                                             "
// "    | |_) | | '__|  / _ \   / __|  / _ \ / __| / __|                                            "
// "    |  __/  | |    | (_) | | (__  |  __/ \__ \ \__ \                                            "
// "    |_|     |_|     \___/   \___|  \___| |___/ |___/                                            "
// "                                                                                                "
// "================================================================================================"


void Debugger_DAP::AttachToProcess(const wxString & pid)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    m_pProject = NULL;
    //    long number;
    //
    //    if (!pid.ToLong(&number))
    //    {
    //        return;
    //    }
    //
    //    LaunchDebugger(m_pProject,
    //                   GetActiveConfigEx().GetDebuggerExecutable(),
    //                   wxEmptyString,
    //                   wxEmptyString,
    //                   wxEmptyString,
    //                   number,
    //                   false,
    //                   StartTypeRun);
    //    m_executor.SetAttachedPID(number);
}

void Debugger_DAP::DetachFromProcess()
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Functionality not supported yet!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //    AddStringCommand(wxString::Format("-target-detach %ld", m_executor.GetAttachedPID()));
}

bool Debugger_DAP::IsAttachedToProcess() const
{
    // Gets called way too much, so message removed.
    //    return m_pid_attached != 0;
    return false;
}

// "================================================================================================"
// "     __  __   ___   ____     ____                                                               "
// "    |  \/  | |_ _| / ___|   / ___|                                                              "
// "    | |\/| |  | |  \___ \  | |                                                                  "
// "    | |  | |  | |   ___) | | |___                                                               "
// "    |_|  |_| |___| |____/   \____|                                                              "
// "                                                                                                "
// "================================================================================================"

void Debugger_DAP::ConvertDirectory(wxString & str, wxString base, bool relative)
{
    //    dbg_DAP::ConvertDirectory(str, base, relative);
}

void Debugger_DAP::RequestUpdate(DebugWindows window)
{
    if (!IsStopped())
    {
        return;
    }

    //    switch (window)
    //    {
    //        case Backtrace:
    //            {
    //                struct Switcher : dbg_DAP::DAPSwitchToFrameInvoker
    //                {
    //                    Switcher(Debugger_DAP * plugin, dbg_DAP::ActionsMap & actions) :
    //                        m_plugin(plugin),
    //                        m_actions(actions)
    //                    {
    //                    }
    //
    //                    virtual void Invoke(int frame_number)
    //                    {
    //                        typedef dbg_DAP::DAPSwitchToFrame<DAPSwitchToFrameNotification> SwitchType;
    //                        m_actions.Add(new SwitchType(frame_number, DAPSwitchToFrameNotification(m_plugin), false));
    //                    }
    //
    //                    Debugger_DAP * m_plugin;
    //                    dbg_DAP::ActionsMap & m_actions;
    //                };
    //                Switcher * switcher = new Switcher(this, m_actions);
    //                m_actions.Add(new dbg_DAP::DAPGenerateBacktrace(switcher, m_backtrace, m_current_frame, m_pLogger));
    //            }
    //            break;
    //
    //        case Threads:
    //            m_actions.Add(new dbg_DAP::DAPGenerateThreadsList(m_threads, m_current_frame.GetThreadId(), m_pLogger));
    //            break;
    //
    //        case CPURegisters:
    //            {
    //                m_actions.Add(new dbg_DAP::DAPGenerateCPUInfoRegisters(m_pLogger));
    //            }
    //            break;
    //
    //        case Disassembly:
    //            {
    //                wxString flavour = GetActiveConfigEx().GetDisassemblyFlavorCommand();
    //                m_actions.Add(new dbg_DAP::DAPDisassemble(flavour, m_pLogger));
    //            }
    //            break;
    //
    //        case ExamineMemory:
    //            {
    //                cbExamineMemoryDlg * dialog = Manager::Get()->GetDebuggerManager()->GetExamineMemoryDialog();
    //                wxString memaddress = dialog->GetBaseAddress();
    //
    //                // Check for blank memory string
    //                if (!memaddress.IsEmpty())
    //                {
    //                    m_actions.Add(new dbg_DAP::DAPGenerateExamineMemory(m_pLogger));
    //                }
    //            }
    //            break;
    //
    //        case MemoryRange:
    //            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("DebugWindows MemoryRange called!!"), dbg_DAP::LogPaneLogger::LineType::Error);
    //#ifdef __MINGW32__
    //            if (IsDebuggerPresent())
    //            {
    //                DebugBreak();
    //            }
    //#endif // __MINGW32__
    //            break;
    //
    //        case Watches:
    //            if (IsWindowReallyShown(Manager::Get()->GetDebuggerManager()->GetWatchesDialog()->GetWindow()))
    //            {
    //                DoWatches();
    //            }
    //            break;
    //
    //        default:
    //            break;
    //    }
}

void Debugger_DAP::StripQuotes(wxString & str)
{
    if ((str.GetChar(0) == '\"') && (str.GetChar(str.Length() - 1) == '\"'))
    {
        str = str.Mid(1, str.Length() - 2);
    }
}

void Debugger_DAP::DAPDebuggerResetData(dbg_DAP::ResetDataType bClearAllData)
{
    Debugger_State::SetState(Debugger_State::eDAPState::NotConnected);
    m_timer_poll_debugger.Stop();
    pDAPCallStack->DAPDebuggerResetData(bClearAllData);
    pDAPBreakpoints->DAPDebuggerResetData(bClearAllData);
    m_dapClient.Reset();

    if (m_dapPid != 0)
    {
        wxKill(m_dapPid);
        m_dapPid = 0;
    }

    ClearActiveMarkFromAllEditors();
    MarkAsStopped();
    // Notify debugger plugins for end of debug session
    PluginManager * plm = Manager::Get()->GetPluginManager();
    CodeBlocksEvent evt(cbEVT_DEBUGGER_FINISHED);
    plm->NotifyPlugins(evt);
    SwitchToPreviousLayout();
    cbCPURegistersDlg * pDialogCPURegisters = Manager::Get()->GetDebuggerManager()->GetCPURegistersDialog();

    if (pDialogCPURegisters)
    {
        pDialogCPURegisters->Clear();
    }

    cbDisassemblyDlg * pDialogDisassembly = Manager::Get()->GetDebuggerManager()->GetDisassemblyDialog();

    if (pDialogDisassembly)
    {
        cbStackFrame sf;
        pDialogDisassembly->Clear(sf);
    }
}

void Debugger_DAP::ConvertToDAPFriendly(wxString & str)
{
    if (str.IsEmpty())
    {
        return;
    }

    str = UnixFilename(str);

    while (str.Replace("\\", "/"))
        ;

    while (str.Replace("//", "/"))
        ;

    if ((str.Find(' ') != -1) && (str.GetChar(0) != '"'))
    {
        str = "\"" + str + "\"";
    }
}

void Debugger_DAP::ConvertToDAPDirectory(wxString & str, wxString base, bool relative)
{
    if (str.IsEmpty())
    {
        return;
    }

    ConvertToDAPFriendly(str);
    StripQuotes(str);

    if (!base.IsEmpty())
    {
        ConvertToDAPFriendly(base);
        StripQuotes(base);
    }

    if (platform::windows)
    {
        int  ColonLocation   = str.Find(':');
        bool convert_path_83 = false;

        if (ColonLocation != wxNOT_FOUND)
        {
            convert_path_83 = true;
        }
        else
        {
            if (!base.IsEmpty() && str.GetChar(0) != '/')
            {
                if (base.GetChar(base.Length()) == '/')
                {
                    base = base.Mid(0, base.Length() - 2);
                }

                while (!str.IsEmpty())
                {
                    base += "/" + str.BeforeFirst('/');

                    if (str.Find('/') != wxNOT_FOUND)
                    {
                        str = str.AfterFirst('/');
                    }
                    else
                    {
                        str.Clear();
                    }
                }

                convert_path_83 = true;
            }
        }

        // If can, get 8.3 name for path (Windows only)
        if (convert_path_83 && str.Contains(' ')) // only if has spaces
        {
            wxFileName fn(str); // might contain a file name, too
            wxString path_83 = fn.GetShortPath();

            if (!path_83.IsEmpty())
            {
                str = path_83; // construct filename again
            }
        }

        if (ColonLocation == wxNOT_FOUND || base.IsEmpty())
        {
            relative = false; // Can't do it
        }
    }
    else
    {
        if ((str.GetChar(0) != '/' && str.GetChar(0) != '~') || base.IsEmpty())
        {
            relative = false;
        }
    }

    if (relative)
    {
        if (platform::windows)
        {
            if (str.Find(':') != wxNOT_FOUND)
            {
                str = str.Mid(str.Find(':') + 2, str.Length());
            }

            if (base.Find(':') != wxNOT_FOUND)
            {
                base = base.Mid(base.Find(':') + 2, base.Length());
            }
        }
        else
        {
            if (str.GetChar(0) == '/')
            {
                str = str.Mid(1, str.Length());
            }
            else
            {
                if (str.GetChar(0) == '~')
                {
                    str = str.Mid(2, str.Length());
                }
            }

            if (base.GetChar(0) == '/')
            {
                base = base.Mid(1, base.Length());
            }
            else
            {
                if (base.GetChar(0) == '~')
                {
                    base = base.Mid(2, base.Length());
                }
            }
        }

        while (!base.IsEmpty() && !str.IsEmpty())
        {
            if (str.BeforeFirst('/') == base.BeforeFirst('/'))
            {
                if (str.Find('/') == wxNOT_FOUND)
                {
                    str.Clear();
                }
                else
                {
                    str = str.AfterFirst('/');
                }

                if (base.Find('/') == wxNOT_FOUND)
                {
                    base.Clear();
                }
                else
                {
                    base = base.AfterFirst('/');
                }
            }
            else
            {
                break;
            }
        }

        while (!base.IsEmpty())
        {
            str = "../" + str;

            if (base.Find('/') == wxNOT_FOUND)
            {
                base.Clear();
            }
            else
            {
                base = base.AfterFirst('/');
            }
        }
    }

    ConvertToDAPFriendly(str);
}


wxArrayString Debugger_DAP::ParseSearchDirs(cbProject * pProject)
{
    // NOTE: This is tinyXML as it interacts with the C::B SDK tinyXML code!!!!
    wxArrayString dirs;
    const TiXmlElement * elem = static_cast<const TiXmlElement *>(pProject->GetExtensionsNode());

    if (elem)
    {
        const TiXmlElement * conf = elem->FirstChildElement("debugger");

        if (conf)
        {
            const TiXmlElement * pathsElem = conf->FirstChildElement("search_path");

            while (pathsElem)
            {
                if (pathsElem->Attribute("add"))
                {
                    wxString dir = pathsElem->Attribute("add");

                    if (dirs.Index(dir) == wxNOT_FOUND)
                    {
                        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(dir); // apply env vars
                        ConvertToDAPDirectory(dir, "", false);
                        dirs.Add(dir);
                    }
                }

                pathsElem = pathsElem->NextSiblingElement("search_path");
            }
        }
    }

    return dirs;
}

//TiXmlElement* Debugger_DAP::GetElementForSaving(cbProject &project, const char *elementsToClear)
//{
//    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!
//
//    TiXmlElement *elem = static_cast<TiXmlElement*>(project.GetExtensionsNode());
//
//    // since rev4332, the project keeps a copy of the <Extensions> element
//    // and re-uses it when saving the project (so to avoid losing entries in it
//    // if plugins that use that element are not loaded atm).
//    // so, instead of blindly inserting the element, we must first check it's
//    // not already there (and if it is, clear its contents)
//    TiXmlElement* node = elem->FirstChildElement("debugger");
//    if (!node)
//        node = elem->InsertEndChild(TiXmlElement("debugger"))->ToElement();
//
//    for (TiXmlElement* child = node->FirstChildElement(elementsToClear);
//         child;
//         child = node->FirstChildElement(elementsToClear))
//    {
//        node->RemoveChild(child);
//    }
//    return node;
//}
//
//
//void Debugger_DAP::SetSearchDirs(cbProject &project, const wxArrayString &dirs)
//{
//    // NOTE: This is tinyXML as it interacts wtih teh C::B SDK tinyXML code!!!!
//
//    TiXmlElement* node = GetElementForSaving(project, "search_path");
//    if (dirs.GetCount() > 0)
//    {
//        for (size_t i = 0; i < dirs.GetCount(); ++i)
//        {
//            TiXmlElement* path = node->InsertEndChild(TiXmlElement("search_path"))->ToElement();
//            path->SetAttribute("add", cbU2C(dirs[i]));
//        }
//    }
//}

// "================================================================================================"
// "         ____       _     __     __  _____     ____    _____      _      _____   _____          "
// "        / ___|     / \    \ \   / / | ____|   / ___|  |_   _|    / \    |_   _| | ____|         "
// "        \___ \    / _ \    \ \ / /  |  _|     \___ \    | |     / _ \     | |   |  _|           "
// "         ___) |  / ___ \    \ V /   | |___     ___) |   | |    / ___ \    | |   | |___          "
// "        |____/  /_/   \_\    \_/    |_____|   |____/    |_|   /_/   \_\   |_|   |_____|         "
// "                                                                                                "
// "================================================================================================"

bool Debugger_DAP::SaveStateToFile(cbProject * pProject)
{
    //There are two types of state we should save:
    //1, breakpoints
    //2, watches
    //Create a file according to the m_pProject
    wxString projectFilename = pProject->GetFilename();

    if (projectFilename.IsEmpty())
    {
        return false;
    }

    //saved file name&extention
    wxFileName fname(projectFilename);
    fname.SetExt("bps");
    tinyxml2::XMLDocument doc;
    // doc.InsertEndChild(tinyxml2::XMLDeclaration("1.0", "UTF-8", "yes"));
    tinyxml2::XMLNode * rootnode = doc.InsertEndChild(doc.NewElement(XML_CFG_ROOT_TAG));

    if (!rootnode)
    {
        return false;
    }

    // ********************  Save debugger name ********************
    wxString compilerID = pProject->GetCompilerID();
    int compilerIdx = CompilerFactory::GetCompilerIndex(compilerID);
    Compiler * pCompiler = CompilerFactory::GetCompiler(compilerIdx);
    const CompilerPrograms & pCompilerProgsp = pCompiler->GetPrograms();
    tinyxml2::XMLNode * pCompilerNode = rootnode->InsertEndChild(doc.NewElement("CompilerInfo"));
    dbg_DAP::AddChildNode(pCompilerNode, "CompilerName", pCompiler->GetName());
    // dbg_DAP::AddChildNode(pCompilerNode, "C_Compiler", pCompilerProgsp.C);
    // dbg_DAP::AddChildNode(pCompilerNode, "CPP_Compiler",  pCompilerProgsp.CPP);
    // dbg_DAP::AddChildNode(pCompilerNode, "DynamicLinker_LD",  pCompilerProgsp.LD);
    // dbg_DAP::AddChildNode(pCompilerNode, "StaticLinker_LIB",  pCompilerProgsp.LIB);
    // dbg_DAP::AddChildNode(pCompilerNode, "Make",  pCompilerProgsp.MAKE);
    dbg_DAP::AddChildNode(pCompilerNode, "DBGconfig",  pCompilerProgsp.DBGconfig);
    // ******************** Save breakpoints ********************
    pDAPBreakpoints->SaveStateToFile(pProject);
    // ********************  Save Watches and Memory Range Watches ********************
    pDAPWatches->SaveStateToFile(pProject);
    // ********************  Save XML to disk ********************
    return doc.SaveFile(cbU2C(fname.GetFullPath()), false);
}

bool Debugger_DAP::LoadStateFromFile(cbProject * pProject)
{
    wxString projectFilename = pProject->GetFilename();

    if (projectFilename.IsEmpty())
    {
        return false;
    }

    wxFileName fname(projectFilename);
    fname.SetExt("bps");

    if (!fname.FileExists())
    {
        return false;
    }

    //Open XML file
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.LoadFile(cbU2C(fname.GetFullPath()));

    if (eResult != tinyxml2::XMLError::XML_SUCCESS)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Could not open the file '\%s\" due to the error: %s"), fname.GetFullPath(), doc.ErrorIDToName(eResult)), dbg_DAP::LogPaneLogger::LineType::Error);
        return false;
    }

    tinyxml2::XMLElement * root = doc.FirstChildElement(XML_CFG_ROOT_TAG);

    if (!root)
    {
        return false;
    }

    // ******************** Load breakpoints ********************
    pDAPBreakpoints->LoadStateFromFile(pProject);
    // ********************  Load Watches and Memory Range Watches ********************
    pDAPWatches->LoadStateFromFile(pProject);
    // ******************** Finished Load ********************
    return true;
}

// "======================================================================================================================="
// "                     ____       _      ____        _____  __     __  _____   _   _   _____   ____                      "
// "                    |  _ \     / \    |  _ \      | ____| \ \   / / | ____| | \ | | |_   _| / ___|                     "
// "                    | | | |   / _ \   | |_) |     |  _|    \ \ / /  |  _|   |  \| |   | |   \___ \                     "
// "                    | |_| |  / ___ \  |  __/      | |___    \ V /   | |___  | |\  |   | |    ___) |                    "
// "                    |____/  /_/   \_\ |_|         |_____|    \_/    |_____| |_| \_|   |_|   |____/                     "
// "                                                                                                                       "
// "     _____   _   _   _   _    ____   _____   ___    ___    _   _   ____      ____    _____      _      ____    _____   "
// "    |  ___| | | | | | \ | |  / ___| |_   _| |_ _|  / _ \  | \ | | / ___|    / ___|  |_   _|    / \    |  _ \  |_   _|  "
// "    | |_    | | | | |  \| | | |       | |    | |  | | | | |  \| | \___ \    \___ \    | |     / _ \   | |_) |   | |    "
// "    |  _|   | |_| | | |\  | | |___    | |    | |  | |_| | | |\  |  ___) |    ___) |   | |    / ___ \  |  _ <    | |    "
// "    |_|      \___/  |_| \_|  \____|   |_|   |___|  \___/  |_| \_| |____/    |____/    |_|   /_/   \_\ |_| \_\   |_|    "
// "                                                                                                                       "
// "======================================================================================================================="

void Debugger_DAP::OnLaunchResponse(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    // Check that the debugee was started successfully
    dap::LaunchResponse * resp = event.GetDapResponse()->As<dap::LaunchResponse>();

    if (resp)
    {
        if (resp->success)
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Good Response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Bad Response: %s"), resp->message), dbg_DAP::LogPaneLogger::LineType::Error);
            // launch failed!
            wxMessageBox("Failed to launch debuggee: " + resp->message, "DAP", wxICON_ERROR | wxOK | wxOK_DEFAULT | wxCENTRE);
            // Reset plugin back to default state!!!!
            DAPDebuggerResetData(dbg_DAP::ResetDataType::ResetData_Normal);
        }
    }
}

/// DAP server responded to our `initialize` request
void Debugger_DAP::OnInitializedEvent(DAPEvent & event)
{
    // got initialized event, place breakpoints and continue
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    pDAPBreakpoints->OnInitializedEvent(event);
    pDAPWatches->OnInitializedEvent(event);
    // Let DAP server know that we have completed the configuration required
    m_dapClient.ConfigurationDone();
}

#define SHOW_RESPONSE_DATA(msg, OptType, variable)                              \
    if (!OptType.IsNull())                                          \
    {                                                               \
        m_pLogger->LogDAPMsgType("", -1,                            \
                                 wxString::Format(msg, OptType.As<bool>() ?"True":"False"),  \
                                 dbg_DAP::LogPaneLogger::LineType::UserDisplay);             \
        variable =  OptType.As<bool>();                             \
    }

/// DAP server sent `initialize` reponse to our `initialize` message
void Debugger_DAP::OnInitializeResponse(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    pDAPBreakpoints->OnInitializeResponse(event);
    dap::InitializeResponse * response_data = event.GetDapResponse()->As<dap::InitializeResponse>();

    if (response_data)
    {
        if (response_data->success)
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got OnInitialize Response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
            SHOW_RESPONSE_DATA(_("supportsBreakpointLocationsRequest: %s"), response_data->capabilities.supportsBreakpointLocationsRequest, supportsBreakpointLocationsRequest);
            SHOW_RESPONSE_DATA(_("supportsCancelRequest: %s"), response_data->capabilities.supportsCancelRequest, supportsCancelRequest);
            SHOW_RESPONSE_DATA(_("supportsClipboardContext: %s"), response_data->capabilities.supportsClipboardContext, supportsClipboardContext);
            SHOW_RESPONSE_DATA(_("supportsCompletionsRequest: %s"), response_data->capabilities.supportsCompletionsRequest, supportsCompletionsRequest);
            SHOW_RESPONSE_DATA(_("supportsConditionalBreakpoints: %s"), response_data->capabilities.supportsConditionalBreakpoints, supportsConditionalBreakpoints);
            SHOW_RESPONSE_DATA(_("supportsConfigurationDoneRequest: %s"), response_data->capabilities.supportsConfigurationDoneRequest, supportsConfigurationDoneRequest);
            SHOW_RESPONSE_DATA(_("supportsDataBreakpoints: %s"), response_data->capabilities.supportsDataBreakpoints, supportsDataBreakpoints);
            SHOW_RESPONSE_DATA(_("supportsDelayedStackTraceLoading: %s"), response_data->capabilities.supportsDelayedStackTraceLoading, supportsDelayedStackTraceLoading);
            SHOW_RESPONSE_DATA(_("supportsDisassembleRequest: %s"), response_data->capabilities.supportsDisassembleRequest, supportsDisassembleRequest);
            SHOW_RESPONSE_DATA(_("supportsEvaluateForHovers: %s"), response_data->capabilities.supportsEvaluateForHovers, supportsEvaluateForHovers);
            SHOW_RESPONSE_DATA(_("supportsExceptionFilterOptions: %s"), response_data->capabilities.supportsExceptionFilterOptions, supportsExceptionFilterOptions);
            SHOW_RESPONSE_DATA(_("supportsExceptionInfoRequest: %s"), response_data->capabilities.supportsExceptionInfoRequest, supportsExceptionInfoRequest);
            SHOW_RESPONSE_DATA(_("supportsExceptionOptions: %s"), response_data->capabilities.supportsExceptionOptions, supportsExceptionOptions);
            SHOW_RESPONSE_DATA(_("supportsFunctionBreakpoints: %s"), response_data->capabilities.supportsFunctionBreakpoints, supportsFunctionBreakpoints);
            SHOW_RESPONSE_DATA(_("supportsGotoTargetsRequest: %s"), response_data->capabilities.supportsGotoTargetsRequest, supportsGotoTargetsRequest);
            SHOW_RESPONSE_DATA(_("supportsHitConditionalBreakpoints: %s"), response_data->capabilities.supportsHitConditionalBreakpoints, supportsHitConditionalBreakpoints);
            SHOW_RESPONSE_DATA(_("supportsInstructionBreakpoints: %s"), response_data->capabilities.supportsInstructionBreakpoints, supportsInstructionBreakpoints);
            SHOW_RESPONSE_DATA(_("supportsLoadedSourcesRequest: %s"), response_data->capabilities.supportsLoadedSourcesRequest, supportsLoadedSourcesRequest);
            SHOW_RESPONSE_DATA(_("supportsLogPoints: %s"), response_data->capabilities.supportsLogPoints, supportsLogPoints);
            SHOW_RESPONSE_DATA(_("supportsModulesRequest: %s"), response_data->capabilities.supportsModulesRequest, supportsModulesRequest);
            //SHOW_RESPONSE_DATA(_("supportsProgressReporting: %s"), response_data->capabilities.supportsProgressReporting , supportsProgressReporting);
            SHOW_RESPONSE_DATA(_("supportsReadMemoryRequest: %s"), response_data->capabilities.supportsReadMemoryRequest, supportsReadMemoryRequest);
            SHOW_RESPONSE_DATA(_("supportsRestartFrame: %s"), response_data->capabilities.supportsRestartFrame, supportsRestartFrame);
            SHOW_RESPONSE_DATA(_("supportsRestartRequest: %s"), response_data->capabilities.supportsRestartRequest, supportsRestartRequest);
            //SHOW_RESPONSE_DATA(_("supportsRunInTerminalRequest: %s"), response_data->capabilities.supportsRunInTerminalRequest, supportsRunInTerminalRequest);
            SHOW_RESPONSE_DATA(_("supportsSetExpression: %s"), response_data->capabilities.supportsSetExpression, supportsSetExpression);
            SHOW_RESPONSE_DATA(_("supportsSetVariable: %s"), response_data->capabilities.supportsSetVariable, supportsSetVariable);
            SHOW_RESPONSE_DATA(_("supportsSingleThreadExecutionRequests: %s"), response_data->capabilities.supportsSingleThreadExecutionRequests, supportsSingleThreadExecutionRequests);
            SHOW_RESPONSE_DATA(_("supportsStepBack: %s"), response_data->capabilities.supportsStepBack, supportsStepBack);
            SHOW_RESPONSE_DATA(_("supportsStepInTargetsRequest: %s"), response_data->capabilities.supportsStepInTargetsRequest, supportsStepInTargetsRequest);
            SHOW_RESPONSE_DATA(_("supportsSteppingGranularity: %s"), response_data->capabilities.supportsSteppingGranularity, supportsSteppingGranularity);
            SHOW_RESPONSE_DATA(_("supportsTerminateRequest: %s"), response_data->capabilities.supportsTerminateRequest, supportsTerminateRequest);
            SHOW_RESPONSE_DATA(_("supportsTerminateThreadsRequest: %s"), response_data->capabilities.supportsTerminateThreadsRequest, supportsTerminateThreadsRequest);
            SHOW_RESPONSE_DATA(_("supportSuspendDebuggee: %s"), response_data->capabilities.supportSuspendDebuggee, supportSuspendDebuggee);
            SHOW_RESPONSE_DATA(_("supportsValueFormattingOptions: %s"), response_data->capabilities.supportsValueFormattingOptions, supportsValueFormattingOptions);
            SHOW_RESPONSE_DATA(_("supportsWriteMemoryRequest: %s"), response_data->capabilities.supportsWriteMemoryRequest, supportsWriteMemoryRequest);
            SHOW_RESPONSE_DATA(_("supportTerminateDebuggee: %s"), response_data->capabilities.supportTerminateDebuggee, supportTerminateDebuggee);

            if (!response_data->capabilities.completionTriggerCharacters.IsNull())
            {
                // The set of characters that should trigger completion in a REPL. If not specified, the UI should assume the '.' character.
                vCompletionTriggerCharacters = response_data->capabilities.completionTriggerCharacters.As<std::vector<wxString>>();

                for (std::vector<wxString>::iterator it = vCompletionTriggerCharacters.begin(); it != vCompletionTriggerCharacters.end(); ++it)
                {
                    m_pLogger->LogDAPMsgType("", -1, wxString::Format(_("completionTriggerCharacters - %s"), *it), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
                }
            }

            if (!response_data->capabilities.additionalModuleColumns.IsNull())
            {
                // The set of additional module information exposed by the debug adapter.
                vAdditionalModuleColumns = response_data->capabilities.additionalModuleColumns.As<std::vector<dap::ColumnDescriptor>>();
                m_pLogger->LogDAPMsgType("", -1, wxString::Format(_("additionalModuleColumns - future display data ")), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
            }

            m_dapClient.Launch(std::move(m_DAP_DebuggeeStartCMD), UnixFilename(m_pProject->GetBasePath()));
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got BAD OnInitialize Response"), dbg_DAP::LogPaneLogger::LineType::Error);
        }
    }
}


void Debugger_DAP::OnConfigurationDoneResponse(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

/// DAP server stopped. This can happen for multiple reasons:
/// - exception
/// - breakpoint hit
/// - step (user previously issued `Next` command)
void Debugger_DAP::OnStopped(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    Debugger_State::SetState(Debugger_State::eDAPState::Stopped);
    // got stopped event
    dap::StoppedEvent * stopped_data = event.GetDapEvent()->As<dap::StoppedEvent>();

    if (stopped_data)
    {
        //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("text: %s"),  stopped_data->text), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("description: %s"),  stopped_data->description), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("All threads stopped: %s"),  stopped_data->allThreadsStopped ? "True" : "False"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Stopped thread ID: %d (active thread ID: %d)"), stopped_data->threadId, m_dapClient.GetActiveThreadId()), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        if (
            (stopped_data->reason.IsSameAs("breakpoint")) ||
            (stopped_data->reason.IsSameAs("step")) ||
            (stopped_data->reason.IsSameAs("goto"))
        )
        {
            /* reason:
            * Values: 'step', 'breakpoint', 'goto',
            */
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Stopped reason: %s"), stopped_data->reason), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
            pDAPBreakpoints->OnProcessBreakpointData(stopped_data->description);
        }
        else
        {
            if (stopped_data->reason.IsSameAs("exception"))
            {
                /* reason:
                * Values: 'exception',
                */
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Stopped reason: %s"), stopped_data->reason), dbg_DAP::LogPaneLogger::LineType::Error);
            }
            else
            {
                /* reason:
                * Values: 'pause', 'entry', 'function breakpoint', 'data breakpoint', etc
                */
                m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Stopped reason: %s not suported"), stopped_data->reason), dbg_DAP::LogPaneLogger::LineType::Error);
            }

            m_dapClient.GetFrames();
        }
    }

    MarkAsStopped();
}

/// Received a response to `GetFrames()` call
void Debugger_DAP::OnScopes(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    pDAPWatches->OnScopes(event);
}

void Debugger_DAP::OnVariables(DAPEvent & event)
{
    // m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    pDAPWatches->OnVariables(event);
}

/// Received a response to `GetFrames()` call
void Debugger_DAP::OnStackTrace(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    pDAPCallStack->OnStackTrace(event);
    dap::StackTraceResponse * stack_trace_data = event.GetDapResponse()->As<dap::StackTraceResponse>();

    if (stack_trace_data)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received stack trace event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        bool bFileFound = false;

        for (const auto & stack : stack_trace_data->stackFrames)
        {
#ifdef DAP_DEBUG_ENABLE
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__,
                                     __LINE__,
                                     wxString::Format(_("Stack: ID 0x%X , Name: %s , File: %s  %d"),
                                                      stack.id,
                                                      stack.name,
                                                      stack.source.path.IsEmpty() ? stack.source.name : stack.source.path,
                                                      stack.line
                                                     ),
                                     dbg_DAP::LogPaneLogger::LineType::UserDisplay);
#endif
            wxString sFileName = stack.source.path;

            if (!sFileName.IsEmpty() && !bFileFound)
            {
                wxFileName fnFileName(sFileName);

                if (fnFileName.Exists())
                {
                    bFileFound = true;
                    int lineNumber = stack.line;
                    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("SyncEditor: %s %d"), sFileName, lineNumber), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
                    SyncEditor(sFileName, lineNumber, true);
                    // request the scopes for the first stack
                    m_dapClient.GetScopes(stack.id);
                }
            }
        }
    }
}

/// Debuggee process exited, print the exit code
void Debugger_DAP::OnExited(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Debuggee exited. Exit code: %d"), event.GetDapEvent()->As<dap::ExitedEvent>()->exitCode));
    Debugger_State::SetState(Debugger_State::eDAPState::NotConnected);
}

/// Debug session terminated
void Debugger_DAP::OnTerminated(DAPEvent & event)
{
    wxUnusedVar(event);
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event for debugger terminated!"), dbg_DAP::LogPaneLogger::LineType::Warning);
    // Reset the client and data
    DAPDebuggerResetData(dbg_DAP::ResetDataType::ResetData_Normal);
}

void Debugger_DAP::OnOutput(DAPEvent & event)
{
    dap::OutputEvent * output_data = event.GetDapEvent()->As<dap::OutputEvent>();

    if (output_data)
    {
        wxString msg(output_data->output);
        msg.Replace("\r\n", "");
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format("%s - %s", output_data->category, msg), dbg_DAP::LogPaneLogger::LineType::UserDisplay);

        if (m_pTerminalMgr)
        {
            // The output category. If not specified, 'console' is assumed.
            // Values: 'console', 'stdout', 'stderr', 'telemetry', etc.
            m_pTerminalMgr->AppendString(output_data->output);
        }
    }
    else
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    }
}

void Debugger_DAP::OnBreakpointLocations(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::BreakpointLocationsResponse * d = event.GetDapResponse()->As<dap::BreakpointLocationsResponse>();

    if (d)
    {
        m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("==> Breakpoints:\n"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);

        for (const auto & bp : d->breakpoints)
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _(d->filepath << ":" << bp.line), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
    }
}

void Debugger_DAP::OnConnectionError(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("OnConnectionError!")), dbg_DAP::LogPaneLogger::LineType::Error);
    wxMessageBox(_("Lost connection to dap server"));
    event.Skip();
    DAPDebuggerResetData(dbg_DAP::ResetDataType::ResetData_Normal);
}

void Debugger_DAP::OnBreakpointDataSet(DAPEvent & event)
{
    //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    pDAPBreakpoints->OnBreakpointDataSet(event);
}

void Debugger_DAP::OnBreakpointFunctionSet(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received response"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::SetBreakpointsResponse * resp = event.GetDapResponse()->As<dap::SetBreakpointsResponse>();

    if (resp)
    {
        if (resp->success)
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got OnBreakpointFunctionSet event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
        }
        else
        {
            m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Got BAD OnBreakpointFunctionSet Response"), dbg_DAP::LogPaneLogger::LineType::Error);
        }
    }
}

void Debugger_DAP::OnDapLog(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, event.GetString(), dbg_DAP::LogPaneLogger::LineType::Debug);
}

void Debugger_DAP::OnDapModuleEvent(DAPEvent & event)
{
    // m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    dap::ModuleEvent * event_data = event.GetDapEvent()->As<dap::ModuleEvent>();

    if (event_data)
    {
        //wxString log_entry;
        //log_entry << event_data->module.id << ": " << event_data->module.name << " " << event_data->module.symbolStatus << event_data->module.version << " " << event_data->module.path;
        //m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, log_entry, dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    }
}

void Debugger_DAP::OnRunInTerminalRequest(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Handling `OnRunInTerminalRequest` event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
    auto request = event.GetDapRequest()->As<dap::RunInTerminalRequest>();

    if (!request)
    {
        return;
    }

    wxString command;

    for (const wxString & cmd : request->arguments.args)
    {
        command << cmd << " ";
    }

    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, wxString::Format(_("Starting process: %s"), command), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
#if 1
    wxString consoleTty;
    int m_DAPTerminalProcessID = RunNixConsole(consoleTty);
#else
    m_DAPTerminalProcess = dap::ExecuteProcess(command);
#endif
    dap::RunInTerminalResponse response = m_dapClient.MakeRequest<dap::RunInTerminalResponse>();
    response.request_seq = request->seq;

    //    if (!m_DAPTerminalProcess)
    if (m_DAPTerminalProcessID == 0)
    {
        response.success = false;
        //        response.processId = m_DAPTerminalProcessID;
    }
    else
    {
        response.success = true;
        //        response.processId = m_DAPTerminalProcess->GetProcessId();
    }

    response.processId = m_DAPTerminalProcessID;
    m_dapClient.SendResponse(response);
}

void Debugger_DAP::OnThreadResponse(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

void Debugger_DAP::OnStopOnEntryEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

void Debugger_DAP::OnProcessEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

void Debugger_DAP::OnBreakpointEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

void Debugger_DAP::OnContinuedEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

void Debugger_DAP::OnDebugPYWaitingForServerEvent(DAPEvent & event)
{
    m_pLogger->LogDAPMsgType(__PRETTY_FUNCTION__, __LINE__, _("Received event"), dbg_DAP::LogPaneLogger::LineType::UserDisplay);
}

// "==================================================================================================================="
// "          ____       _      ____      _____  __     __  _____   _   _   _____   ____      _____   _   _   ____     "
// "         |  _ \     / \    |  _ \    | ____| \ \   / / | ____| | \ | | |_   _| / ___|    | ____| | \ | | |  _ \    "
// "         | | | |   / _ \   | |_) |   |  _|    \ \ / /  |  _|   |  \| |   | |   \___ \    |  _|   |  \| | | | | |   "
// "         | |_| |  / ___ \  |  __/    | |___    \ V /   | |___  | |\  |   | |    ___) |   | |___  | |\  | | |_| |   "
// "         |____/  /_/   \_\ |_|       |_____|    \_/    |_____| |_| \_|   |_|   |____/    |_____| |_| \_| |____/    "
// "                                                                                                                   "
// "==================================================================================================================="


// Windows: C:\msys64\mingw64\bin\lldb-vscode.exe -port 12345
// Linux:  /usr/bin/lldb-vscode-14 -port 12345
// MACOS:: /usr/local/opt/llvm/bin/lldb-vscode -port 12345
// --personality=debuging --multiple-instance
