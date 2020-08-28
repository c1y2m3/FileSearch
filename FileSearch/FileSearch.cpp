#include<stdio.h>
#include<iostream>
//#include "tchar.h"
#include<windows.h>
#include "tchar.h"


void FindFile(char*, char*);
int count = 0;
char * fname;
#define BUFSIZE 256

//using namespace std;

int main(int argc,char* argv[])
{
	char szLogicalDriveStrings[BUFSIZE];
	DWORD iLength;
	int iSub;


	iLength = GetLogicalDriveStringsA(BUFSIZE - 1, szLogicalDriveStrings);
	printf("System Drive are:");
	for (iSub = 0; iSub < iLength; iSub += 4)
	{
		if (GetDriveType((LPCSTR)szLogicalDriveStrings + iSub) != 3)
			continue;
		printf(szLogicalDriveStrings + iSub );
	}

	wprintf(L"\nCommand:FileSearch.exe Drive Keyword\n");
	wprintf(L"Example:FileSearch.exe C:\\ docx\n");
	if (argv[1])
	{
		ZeroMemory(szLogicalDriveStrings, BUFSIZE);
		const char *str7 = argv[1];
		fname = argv[2];
		FindFile((char *)str7, (char *)"*.*");

		//printf("一共%s文件...\n", count);
		return 0;
	}

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