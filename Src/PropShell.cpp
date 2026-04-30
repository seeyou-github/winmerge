/** 
 * @file  PropShell.cpp
 *
 * @brief Implementation file for Shell Options dialog.
 *
 */

#include "stdafx.h"
#include "PropShell.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "Win_VersionHelper.h"
#include "MergeCmdLineInfo.h"
#include "JumpList.h"
#include "SuperComboBox.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// Flags for enabling and mode of extension
enum
{
	CONTEXT_F_ENABLED = 0x01,
	CONTEXT_F_ADVANCED = 0x02,
	CONTEXT_F_COMPARE_AS = 0x04
};

static bool IsShellExtensionRegistered(bool peruser)
{
	UNREFERENCED_PARAMETER(peruser);
	return false;
}

static bool IsWinMergeContextMenuRegistered()
{
	return false;
}

static bool RegisterShellExtension(bool unregister, bool peruser)
{
	UNREFERENCED_PARAMETER(unregister);
	UNREFERENCED_PARAMETER(peruser);
	return false;
}

static bool RegisterWinMergeContextMenu(bool unregister)
{
	UNREFERENCED_PARAMETER(unregister);
	return false;
}

static void LoadListView(CListCtrl& list)
{
	CRect rc;
	list.DeleteAllItems();
	list.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	list.GetClientRect(&rc);
	list.InsertColumn(0, _T(""), LVCFMT_LEFT, rc.Width() - GetSystemMetrics(SM_CXVSCROLL));
	DWORD dwFlags = 0;
	int i = 0;
	for (auto [mask, text] : std::vector<std::pair<MergeCmdLineInfo::usertasksflags_t, String>>(
		{
			{ MergeCmdLineInfo::NEW_TEXT_COMPARE,    _("New Text Compare") },
			{ MergeCmdLineInfo::NEW_TABLE_COMPARE,   _("New Table Compare") },
			{ MergeCmdLineInfo::NEW_BINARY_COMPARE,  _("New Binary Compare") },
			{ MergeCmdLineInfo::NEW_IMAGE_COMPARE,   _("New Image Compare") },
			{ MergeCmdLineInfo::NEW_FOLDER_COMPARE,  _("New Folder Compare") },
			{ MergeCmdLineInfo::CLIPBOARD_COMPARE,   _("Clipboard Compare") },
			{ MergeCmdLineInfo::SHOW_OPTIONS_DIALOG, _("Options") },
		}))
	{
		list.InsertItem(LVIF_TEXT | LVIF_PARAM, i, text.c_str(), 0, 0, 0, mask);
		list.SetCheck(i, dwFlags & mask);
		++i;
	}
}

static void SaveListView(CListCtrl& list)
{
	MergeCmdLineInfo::usertasksflags_t dwFlags = 0;
	int itemCount = list.GetItemCount();
	for (int i = 0; i < itemCount; ++i)
		dwFlags |= list.GetCheck(i) ? list.GetItemData(i) : 0;
	JumpList::AddUserTasks(CMergeApp::CreateUserTasks(dwFlags));
}

PropShell::PropShell(COptionsMgr *optionsMgr) 
: OptionsPanel(optionsMgr, PropShell::IDD)
, m_bContextAdded(false)
, m_bContextAdvanced(false)
, m_bContextCompareAs(false)
{
}

BOOL PropShell::OnInitDialog()
{
	OptionsPanel::OnInitDialog();

#ifndef BCM_SETSHIELD
#define BCM_SETSHIELD            (0x1600/*BCM_FIRST*/ + 0x000C)
#endif

	SendDlgItemMessage(IDC_REGISTER_SHELLEXTENSION, BCM_SETSHIELD, 0, TRUE);
	SendDlgItemMessage(IDC_UNREGISTER_SHELLEXTENSION, BCM_SETSHIELD, 0, TRUE);

	LoadListView(m_list);

	// Update shell extension checkboxes
	UpdateButtons();
	GetContextRegValues();
	AdvancedContextMenuCheck();
	UpdateData(FALSE);

	SetTimer(0, 1000, nullptr);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void PropShell::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropShell)
	DDX_Check(pDX, IDC_EXPLORER_CONTEXT, m_bContextAdded);
	DDX_Check(pDX, IDC_EXPLORER_ADVANCED, m_bContextAdvanced);
	DDX_Check(pDX, IDC_EXPLORER_COMPARE_AS, m_bContextCompareAs);
	DDX_Control(pDX, IDC_JUMP_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PropShell, OptionsPanel)
	//{{AFX_MSG_MAP(PropShell)
	ON_BN_CLICKED(IDC_EXPLORER_CONTEXT, OnAddToExplorer)
	ON_BN_CLICKED(IDC_EXPLORER_ADVANCED, OnAddToExplorerAdvanced)
	ON_BN_CLICKED(IDC_REGISTER_SHELLEXTENSION, OnRegisterShellExtension)
	ON_BN_CLICKED(IDC_UNREGISTER_SHELLEXTENSION, OnUnregisterShellExtension)
	ON_BN_CLICKED(IDC_REGISTER_SHELLEXTENSION_PERUSER, OnRegisterShellExtensionPerUser)
	ON_BN_CLICKED(IDC_UNREGISTER_SHELLEXTENSION_PERUSER, OnUnregisterShellExtensionPerUser)
	ON_BN_CLICKED(IDC_REGISTER_WINMERGECONTEXTMENU, OnRegisterWinMergeContextMenu)
	ON_BN_CLICKED(IDC_UNREGISTER_WINMERGECONTEXTMENU, OnUnregisterWinMergeContextMenu)
	ON_BN_CLICKED(IDC_CLEAR_ALL_RECENT_ITEMS, OnClearAllRecentItems)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropShell::ReadOptions()
{
	GetContextRegValues();
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropShell::WriteOptions()
{
	bool registered = IsShellExtensionRegistered(false);
	bool registeredPerUser = IsShellExtensionRegistered(true);
	bool registeredWinMergeContextMenu = IsWinMergeContextMenuRegistered();
	UNREFERENCED_PARAMETER(registered);
	UNREFERENCED_PARAMETER(registeredPerUser);
	UNREFERENCED_PARAMETER(registeredWinMergeContextMenu);
	if (m_list.m_hWnd)
		SaveListView(m_list);
}

void PropShell::GetContextRegValues()
{
	m_bContextAdded = false;
	m_bContextAdvanced = false;
	m_bContextCompareAs = false;
}

/// Set registry values for ShellExtension
void PropShell::OnAddToExplorer()
{
	AdvancedContextMenuCheck();
	UpdateButtons();
}

void PropShell::OnAddToExplorerAdvanced()
{
	CompareAsContextMenuCheck();
	UpdateButtons();
}

void PropShell::SaveMergePath()
{
}

/// Enable/Disable "Advanced menu" checkbox.
void PropShell::AdvancedContextMenuCheck()
{
	if (!IsDlgButtonChecked(IDC_EXPLORER_CONTEXT))
	{
		CheckDlgButton(IDC_EXPLORER_ADVANCED, FALSE);
		m_bContextAdvanced = false;
		CompareAsContextMenuCheck();
	}
}

void PropShell::CompareAsContextMenuCheck()
{
	if (!IsDlgButtonChecked(IDC_EXPLORER_ADVANCED))
	{
		CheckDlgButton(IDC_EXPLORER_COMPARE_AS, FALSE);
		m_bContextCompareAs = false;
	}
}

void PropShell::UpdateButtons()
{
	bool registered = IsShellExtensionRegistered(false);
	bool registeredPerUser = IsShellExtensionRegistered(true);
	bool registerdWinMergeContextMenu = IsWinMergeContextMenuRegistered();
	bool win11 = IsWin11_OrGreater();
	bool win7 = IsWin7_OrGreater();
	EnableDlgItem(IDC_EXPLORER_CONTEXT, registered || registeredPerUser || registerdWinMergeContextMenu);
	EnableDlgItem(IDC_REGISTER_SHELLEXTENSION, !registered);
	EnableDlgItem(IDC_UNREGISTER_SHELLEXTENSION, registered);
	EnableDlgItem(IDC_REGISTER_SHELLEXTENSION_PERUSER, !registeredPerUser);
	EnableDlgItem(IDC_UNREGISTER_SHELLEXTENSION_PERUSER, registeredPerUser);
	EnableDlgItem(IDC_REGISTER_WINMERGECONTEXTMENU, !registerdWinMergeContextMenu && win11);
	EnableDlgItem(IDC_UNREGISTER_WINMERGECONTEXTMENU, registerdWinMergeContextMenu && win11);
	EnableDlgItem(IDC_EXPLORER_ADVANCED, 
		(registered || registeredPerUser || registerdWinMergeContextMenu) && IsDlgButtonChecked(IDC_EXPLORER_CONTEXT));
	EnableDlgItem(IDC_EXPLORER_COMPARE_AS,
		(registered || registeredPerUser || registerdWinMergeContextMenu) && IsDlgButtonChecked(IDC_EXPLORER_ADVANCED));
	EnableDlgItem(IDC_JUMP_LIST, win7);
	EnableDlgItem(IDC_CLEAR_ALL_RECENT_ITEMS, win7);
}

void PropShell::OnRegisterShellExtension()
{
	RegisterShellExtension(false, false);
}

void PropShell::OnUnregisterShellExtension()
{
	RegisterShellExtension(true, false);
}

void PropShell::OnRegisterShellExtensionPerUser()
{
	RegisterShellExtension(false, true);
}

void PropShell::OnUnregisterShellExtensionPerUser()
{
	RegisterShellExtension(true, true);
}

void PropShell::OnRegisterWinMergeContextMenu()
{
	RegisterWinMergeContextMenu(false);
}

void PropShell::OnUnregisterWinMergeContextMenu()
{
	RegisterWinMergeContextMenu(true);
}

void PropShell::OnClearAllRecentItems()
{
	JumpList::RemoveRecentDocs();
	for (const auto& name : {
		_T("ReportFiles"),
		_T("Files\\Left"),
		_T("Files\\Right"),
		_T("Files\\Option"),
		_T("Files\\Ext"),
		_T("Files\\Unpacker"),
		_T("Files\\Prediffer"),
		_T("Files\\EditorScript"),
		_T("Files\\DiffFileResult"),
		_T("Files\\DiffFile1"),
		_T("Files\\DiffFile2"),
		_T("PatchCreator\\DiffContext"),
		})
	{
		CSuperComboBox::ClearState(name);
	}
}

void PropShell::OnTimer(UINT_PTR nIDEvent)
{
	UpdateButtons();
}
