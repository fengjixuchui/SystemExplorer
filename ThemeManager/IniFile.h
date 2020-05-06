#pragma once

class IniFile {
public:
	IniFile(PCWSTR path);

	CString ReadString(PCWSTR section, PCWSTR name, PCWSTR defaultValue = nullptr);
	int ReadInt(PCWSTR section, PCWSTR name, int defaultValue = 0);
	COLORREF ReadColor(PCWSTR section, PCWSTR name, COLORREF defaultValue = CLR_INVALID);

protected:
	COLORREF ParseHexColor(const CString& hex);
	COLORREF ParseDecColor(const CString& text);

private:
	CString _path;
};

