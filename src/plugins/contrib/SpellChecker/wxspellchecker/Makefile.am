AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			$(CB_HUNSPELL_CFLAGS) \
			-Iinclude \
			-I.. \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT
AM_CPPFLAGS += \
			-DWXUSINGDLL \
			-DwxUSE_UNICODE \
			-D_WIN64 \
			-DWXUSINGDLL \
			-D__WXMSW__
else
AM_CPPFLAGS += \
			-I../hunspell/src/hunspell \
			-DHUNSPELL_STATIC
endif

noinst_LTLIBRARIES = \
			libwxSpellChecker.la

libwxSpellChecker_la_LIBADD = \
			$(WX_LIBS) \
			$(CB_HUNSPELL_LIBS)

libwxSpellChecker_la_SOURCES = \
			src/HunspellInterface.cpp \
			src/PersonalDictionary.cpp \
			src/SpellCheckCmdLineInterface.cpp \
			src/SpellCheckDialogInterface.cpp \
			src/SpellCheckEngineInterface.cpp \
			src/SpellCheckEngineOption.cpp \
			src/SpellCheckUserInterface.cpp \
			src/SpellCheckerOptionsDialog.cpp \
			src/XmlPersonalDictionaryDialog.cpp \
			src/XmlSpellCheckDialog.cpp \
			src/exports.cpp

noinst_HEADERS = \
			include/HunspellInterface.h \
			include/PersonalDictionary.h \
			include/SpellCheckEngineInterface.h \
			include/SpellCheckerOptionsDialog.h \
			include/exports.h \
			include/SpellCheckCmdLineInterface.h \
			include/SpellCheckEngineOption.h \
			include/XmlPersonalDictionaryDialog.h \
			include/SpellCheckDialogInterface.h \
			include/SpellCheckUserInterface.h \
			include/XmlSpellCheckDialog.h

EXTRA_DIST = \
			src/open.xpm
