// ThemeManager.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "ThemeManager.h"
#include <exception>
#include "CustomParent.h"

#pragma comment(lib, R"(..\packages\Detours.4.0.1\lib\native\libs\x64\detours.lib)")

extern CAtlWinModule _AtlWinModule;

LRESULT CALLBACK OnHookFunc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);

static decltype(::GetSysColor)* OrgGetSysColor = ::GetSysColor;
static decltype(::GetSysColorBrush)* OrgGetSysColorBrush = ::GetSysColorBrush;

HBRUSH WINAPI HookedGetSysColorBrush(int index) {
	switch (index) {
		case COLOR_3DFACE:
		case COLOR_WINDOW:
		case COLOR_APPWORKSPACE:
		case COLOR_MENUHILIGHT:
		case COLOR_MENUBAR:
			return (HBRUSH)::GetStockObject(BLACK_BRUSH);

		case COLOR_WINDOWTEXT:
		case COLOR_BTNTEXT:
			return (HBRUSH)::GetStockObject(WHITE_BRUSH);
	}
	return OrgGetSysColorBrush(index);
}

COLORREF WINAPI HookedGetSysColor(int index) {
	switch (index) {
		case COLOR_WINDOW:
		case COLOR_3DFACE:
		case COLOR_APPWORKSPACE:
		case COLOR_MENUBAR:
			return RGB(0, 0, 0);

		case COLOR_HIGHLIGHT:
			return RGB(0, 128, 0);

		case COLOR_MENU:
			return RGB(255, 255, 0);

		case COLOR_INFOBK:
			return RGB(0, 0, 255);

		case COLOR_WINDOWTEXT:
		case COLOR_BTNTEXT:
			return RGB(255, 255, 255);
	}
	return OrgGetSysColor(index);
}

ThemeManager::ThemeManager() {
}

ThemeManager* ThemeManager::Get() {
	static ThemeManager tm;
	return &tm;
}

bool ThemeManager::Init(CAtlWinModule* module) {
	ATLASSERT(module);
	_module = module;
	_hook = ::SetWindowsHookEx(WH_CALLWNDPROCRET, OnHookFunc, nullptr, ::GetCurrentThreadId());
	ATLASSERT(_hook);

	if (!_hook)
		return false;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&OrgGetSysColor, HookedGetSysColor);
	DetourAttach((PVOID*)&OrgGetSysColorBrush, HookedGetSysColorBrush);
	auto error = DetourTransactionCommit();
	ATLASSERT(error == NOERROR);

	return true;
}

bool ThemeManager::HookWindow(HWND hWnd, const CString& className) {
	ATLASSERT(hWnd);

	if (className == WC_LISTVIEW) {
		auto win = new CCustomParent;
		auto hParent = ::GetParent(hWnd);
		ATLVERIFY(win->SubclassWindow(hParent));
		win->InitListView(hWnd);
		win->RedrawWindow();
		return true;
	}
	return false;
}

LRESULT OnHookFunc(int nCode, WPARAM wParam, LPARAM lParam) {
	return ThemeManager::Get()->OnHook(nCode, wParam, lParam);
}

LRESULT ThemeManager::OnHook(int nCode, WPARAM wParam, LPARAM lParam) {
	static HBRUSH hBack = ::CreateSolidBrush(RGB(0, 128, 255));
	if (nCode == HC_ACTION) {
		auto cs = (CWPRETSTRUCT*)lParam;
		if (cs->message == WM_CREATE) {
			CString className;
			if (::GetClassName(cs->hwnd, className.GetBufferSetLength(64), 64)) {
				ATLTRACE(L"Class Name: %s\n", (PCWSTR)className);
				if (className.Left(7) == L"Themed_")
					className = className.Mid(7);
				auto hParent = ::GetParent(cs->hwnd);
				if (className.CompareNoCase(WC_LISTVIEW) == 0) {
					// list view control
					auto win = new CCustomParent;
					ATLVERIFY(win->SubclassWindow(hParent));
					win->InitListView(cs->hwnd);
				}
				else if (className.CompareNoCase(WC_TREEVIEW) == 0) {
					// tree view control
					auto win = new CCustomParent;
					ATLVERIFY(win->SubclassWindow(hParent));
					win->InitTreeView(cs->hwnd);
				}
				else if (className.CompareNoCase(REBARCLASSNAME) == 0) {
					// rebar control
					auto win = new CCustomParent;
					ATLVERIFY(win->SubclassWindow(hParent));
					win->InitRebar(cs->hwnd);
				}
				else if (className.CompareNoCase(WC_TABCONTROL) == 0) {
					// tab control
					int zz = 9;
				}
				//else if (className.CompareNoCase(WC_HEADER) == 0) {
				//	// header control
				//	auto win = new CCustomParent;
				//	ATLVERIFY(win->SubclassWindow(hParent));
				//	win->InitHeader(cs->hwnd);
				//}
			}
		}
	}
	return ::CallNextHookEx(_hook, nCode, wParam, lParam);
}

ListViewThemeData* ThemeManager::GetListViewThemeData() {
	return &_listViewThemeData;
}

