#include "pch.h"
#include "CustomParent.h"
#include "ThemeManager.h"
#include "ThemeData.h"

bool CCustomParent::InitHeader(HWND hwnd) {
	m_hChild = hwnd;
	m_Type = ControlType::Header;
	return true;
}

bool CCustomParent::InitListView(HWND hwnd) {
	m_Type = ControlType::ListView;
	CListViewCtrl lv(m_hChild = hwnd);
	m_Data.ListView.ThemeData = ThemeManager::Get()->GetListViewThemeData();
	lv.SetBkColor(0);
	lv.SetTextBkColor(0);
	lv.SetTextColor(RGB(240, 240, 240));

	return true;
}

bool CCustomParent::InitTreeView(HWND hwnd) {
	CTreeViewCtrl tv(m_hChild = hwnd);
	tv.SetBkColor(0);
	tv.SetTextColor(RGB(240, 240, 240));

	return true;
}

bool CCustomParent::InitRebar(HWND hwnd) {
	m_Type = ControlType::Rebar;
	CReBarCtrl rb(m_hChild = hwnd);
	//m_Data.ListView.ThemeData = ThemeManager::Get()->GetListViewThemeData();
	rb.SetBkColor(0);
	rb.SetTextColor(RGB(240, 240, 240));

	return true;
}

void CCustomParent::OnFinalMessage(HWND) {
	delete this;
}

LRESULT CCustomParent::OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
	if(DefWindowProc() < 0)
		return -1;

	return 0;
}

LRESULT CCustomParent::OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&) {
	return FALSE;
}

DWORD CCustomParent::OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW cd) {
	auto hWnd = cd->hdr.hwndFrom;
	if (m_hChild == hWnd) {
		switch (m_Type) {
			case ControlType::Header:
				CDCHandle dc(cd->hdc);
				dc.FillSolidRect(&cd->rc, RGB(0, 0, 128));
				dc.SetBkColor(0);
				dc.SelectStockBrush(BLACK_BRUSH);
				dc.SelectStockPen(WHITE_PEN);
				dc.SetBkMode(OPAQUE);
				dc.SetTextColor(RGB(240, 240, 240));
				//CHeaderCtrl header(m_hChild);
				//HDITEM item;
				//item.mask = HDI_TEXT;
				//WCHAR text[64];
				//item.pszText = text;
				//item.cchTextMax = _countof(text);
				//header.GetItem((int)cd->dwItemSpec, &item);
				//dc.DrawText(text, -1, &cd->rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				return CDRF_NOTIFYSUBITEMDRAW;
		}
	}
	SetMsgHandled(FALSE);
	return CDRF_SKIPPOSTPAINT;
}

DWORD CCustomParent::OnSubItemPrePaint(int, LPNMCUSTOMDRAW cd) {
	switch (m_Type) {
		case ControlType::Header:
			CDCHandle dc(cd->hdc);
			dc.SetBkColor(0);
			dc.SelectStockBrush(BLACK_BRUSH);
			dc.SelectStockPen(WHITE_PEN);
			dc.SetBkMode(OPAQUE);
			dc.SetTextColor(RGB(240, 240, 240));
			return CDRF_DODEFAULT;
	}

	SetMsgHandled(FALSE);
	return 0;
}

DWORD CCustomParent::OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/) {
	switch (m_Type) {
		case ControlType::Header:
			return CDRF_NOTIFYITEMDRAW;

	}
	SetMsgHandled(FALSE);
	return CDRF_NOTIFYITEMDRAW;
}
