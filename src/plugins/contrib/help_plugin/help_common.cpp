#include "help_common.h"
#include <configmanager.h>
#include <wx/intl.h>
#include <wx/dynarray.h>

using std::make_pair;

int HelpCommon::m_DefaultHelpIndex = -1;

void HelpCommon::LoadHelpFilesVector(HelpCommon::HelpFilesVector &vect)
{
  vect.clear();
  ConfigManager* conf = Manager::Get()->GetConfigManager(_T("help_plugin"));
  m_DefaultHelpIndex = conf->ReadInt(_T("/default"), -1);
  wxArrayString list = conf->EnumerateSubPaths(_T("/"));

  for (unsigned int i = 0; i < list.GetCount(); ++i)
  {
      HelpFileAttrib hfa;
      wxString name = conf->Read(list[i] + _T("/name"), wxEmptyString);
      hfa.name = conf->Read(list[i] + _T("/file"), wxEmptyString);
      conf->Read(list[i] + _T("/isexec"), &hfa.isExecutable);
      conf->Read(list[i] + _T("/embeddedviewer"), &hfa.openEmbeddedViewer);
      // Patch by Yorgos Pagles: Read new attributes from settings
      int keyWordCase=0;
      (conf->Read(list[i] + _T("/keywordcase"), &keyWordCase));
      hfa.keywordCase = static_cast<HelpCommon::StringCase>(keyWordCase);
      hfa.defaultKeyword = conf->Read(list[i] + _T("/defaultkeyword"), wxEmptyString);

      if (!name.IsEmpty() && !hfa.name.IsEmpty())
      {
        vect.push_back(make_pair(name, hfa));
      }
  }
}

void HelpCommon::SaveHelpFilesVector(HelpCommon::HelpFilesVector &vect)
{
  ConfigManager* conf = Manager::Get()->GetConfigManager(_T("help_plugin"));
  wxArrayString list = conf->EnumerateSubPaths(_T("/"));

  for (unsigned int i = 0; i < list.GetCount(); ++i)
  {
    conf->DeleteSubPath(list[i]);
  }

  HelpFilesVector::iterator it;

  int count = 0;

  for (it = vect.begin(); it != vect.end(); ++it)
  {
    HelpFileAttrib hfa;
    wxString name = it->first;
    hfa = it->second;

    if (!name.IsEmpty() && !hfa.name.IsEmpty())
    {
      wxString key = wxString::Format(_T("/help%d/"), count++);
      conf->Write(key + _T("name"), name);
      conf->Write(key + _T("file"), hfa.name);
      conf->Write(key + _T("isexec"), hfa.isExecutable);
      conf->Write(key + _T("embeddedviewer"), hfa.openEmbeddedViewer);
      // Patch by Yorgos Pagles: Write new attributes in settings
      conf->Write(key + _T("keywordcase"), static_cast<int>(hfa.keywordCase));
      conf->Write(key + _T("defaultkeyword"), hfa.defaultKeyword);
    }
  }

  conf->Write(_T("/default"), m_DefaultHelpIndex);
}
