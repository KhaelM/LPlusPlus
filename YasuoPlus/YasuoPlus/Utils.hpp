#pragma once
#include <Windows.h>
#include <iostream>


namespace Utils
{
	static void CreateConsoleWindow()
	{
		AllocConsole();

		HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		WORD dwAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		SetConsoleTitleA("Debug Console");
	}

	static void Log(std::string s)
	{
		using namespace std;
		//Log(s.c_str());
	}

	static void Log(char* Fmt, ...)
	{
		DWORD dwBytes = 0;
		char szBuffer[1024] = { 0 };

		va_list va;
		va_start(va, Fmt);
		vsnprintf_s(szBuffer, sizeof(szBuffer), Fmt, va);
		va_end(va);

		strcat_s(szBuffer, "\n");

		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), szBuffer, strlen(szBuffer), &dwBytes, nullptr);
	}
}