#pragma once

#include "ThemeData.h"

class CCustomParent;

class ThemeManager {
public:
	static ThemeManager* Get();

	bool Init(CAtlWinModule* module);
	bool LoadTheme(PCWSTR path);
	bool HookWindow(HWND hWnd, const CString& className);

	LRESULT OnHook(int nCode, WPARAM wParam, LPARAM lParam);

	ListViewThemeData* GetListViewThemeData();

private:
	bool CheckATLClasses(HWND hWnd, const CString& className);
	ThemeManager();

private:
	CAtlWinModule* _module;
	HHOOK _hook{ nullptr };
	ListViewThemeData _listViewThemeData;
	TreeViewThemeData _treeViewThemeData;
};
