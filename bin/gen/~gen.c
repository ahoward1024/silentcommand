#include <stdio.h>
#include <windows.h>
#include <tchar.h>
int CALLBACK WinMain(HINSTANCE h, HINSTANCE p, LPSTR cmd, int show) {
	SHELLEXECUTEINFO shex = {0};
	shex.cbSize = sizeof(shex);
	shex.fMask = SEE_MASK_NOCLOSEPROCESS;
	shex.hwnd = 0;
	shex.lpFile = _T("\"C:\\SCRIPTS\\nircmdc.exe\"");
	shex.lpParameters = _T("setdefaultsounddevice \"Audio Device\"");
	shex.lpDirectory = 0;
	shex.nShow = SW_HIDE;
	shex.hInstApp = 0;
	if(ShellExecuteEx(&shex)) {
		WaitForSingleObject(shex.hProcess, INFINITE);
		CloseHandle(shex.hProcess);
	}
	return 0;
}
