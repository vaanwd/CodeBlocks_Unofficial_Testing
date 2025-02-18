==================
Summary
==================

cbDiff is a plugin for Code::Blocks IDE,
which can show visually the difference
between two files.

==================
License
==================

Please read the COPYING.

==================
How to build
==================

You need to have Code::Blocks IDE installed and the Code::Blocks IDE SDK.
Open cbDiff.cbp, hit build and install the cbDiff.cbplugin.
Now you can use it through the menubar (File -> Diff Files...), the projecttree
or the editor context menu.
This should work on every plattform.
==================
Notes for other Developers
==================

With a little trick cbDiff can be used by other plugins:

#include <wx/dynlib.h>

// First check if cbDiff is installed
if(Manager::Get()->GetPluginManager()->FindPluginByName(_T("cbDiff")) != NULL)
{
	PluginElement* element = Manager::Get()->GetPluginManager()->FindElementByName(_T("cbDiff"));
	// is library loaded
	if(element->library->IsLoaded())
	{
	    typedef void (*cbDiffFunc) (const wxString&, const wxString&, int viewmode);

	    cbDiffFunc difffunc = (cbDiffFunc)element->library->GetSymbol(_("DiffFiles"));
	    if(difffunc != NULL)
		{
			difffunc(firstfile, secondfile, -1);
	    }
	}
}

Latest version source code:
https://github.com/ywx/cbDiff
https://github.com/danselmi/cbDiff