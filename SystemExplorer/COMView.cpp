#include "stdafx.h"
#include "COMView.h"
#include "SortHelper.h"
#include <algorithm>

using namespace WinSys;

CComView::CComView(IMainFrame* frame) : CViewBase(frame) {
}

CString CComView::GetColumnText(HWND, int row, int col) const {
	auto f = m_GetColumnTextFunctions[m_SelectedNode.GetData()];
	if (f)
		return f(row, col);

	return L"";
}

void CComView::DoSort(const SortInfo* si) {
	if (si == nullptr)
		return;

	auto sorter = m_Sorter[m_SelectedNode.GetData()];
	if (sorter)
		sorter(si);
}

LRESULT CComView::OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
	m_hWndClient = m_Splitter.Create(*this, rcDefault, nullptr,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	m_List.Create(m_Splitter, rcDefault, nullptr, ListViewDefaultStyle & ~LVS_SHAREIMAGELISTS);
	m_List.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP | LVS_EX_HEADERDRAGDROP);

	m_Tree.Create(m_Splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | TVS_LINESATROOT | TVS_HASLINES | TVS_HASBUTTONS);

	m_Splitter.SetSplitterExtendedStyle(SPLIT_FLATBAR | SPLIT_PROPORTIONAL);

	m_Splitter.SetSplitterPanes(m_Tree, m_List);

	InitTree();

	m_GetColumnTextFunctions[(int)NodeType::Classes] = [this](auto row, auto col) { return GetColumnTextClass(row, col); };
	m_GetColumnTextFunctions[(int)NodeType::Interfaces] = [this](auto row, auto col) { return GetColumnTextInterface(row, col); };

	m_InitListView [(int)NodeType::Classes]= [this]() { InitListViewClasses(); };
	m_InitListView[(int)NodeType::Interfaces] = [this]() { InitListViewInterfaces(); };

	m_Sorter[(int)NodeType::Classes] = [this](auto si) { DoSortClasses(si); };
	m_Sorter[(int)NodeType::Interfaces] = [this](auto si) { DoSortInterfaces(si); };

	m_Splitter.SetSplitterPosPct(15);

	return 0;
}

LRESULT CComView::OnSelectedTreeItemChanged(int, LPNMHDR hdr, BOOL&) {
	auto selected = m_Tree.GetSelectedItem();
	if (selected == nullptr)
		return 0;

	UpdateList(selected);

	return 0;
}

void CComView::InitTree() {
	CreateRootItem(L"Default", ComStore::Default);
	CreateRootItem(L"Machine", ComStore::User);
	CreateRootItem(L"User", ComStore::Machine);
	CreateRootItem(L"Default (32 bit)", ComStore::Default32);
	CreateRootItem(L"Machine (32 bit)", ComStore::User32);
	CreateRootItem(L"User (32 bit)", ComStore::Machine32);
}

CTreeItem CComView::CreateRootItem(PCWSTR name, ComStore store) {
	auto root = m_Tree.InsertItem(name, TVI_ROOT, TVI_LAST);
	root.SetData((DWORD_PTR)store);
	CreateStoreItems(root);

	return root;
}

void CComView::CreateStoreItems(CTreeItem root) {
	root.InsertAfter(L"Classes", root, 0).SetData((DWORD_PTR)NodeType::Classes);
	root.InsertAfter(L"Interfaces", root, 0).SetData((DWORD_PTR)NodeType::Interfaces);
	root.InsertAfter(L"Type Libraries", root, 0).SetData((DWORD_PTR)NodeType::Typelibs);
	root.InsertAfter(L"App IDs", root, 0).SetData((DWORD_PTR)NodeType::AppIds);
}

void CComView::UpdateList(CTreeItem item) {
	CWaitCursor wait;

	m_Classes.clear();
	m_Interfaces.clear();

	auto parent = item.GetParent();
	if (!parent) {
		// sub node
		if (m_ComExplorer.Open((ComStore)item.GetData())) {

		}
		m_List.SetItemCount(0);
		return;
	}

	int count = 0;
	m_SelectedNode = item;
	auto init = m_InitListView[item.GetData()];
	if (init)
		init();

	switch ((NodeType)item.GetData()) {
		case NodeType::Classes:
			m_Classes = m_ComExplorer.EnumClasses();
			count = static_cast<int>(m_Classes.size());
			break;

		case NodeType::Interfaces:
			m_Interfaces = m_ComExplorer.EnumInterfaces();
			count = static_cast<int>(m_Interfaces.size());
			break;
	}

	m_List.SetItemCount(count);
}

CString CComView::GetColumnTextClass(int row, int col) const {
	auto& data = m_Classes[row];

	CString text;

	switch (col) {
		case 0: return data.FriendlyName.c_str();
		case 1: return ClsidToString(data.Clsid);
		case 2: return ServerTypeToString(data.ServerType);
		case 3: return data.ModulePath.c_str();
		case 4: return data.ThreadingModel.c_str();
	}
	return text;
}

CString CComView::GetColumnTextInterface(int row, int col) const {
	auto& data = m_Interfaces[row];

	CString text;

	switch (col) {
		case 0: return data.FriendlyName.c_str();
		case 1: return ClsidToString(data.Iid);
		case 2: return data.ProxyStub == GUID_NULL ? L"" : ClsidToString(data.ProxyStub);
		case 3: return data.TypeLib == GUID_NULL ? L"" : ClsidToString(data.TypeLib);
	}
	return text;
}

void CComView::InitListViewClasses() {
	while (m_List.DeleteColumn(0))
		;

	m_List.InsertColumn(0, L"Friendly Name", LVCFMT_LEFT, 250);
	m_List.InsertColumn(1, L"CLSID", LVCFMT_RIGHT, 250);
	m_List.InsertColumn(2, L"Server Type", LVCFMT_LEFT, 80);
	m_List.InsertColumn(3, L"Path / Service", LVCFMT_LEFT, 250);
	m_List.InsertColumn(4, L"Threading Model", LVCFMT_LEFT, 80);
}

void CComView::InitListViewInterfaces() {
	while (m_List.DeleteColumn(0))
		;

	m_List.InsertColumn(0, L"Friendly Name", LVCFMT_LEFT, 250);
	m_List.InsertColumn(1, L"IID", LVCFMT_RIGHT, 250);
	m_List.InsertColumn(2, L"Proxy/Stub", LVCFMT_LEFT, 250);
	m_List.InsertColumn(3, L"Type Library", LVCFMT_LEFT, 250);
}

void CComView::DoSortClasses(const SortInfo* si) {
	auto asc = si->SortAscending;
	auto col = si->SortColumn;

	std::sort(m_Classes.begin(), m_Classes.end(), [=](auto& c1, auto& c2) {
		switch (col) {
			case 0: return SortHelper::SortStrings(c1.FriendlyName, c2.FriendlyName, asc);
			case 1: return SortHelper::SortStrings(ClsidToString(c1.Clsid), ClsidToString(c2.Clsid), asc);
			case 2: return SortHelper::SortNumbers(c1.ServerType, c2.ServerType, asc);
			case 3: return SortHelper::SortStrings(c1.ModulePath, c2.ModulePath, asc);
			case 4: return SortHelper::SortStrings(c1.ThreadingModel, c2.ThreadingModel, asc);
		}
		return false;
		});
}

void CComView::DoSortInterfaces(const SortInfo* si) {
	auto asc = si->SortAscending;
	auto col = si->SortColumn;

	std::sort(m_Interfaces.begin(), m_Interfaces.end(), [=](auto& c1, auto& c2) {
		switch (col) {
			case 0: return SortHelper::SortStrings(c1.FriendlyName, c2.FriendlyName, asc);
			case 1: return SortHelper::SortStrings(ClsidToString(c1.Iid), ClsidToString(c2.Iid), asc);
			case 2: return SortHelper::SortStrings(ClsidToString(c1.ProxyStub), ClsidToString(c2.ProxyStub), asc);
			case 3: return SortHelper::SortStrings(ClsidToString(c1.TypeLib), ClsidToString(c2.TypeLib), asc);
		}
		return false;
		});
}

CString CComView::ClsidToString(const GUID& guid) {
	WCHAR text[64];
	if(::StringFromGUID2(guid, text, _countof(text)))
		return text;
	return L"";
}

PCWSTR CComView::ServerTypeToString(ComServerType type) {
	switch (type) {
		case ComServerType::InProc: return L"In Proc";
		case ComServerType::OutOfProc: return L"Out of Proc";
		case ComServerType::Service: return L"Service";
	}
	return L"";
}
