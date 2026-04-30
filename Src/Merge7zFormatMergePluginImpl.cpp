/** 
 * @file  Merge7zFormatMergePluginImpl.cpp
 *
 * @brief Implementation file for Merge7zFormatMergePluginImpl class
 */
#include "pch.h"
#include "Merge7zFormatMergePluginImpl.h"
#include "paths.h"
#include "Plugins.h"
#include "Merge7zFormatRegister.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "MergeApp.h"
#include "7zCommon.h"
#include <list>
#include <Poco/Mutex.h>

static Merge7zFormatRegister g_autoregister(&Merge7zFormatMergePluginImpl::GuessFormat);
static __declspec(thread) Merge7zFormatMergePluginImpl *g_pluginformat;
static std::list<std::unique_ptr<Merge7zFormatMergePluginImpl> > g_pluginformat_list;
static Poco::FastMutex g_mutex;

static Merge7zFormatMergePluginImpl *GetInstance()
{
	if (g_pluginformat == nullptr)
	{
		g_pluginformat = new Merge7zFormatMergePluginImpl();
		Poco::FastMutex::ScopedLock lock(g_mutex);
		g_pluginformat_list.emplace_back(g_pluginformat);
	}
	return g_pluginformat;
}

Merge7z::Format *Merge7zFormatMergePluginImpl::GuessFormat(const String& path)
{
	// TODO: Support plugin pipeline and plugin aliases.
	if (!GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED))
		return nullptr;
	Merge7zFormatMergePluginImpl *format = GetInstance();
	Merge7z::Format* formatChild = nullptr;
	PluginInfo *plugin = nullptr;
	String dummypath = path;
	bool isfolder = false;
	if (!isfolder)
	{
		if (format->m_infoUnpacker.GetPluginPipeline().find(_T("<Automatic>")) != String::npos)
			plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_FOLDER_PACK_UNPACK", dummypath);
		else if (!format->m_infoUnpacker.GetPluginPipeline().empty())
			plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"FILE_FOLDER_PACK_UNPACK", format->m_infoUnpacker.GetPluginPipeline());
		if (plugin == nullptr)
			return nullptr;
		if (plugin)
		{
			if (!plugin::InvokeIsFolder(dummypath, plugin->m_lpDispatch))
				return nullptr;
		}
	}
	format->m_plugin = plugin;
	format->m_format = formatChild;
	return format;
}

HRESULT Merge7zFormatMergePluginImpl::DeCompressArchive(HWND, LPCTSTR path, LPCTSTR folder)
{
	int nChanged = 0;
	int subcode = 0;
	if (m_plugin == nullptr && m_format == nullptr)
		return E_FAIL;
	String srcpath = path;
	paths::CreateIfNeeded(folder);
	if (m_plugin)
	{
		return plugin::InvokeUnpackFolder(srcpath, folder, nChanged, m_plugin->m_lpDispatch, subcode) ? S_OK : E_FAIL;
	}
	else if (m_format)
	{
		return m_format->DeCompressArchive(nullptr, srcpath.c_str(), folder);
	}
	return E_FAIL;
}

HRESULT Merge7zFormatMergePluginImpl::CompressArchive(HWND, LPCTSTR path, Merge7z::DirItemEnumerator *)
{
	return E_FAIL;
}

Merge7z::Format::Inspector *Merge7zFormatMergePluginImpl::Open(HWND, LPCTSTR) { return nullptr; }
Merge7z::Format::Updater *Merge7zFormatMergePluginImpl::Update(HWND, LPCTSTR) { return nullptr; }
HRESULT Merge7zFormatMergePluginImpl::GetHandlerProperty(HWND, PROPID, PROPVARIANT *, VARTYPE) { return E_FAIL; }
BSTR Merge7zFormatMergePluginImpl::GetHandlerName(HWND) { return nullptr; }
BSTR Merge7zFormatMergePluginImpl::GetHandlerClassID(HWND) { return nullptr; }
BSTR Merge7zFormatMergePluginImpl::GetHandlerExtension(HWND) { return nullptr; }
BSTR Merge7zFormatMergePluginImpl::GetHandlerAddExtension(HWND) { return nullptr; }
VARIANT_BOOL Merge7zFormatMergePluginImpl::GetHandlerUpdate(HWND) { return VARIANT_FALSE; }
VARIANT_BOOL Merge7zFormatMergePluginImpl::GetHandlerKeepName(HWND) { return VARIANT_FALSE; }

BSTR Merge7zFormatMergePluginImpl::GetDefaultName(HWND, LPCTSTR path)
{
	return SysAllocString(L"");
}

void Merge7zFormatMergePluginImpl::SetPackingInfo(const PackingInfo* infoUnpacker)
{
	GetInstance()->m_infoUnpacker = infoUnpacker ? *infoUnpacker : PackingInfo();
}

PackingInfo *Merge7zFormatMergePluginImpl::GetPackingInfo()
{
	return &GetInstance()->m_infoUnpacker;
}
