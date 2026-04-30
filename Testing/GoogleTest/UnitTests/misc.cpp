#include "pch.h"
#include "MergeApp.h"
#include "unicoder.h"
#include "IniOptionsMgr.h"
#include "OptionsDef.h"

void AppErrorMessageBox(const String& msg)
{
}

String GetSysError(int nerr)
{
	return _T("");
}

CIniOptionsMgr m_option(_T("UnitTests.ini"));

COptionsMgr * GetOptionsMgr()
{
	m_option.InitOption(OPT_PLUGINS_CUSTOM_SETTINGS_LIST, _T(""));
	return &m_option;
}

namespace I18n
{
	String tr(const std::string& str)
	{
		return ucr::toTString(str);
	}
}
