﻿// ConsoleApplication6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>

void inject_dll(DWORD PId, char* DllName)
{

	HANDLE hProcess;
	HINSTANCE dllHandle;
	PVOID Alloc;
	SIZE_T DllPathLen;
	HMODULE Kernel32Base;
	PVOID LoadLibAddress;
	displayMessage messageBoxPtr = NULL;
	BOOL runtimeLinkSucces = FALSE;
	if (PId != 0 && DllName != NULL)
	{
		DllPathLen = strlen(DllName);
		Kernel32Base = GetModuleHandleA("kernel32.dll");
		if (Kernel32Base == NULL)
		{
			std::cout << "kernel32.dll not found" << std::endl;
			return;
		}

		LoadLibAddress = GetProcAddress(Kernel32Base, "LoadLibraryA");
		if (LoadLibAddress == NULL)
		{
			std::cout << "LoadLibraryA not found" << std::endl;
			return;
		}
			

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PId);
		if (hProcess == NULL)
		{
			std::cout << "hProcess handle not opened" << std::endl;
			return;
		}
			

		Alloc = VirtualAllocEx(hProcess, NULL, DllPathLen + 1, MEM_COMMIT, PAGE_READWRITE);
		if (Alloc == NULL)
		{
			std::cout << "no memory allocated for hProcess" << std::endl;
			return;
		}

		SIZE_T numBytesWritten = 0;
		if (!WriteProcessMemory(hProcess, Alloc, DllName, DllPathLen + 1, &numBytesWritten))
		{
			std::cout << "didn't write dll to processmemory" << std::endl;
			return;
		}
		std::cout << numBytesWritten << std::endl;

		HANDLE remoteHandle = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibAddress, (LPVOID)Alloc, 0, NULL);

		std::cout << remoteHandle << std::endl;

		DWORD exitCode = 0;
		LPDWORD ptrExitcode = &exitCode;

		WaitForSingleObject(remoteHandle, INFINITE);
		//GetExitCodeThread(remoteHandle, ptrExitcode);
		VirtualFreeEx(hProcess, Alloc, 0, MEM_RELEASE);
		CloseHandle(hProcess);

		std::cout << "end reached" << std::endl;
	}

}


DWORD get_PId( const char* ProcessName) {

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
		
	if (hSnapshot != NULL)
	{
		if (Process32First(hSnapshot, &pe32)) {
			do
			{
				if (!strcmp(pe32.szExeFile, ProcessName))
				{
					CloseHandle(hSnapshot);
					return pe32.th32ProcessID;
				}
			} while (Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	DWORD pid = get_PId("ConsoleApplication7.exe");
	std::cout << pid << std::endl;
	if (pid) {
		char dllName[] = "C:\\Users\\Arthur\\Desktop\\VisualStudio\\#2\\DLL\\Dll3.dll";
		std::cout << dllName << std::endl;
		inject_dll(pid, dllName);
	}

	system("PAUSE");
	return 0;
}

