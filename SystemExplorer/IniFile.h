#pragma once

class IniFile {
public:
	IniFile(PCWSTR path);

	bool IsValid() const;

	CString ReadString(PCWSTR section, PCWSTR name, PCWSTR defaultValue = nullptr);
	int ReadInt(PCWSTR section, PCWSTR name, int defaultValue = 0);
	COLORREF ReadColor(PCWSTR section, PCWSTR name, COLORREF defaultValue = CLR_INVALID);
	std::vector<CString> ReadSection(PCWSTR section);
	bool ReadBool(PCWSTR section, PCWSTR name, bool defaultValue = false);

	bool WriteString(PCWSTR section, PCWSTR name, PCWSTR value);
	bool WriteInt(PCWSTR section, PCWSTR name, int value, bool hex = false);
	bool WriteBool(PCWSTR section, PCWSTR name, bool value);
	bool WriteColor(PCWSTR section, PCWSTR name, COLORREF color);

protected:
	COLORREF ParseHexColor(const CString& hex);
	COLORREF ParseDecColor(const CString& text);

private:
	CString _path;
};

