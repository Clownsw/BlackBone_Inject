
#include <iostream>
#include <windows.h>

#pragma warning(disable : 4996)

typedef enum _InjectType
{
	IT_Thread,      // CreateThread into LdrLoadDll
	IT_Apc,         // Force user APC into LdrLoadDll
	IT_MMap,        // Manual map
} InjectType;

typedef HANDLE(*Func_Initialize)(void);
typedef NTSTATUS(*Func_InjectDll)(
	DWORD pid,
	const std::wstring& path,
	InjectType itype,
	uint32_t initRVA,
	const std::wstring& initArg,
	bool unlink,
	bool erasePE,
	bool wait);


HMODULE dll_hModule = NULL;
HANDLE DriverHandle = NULL;
HWND Windowhwnd = NULL;

DWORD pid = NULL;
std::wstring dll_path = {};
const std::wstring initArg = L"";

TCHAR ProcessWindowName[MAX_PATH + 1];
TCHAR szValue[MAX_PATH + 1];
int unlink_ = 0, erasePE_ = 0;

Func_Initialize Initialize;
Func_InjectDll InjectDll;

void Init(HMODULE _hModule)
{
	Initialize = (Func_Initialize)GetProcAddress(_hModule, reinterpret_cast<LPCSTR>(1));
	InjectDll = (Func_InjectDll)GetProcAddress(_hModule, reinterpret_cast<LPCSTR>(13));
}

void inject(void)
{
	if (unlink_ == 1 && erasePE_ == 1)
	{
		InjectDll(pid, szValue, IT_Thread, 0, initArg, true, true, true);
	}
	else if (unlink_ == 0 && erasePE_ == 1)
	{
		InjectDll(pid, szValue, IT_Thread, 0, initArg, false, true, true);
	}
	else if (unlink_ == 1 && erasePE_ == 0)
	{
		InjectDll(pid, szValue, IT_Thread, 0, initArg, true, false, true);
	}
	else if (unlink_ == 0 && erasePE_ == 0)
	{
		InjectDll(pid, szValue, IT_Thread, 0, initArg, false, false, true);
	}
	
}

int main()
{

	dll_hModule = LoadLibraryA("BlackBone_DLL.dll");
	std::cout << "dll_hModule: " << dll_hModule << std::endl;

	Init(dll_hModule);

	DriverHandle = Initialize();

	std::cout << "DriverHandle: " << DriverHandle << std::endl;

	std::cout << InjectDll << std::endl;
	
	unlink_ = GetPrivateProfileInt(L"z.smliexx.ml", L"unlink", 0, L"./Inject.ini");
	erasePE_ = GetPrivateProfileInt(L"z.smliexx.ml", L"erasePE", 0, L"./Inject.ini");

	GetPrivateProfileString(L"z.smliexx.ml", L"ProcessWindowName", L"-1", ProcessWindowName, MAX_PATH, L"./Inject.ini");
	GetPrivateProfileString(L"z.smliexx.ml", L"DllPath", L"-1", szValue, MAX_PATH, L"./Inject.ini");

	Windowhwnd = FindWindow(NULL, ProcessWindowName);

	std::cout << Windowhwnd << std::endl;

	GetWindowThreadProcessId(Windowhwnd, &pid);

	std::cout << "Pid: " << pid << std::endl;

	std::cout << "unlink: " << unlink_ << std::endl;
	std::cout << "erasePE: " << erasePE_ << std::endl;

	inject();

	if (DriverHandle != NULL)
	{
		CloseHandle(DriverHandle);
	}

	Sleep(300);
	exit(0);

	return 0;
}