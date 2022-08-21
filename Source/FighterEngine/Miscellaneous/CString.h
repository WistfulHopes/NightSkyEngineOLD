#pragma once

template<int Size>
class CString
{
	char String[Size] = { 0 };
public:
	CString()
	{
		memset(String, 0, Size);
	}

	void SetString(FString InString)
	{
		memset(String, 0, Size);
		strcpy(String, TCHAR_TO_ANSI(*InString));
	}

	char* GetString()
	{
		return String;
	}
};
