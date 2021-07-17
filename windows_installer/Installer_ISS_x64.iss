; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define CB_SrcDir         =SourcePath+'\..'
#define CB_BuildOutputDir =SourcePath+'\..\src\output31_64'
#define CB_VERSION        ='SVN_12487'
#define CB_PROGRAMDIRNAME ='CodeBlocks_TEST'
#define CURRENT_DATE      = GetDateTimeString('ddmmmyyyy', '', '')

#define DICTIONARIES_DIR  =SourcePath+'\Dictionaries'
#define DOCUMENTATION_DIR =SourcePath+'Documentation'
#define GRAPHICS_DIR      =SourcePath+'Graphics'
#define LICENSES_DIR      =SourcePath+'Licenses'
#define URLS_DIR          =SourcePath+'URLS'

[Setup]
AppName=Code::Blocks
AppVerName=Code::Blocks {#CB_VERSION}
AppPublisher=Code::Blocks
DefaultDirName={pf}\{#CB_PROGRAMDIRNAME}
DefaultGroupName={#CB_PROGRAMDIRNAME}
LicenseFile={#LICENSES_DIR}\gpl-3.0.txt
OutputBaseFilename=CodeBlocks-{#CB_VERSION}
AppPublisherURL=www.codeblocks.org
AppVersion={#CB_VERSION} {#CURRENT_DATE}
UninstallDisplayIcon={app}\codeblocks.exe
UninstallDisplayName=Code::Blocks
ShowLanguageDialog=no
AppSupportURL=www.codeblocks.org
AppUpdatesURL=www.codeblocks.org
;InfoAfterFile=.\COMPILERS_win32.rtf
InternalCompressLevel=ultra
SolidCompression=true
Compression=lzma/ultra
InfoBeforeFile=
;VersionInfoVersion={#CB_VERSION}
VersionInfoCompany=Code::Blocks
VersionInfoCopyright=(C) Code::Blocks
VersionInfoDescription=Code::Blocks IDE
VersionInfoProductName=Code::Blocks IDE
;VersionInfoProductVersion={#CB_VERSION}
VersionInfoProductTextVersion={#CB_VERSION}
SourceDir={#CB_BuildOutputDir}
OutputDir={#SourcePath}\Installer_Output
DisableStartupPrompt=False
DisableWelcomePage=False
PrivilegesRequired=none
WizardImageFile={#GRAPHICS_DIR}\setup_1.bmp
WizardSmallImageFile={#GRAPHICS_DIR}\setup_logo_2003.bmp
WizardImageStretch=yes
SetupIconFile={#GRAPHICS_DIR}\setup_icon.ico

[Tasks]
Name: startmenu; Description: "Create a &startmenu entry"; GroupDescription: "Additional icons:"; Components: main
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Components: main
Name: desktopicon\common; Description: "For all users"; GroupDescription: "Additional icons:"; Components: main; Flags: exclusive
Name: desktopicon\user; Description: "For the current user only"; GroupDescription: "Additional icons:"; Components: main; Flags: exclusive unchecked
Name: quicklaunchicon; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Components: main; Flags: unchecked


[Files]
Source: "*"; DestDir: "{app}"; Flags: ignoreversion createallsubdirs recursesubdirs
Source: "{#URLS_DIR}\CodeBLocks_Forums.url"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#URLS_DIR}\CodeBLocks_ReportBugs.url"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#URLS_DIR}\CodeBLocks_WebSite.url"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#URLS_DIR}\CodeBLocks_Wiki.url"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#URLS_DIR}\Download_DMC.url"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#URLS_DIR}\Download_MinGW.url"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#URLS_DIR}\Download_OW.url"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#URLS_DIR}\Download_OW_V2_Fork.url"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: {group}\{#CB_PROGRAMDIRNAME}; Filename: {app}\codeblocks.exe; IconIndex: 0; WorkingDir: {app}; Comment: Code::Blocks IDE; Tasks: startmenu;
Name: {group}\{cm:UninstallProgram, codeblocks}; Filename: {uninstallexe}; Tasks: startmenu;
Name: {userdesktop}\{#CB_PROGRAMDIRNAME}; Filename: {app}\codeblocks.exe; IconIndex: 0; WorkingDir: {app}; Comment: Code::Blocks IDE; Tasks: desktopicon;
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#CB_PROGRAMDIRNAME}; Filename: {app}\codeblocks.exe; IconIndex: 0; WorkingDir: {app}; Comment: Code::Blocks IDE; Tasks: quicklaunchicon;
Name: {group}\Links\CodeBlocks on-line; Filename: {app}\CodeBLocks_WebSite.url; Comment: Go to Code::Blocks IDE website; Tasks: startmenu;
Name: {group}\Links\CodeBlocks forums; Filename: {app}\CodeBLocks_Forums.url; Comment: Go to Code::Blocks IDE discussion forums; Tasks: startmenu;
Name: {group}\Links\CodeBlocks WiKi; Filename: {app}\CodeBLocks_Wiki.url; Comment: Go to Code::Blocks IDE WiKi site; Tasks: startmenu;
Name: {group}\Links\Report a bug; Filename: {app}\CodeBLocks_ReportBugs.url; Comment: Report bugs you have found in Code::Blocks; Tasks: startmenu;
Name: {group}\Docs\License; Filename: {app}\gpl-3.0.txt; Tasks: startmenu;

;Name: {group}\Links\Download MinGW Compiler & Debugger; Filename: {app}\Download_MINGW.url; Comment: Download the MinGW distribution which contains the GNU GCC compiler and GDB debugger; Components: ProgramFiles
;Name: {group}\Links\Download Microsoft Visual C++ Free Toolkit 2003; Filename: {app}\Download_MSVC2003.url; Comment: Download the Microsoft Visual C++ Free Toolkit 2003; Components: ProgramFiles
;Name: {group}\Links\Download Borland C++ Compiler 5.5; Filename: {app}\Download_BCC55.url; Comment: Download the Borland C++ Compiler 5.5; Components: ProgramFiles
;Name: {group}\Links\Download Digital Mars Compiler; Filename: {app}\Download_DMC.url; Comment: Download the Digital Mars free compiler; Components: ProgramFiles
;Name: {group}\Links\Download OpenWatcom; Filename: {app}\Download_OW.url; Comment: Download the OpenWatcom portable compiler; Components: ProgramFiles
;Name: {group}\Docs\How to install a compiler; Filename: {app}\COMPILERS_win32.rtf

[Run]
Filename: {app}\codeblocks.exe; Description: Launch Code::Blocks; Flags: nowait postinstall skipifsilent

[Components]
Name: main; Description: "Main Files"; Types: full compact custom; Flags: fixed
Name: help; Description: "Help Files"; Types: full
Name: help\english; Description: "English"; Types: full
Name: help\french; Description: "French"; Types: full
;Name: ProgramFiles; Description: Required program files; Types: custom compact full; Flags: fixed
;Name: ProgramFiles/Templates; Description: Project templates; Types: custom full

Name: Lexers; Description: Code::Blocks Lexers; Types: custom full
Name: Lexers\C_CPP; Description: C C++ Lexer; Types: custom full
Name: Lexers\ADA; Description: ADA Lexer; Types: custom full
Name: Lexers\D; Description: The D Language; Types: custom full
Name: Lexers\Fortran; Description: Fortran lexer; Types: custom full
Name: Lexers\Java; Description: Java Lexer; Types: custom full
Name: Lexers\JavaScript; Description: JavaScript Lexer; Types: custom full
Name: Lexers\Objective_C; Description: Objective-C Lexer; Types: custom full
Name: Lexers\Pascal; Description: Pascal Lexer; Types: custom full
Name: Lexers\Smalltalk; Description: Smalltalk Lexer; Types: custom full

Name: ScriptLanguages; Description: Script Language; Types: custom full
Name: ScriptLanguages\Angelscript; Description: Angelscript Script Language; Types: custom full
Name: ScriptLanguages\AutoTools; Description: AutoTools Script Language; Types: custom full
Name: ScriptLanguages\Caml; Description: Caml Language; Types: custom full
Name: ScriptLanguages\Coffee; Description: Coffee Script Language; Types: custom full
Name: ScriptLanguages\GameMonkey; Description: Game Monkey Script Language; Types: custom full
Name: ScriptLanguages\Haskell; Description: Haskell Script Language; Types: custom full
Name: ScriptLanguages\Lisp; Description: Lisp Script Language; Types: custom full
Name: ScriptLanguages\Lua; Description: Lua Script Language; Types: custom full
Name: ScriptLanguages\Nim; Description: Nim Script Language; Types: custom full
Name: ScriptLanguages\Perl; Description: Perl Script Language; Types: custom full
Name: ScriptLanguages\Python; Description: Python Script Language; Types: custom full
Name: ScriptLanguages\Ruby; Description: Ruby Script Language; Types: custom full
Name: ScriptLanguages\Squirrel; Description: Squirrel Script Language; Types: custom full
Name: ScriptLanguages\VBScript; Description: VB Script Script Language; Types: custom full

Name: MarkupLanguages; Description: Markup Languages; Types: custom full
Name: MarkupLanguages\BiBTeX; Description: BiBTeX Markup Languages; Types: custom full
Name: MarkupLanguages\CSS; Description: CSS Markup Languages; Types: custom full
Name: MarkupLanguages\HTML; Description: HTML Markup Languages; Types: custom full
Name: MarkupLanguages\LaTeX; Description: LaTeX Markup Languages; Types: custom full
Name: MarkupLanguages\Markdown; Description: Markdown Markup Languages; Types: custom full
Name: MarkupLanguages\XML; Description: XML Markup Languages; Types: custom full
Name: MarkupLanguages\YAML; Description: YAML Markup Languages; Types: custom full

Name: GraphicsProgramming; Description: Graphics Programming; Types: custom full
Name: GraphicsProgramming\CUDA; Description: CUDA Graphics Programming; Types: custom full
Name: GraphicsProgramming\GLSL_GLslang; Description: GLSL (GLslang) Graphics Programming; Types: custom full
Name: GraphicsProgramming\nVidia_Cg; Description: nVidia Cg Graphics Programming; Types: custom full
Name: GraphicsProgramming\Ogre; Description: Ogre Graphics Programming; Types: custom full


Name: EmbeddedDevelopment; Description: Embedded Development; Types: custom full
Name: EmbeddedDevelopment\A68k_Assembler; Description: A68k Assembler Embedded Development; Types: custom full
Name: EmbeddedDevelopment\Hitachi_Assembler; Description: Hitachi Assembler Embedded Development; Types: custom full
Name: EmbeddedDevelopment\Intel_HEX; Description: Intel HEX Embedded Development; Types: custom full
Name: EmbeddedDevelopment\Motorola_SRecord; Description: Motorola S-Record Embedded Development; Types: custom full
Name: EmbeddedDevelopment\Verilog; Description: Verilog Embedded Development; Types: custom full
Name: EmbeddedDevelopment\VHDL; Description: VHDL Embedded Development; Types: custom full

Name: Shell; Description: Shell \ Binutils; Types: custom full
Name: Shell\bash_script; Description: bash script ; Types: custom full
Name: Shell\DOS_batch_files; Description: DOS batch files ; Types: custom full
Name: Shell\Windows_registry_file; Description: Windows registry file ; Types: custom full
Name: Shell\Cmake; Description: Cmake ; Types: custom full
Name: Shell\diff; Description: diff ; Types: custom full
Name: Shell\Makefile; Description: Makefile ; Types: custom full
Name: Shell\PowerShell; Description: PowerShell ; Types: custom full

Name: Others; Description: Others ; Types: custom full
Name: Others\Google_Protocol_Buffer; Description: Google Protocol Buffer ; Types: custom full
Name: Others\InnoSetup; Description: InnoSetup ; Types: custom full
Name: Others\MASM; Description: MASM ; Types: custom full
Name: Others\Matlab; Description: Matlab ; Types: custom full
Name: Others\NSIS_installer_script; Description: NSIS installer script ; Types: custom full
Name: Others\Plain_file; Description: Plain file ; Types: custom full
Name: Others\Property_file; Description: Property file ; Types: custom full
Name: Others\Sql; Description: Sql ; Types: custom full
Name: Others\XBase; Description: XBase ; Types: custom full


Name: Plugins; Description: Code::Blocks plugins; Types: custom full
Name: Plugins\Contrib;Description: Contributed Plugins; Types: custom compact full
Name: Plugins\Contrib\Auto_Versioning;Description: Auto Versioning plugin; Types: custom compact full
Name: Plugins\Contrib\Browse_Tracker;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Byo_Games;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Cccc;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\CodeStat; Description: Code staticstics (lines of code, comments, etc); Types: custom full
Name: Plugins\Contrib\Code_Snippets;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Copy_Strings;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\CppCheck;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Cscope;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\DevPak; Description: Dev-C++ DevPaks support; Types: custom full
Name: Plugins\Contrib\DoxyBlocks;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Drag_Scroll;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\EditorConfig;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Editor_Tweaks;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\EnvVars;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Exporter;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\File_Manager;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Fortran_Project;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\HeaderFixUp;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\HelpPlugin; Description: Support for external help files in Help menu; Types: custom full
Name: Plugins\Contrib\HexEditor;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\IncrementalSearch;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Key_Binder;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Koders;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Lib_Finder;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\MouseSap;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Nassi_Shneiderman;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Occurrences_Highlighting;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Profiler; Description: GProf output parser; Types: custom full
Name: Plugins\Contrib\Project_Options_Manipulator;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\RegEx_Testbed;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Reopen_Editor;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\SourceExporter; Description: Exports open file to HTML\RTF\ODT; Types: custom full
Name: Plugins\Contrib\SpellChecker;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\SymTab;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\ThreadSearch;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\Tools_Plus;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Contrib\wxSmith; Description: RAD editor for wxWidgets; Types: custom
Name: Plugins\Core;Description: Core Plugins; Types: custom compact full
Name: Plugins\Core\AStyleFormatter; Description: Astyle code formatter; Types: custom full
Name: Plugins\Core\Abbreviations;Description: Abbreviations plugin; Types: custom compact full
Name: Plugins\Core\Autosave;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Core\ClassWizard; Description: Simple C++ class-creation wizard; Types: custom full
Name: Plugins\Core\CodeCompletion; Description: Code completion \ Class browser; Types: custom full
Name: Plugins\Core\Compiler;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Core\Debugger;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Core\MIME_Handler;Description: Default MIME handler; Types: custom compact full
Name: Plugins\Core\Open_Files_List;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Core\Projects_Importer;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Core\RND_Generator;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Core\Scripted_Wizard;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Core\SmartIndent;Description: <PLUGIN_TBA>; Types: custom compact full
Name: Plugins\Core\ToDo; Description: To-Do list support; Types: custom full
Name: Plugins\Core\XP_Look_and_Feel;Description: <PLUGIN_TBA>; Types: custom compact full

[UninstallDelete]
Type: filesandordirs; Name: "{app}"
Type: files; Name: {userdesktop}\{#CB_PROGRAMDIRNAME}
Type: files; Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#CB_PROGRAMDIRNAME}

[Code]
// Uninstall
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  case CurUninstallStep of
    usUninstall:
      begin
        // RegDeleteKeyIncludingSubkeys(HKCR, '*\shell\Open With CodeLite');
        // Prompt the user to delete all his settings, default to "No"
        if MsgBox('Would you like to keep your user settings?', mbConfirmation, MB_YESNO or MB_DEFBUTTON1) = IDNO 
        then begin
            DelTree(ExpandConstant('{userappdata}') + '\CodeBlocks', True, True, True);
        end;
      end;
    usPostUninstall:
      begin
      end;
  end;
end;
