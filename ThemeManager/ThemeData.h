#pragma once

struct ListViewThemeData {
	COLORREF BackColor = CLR_INVALID;
	COLORREF TextColor = CLR_INVALID;
	COLORREF SelectedColor = CLR_INVALID;
	bool ShowFocusRect = false;
};

struct TreeViewThemeData {
	COLORREF BackColor = CLR_INVALID;
	COLORREF TextColor = CLR_INVALID;
};
