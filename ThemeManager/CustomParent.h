#pragma once

struct ListViewThemeData;
struct TreeViewThemeData;

class CCustomParent :
	public CWindowImpl<CCustomParent>,
	public CCustomDraw<CCustomParent> {
public:
	CCustomParent() {}

	DECLARE_WND_CLASS(nullptr)

	bool InitHeader(HWND hwnd);
	bool InitListView(HWND hwnd);
	bool InitTreeView(HWND hwnd);
	bool InitRebar(HWND hwnd);
	void OnFinalMessage(HWND);

	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
	DWORD OnSubItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);

	BEGIN_MSG_MAP(CCustomParent)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		CHAIN_MSG_MAP(CCustomDraw<CCustomParent>)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	enum class ControlType {
		Unknown,
		ListView,
		TreeView,
		Header,
		ToolBar,
		Rebar,
		StatusBar,
	};

	union {
		struct {
			ListViewThemeData* ThemeData;
		} ListView;
		struct {
			TreeViewThemeData* ThemeData;
		} TreeView;
	} m_Data;
	ControlType m_Type;
	HWND m_hChild;
};
