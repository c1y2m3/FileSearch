#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#include <process.h>

#include <Windows.h>

using namespace std;

#pragma comment(lib,"ws2_32.lib")


#define MAX_DATA_BLOCK_SIZE 8192

void error_exit(const char * msg, int val) {
	if (msg) { printf("%s\n\n", msg); }
	printf("使用方法：ft_server [监听端口]\n");
	printf("监听端口是可选参数，默认为8888\n\n");
	exit(val);

}
void serve_client(void *s) {
	printf("\n\n\n创建新线程成功！\n\n\n");
	char file_name[MAX_PATH];
	unsigned char pass[MAX_PATH] = { 0 };
	char data[MAX_DATA_BLOCK_SIZE];
	int i;
	char c;
	FILE *fp;


	printf("接收文件名。。。。\n");
	memset((void *)file_name, 0, sizeof(file_name));
	for (i = 0; i < sizeof(file_name); i++) {
		if (recv(*(SOCKET *)s, &c, 1, 0) != 1) {
			printf("接收失败或客户端已关闭连接\n");
			closesocket(*(SOCKET *)s);
			return;
		}
		if (c == 0) {
			break;
		}
		file_name[i] = c;
	}
	if (i == sizeof(file_name)) {
		printf("文件名过长\n");
		closesocket(*(SOCKET *)s);
		return;
	}
	printf("文件名%s\n", file_name);
	fp = fopen(file_name, "wb");
	if (fp == NULL) {
		printf("无法以写方式打开文件\n");
		closesocket(*(SOCKET *)s);
		return;
	}
	printf("接收文件内容");
	memset((void *)data, 0, sizeof(data));
	for (;;) {

		i = recv(*(SOCKET *)s, data, sizeof(data), 0);
		putchar('.');
		if (i == SOCKET_ERROR) {
			printf("\n接收失败，文件可能不完整\n");
			break;
		}
		else if (i == 0) {
			printf("\n接收成功\n");
			break;
		}
		else {
			fwrite((void *)data, 1, i, fp);
		}
	}
	fclose(fp);
	closesocket(*(SOCKET *)s);



	_endthread();

}


void print_socket_detail(SOCKET s) {
	struct sockaddr_in name;
	int namelen;
	namelen = sizeof(name);
	memset(&name, 0, namelen);
	getsockname(s, (struct sockaddr*)&name, &namelen);
	printf("local:%s:%d\n", inet_ntoa(name.sin_addr), ntohs(name.sin_port));
	namelen = sizeof(name);
	memset(&name, 0, namelen);
	getpeername(s, (struct sockaddr*)&name, &namelen);
	printf("peer:%s:%d\n", inet_ntoa(name.sin_addr), ntohs(name.sin_port));
}

void serve_at(u_short port) {
	WSADATA wsaData;
	SOCKET ls, as;
	static SOCKET *a;
	struct sockaddr_in addr;
	struct sockaddr_in cli_addr;
	int cli_addr_len;
	WSAStartup(0x202, &wsaData);
	ls = socket(AF_INET, SOCK_STREAM, 0);
	memset((void *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	addr.sin_port = htons(port);
	bind(ls, (struct sockaddr *)&addr, sizeof(addr));
	listen(ls, SOMAXCONN);
	printf("服务器已启动，监听于端口%d\n", port);
	for (;;) {
		cli_addr_len = sizeof(cli_addr);
		memset((void *)&cli_addr, 0, cli_addr_len);
		as = accept(ls, (struct sockaddr *)&cli_addr, &cli_addr_len);
		a = &as;
		printf("客户端%s:%d已连接\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		_beginthread(serve_client, 0, (void *)a);
		Sleep(1000);
		print_socket_detail(as);
		// while(1){}
	}
	closesocket(ls);
	WSACleanup();
}

int main(int argc, char ** argv) {
	u_short port;
	if (argc == 1) {
		serve_at(8888);
	}
	else if (argc == 2) {
		port = (u_short)atoi(argv[1]);
		if (port == 0) {
			error_exit("非法的监听端口", -1);
		}
		else {
			serve_at(port);
		}
	}
	else {
		error_exit("参数错误", -1);
	}
	return 0;
}