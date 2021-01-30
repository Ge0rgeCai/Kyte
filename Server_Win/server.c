#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <io.h>

#pragma comment (lib, "ws2_32.lib")

#define SERVER_PORT 10086
#define BUFFER_SIZE 1024
#define MAX_FILE_NAME_SIZE 256
#define DOWNLOAD_DIR	"Download_Dir"

void mergeFile(FILE* file, char* buffer, SOCKET new_serverSocket, unsigned long size);						//合并文件	

int main() {
	//判断下载文件夹是否存在
	if (_access(DOWNLOAD_DIR, 6) < 0) {
		if (mkdir(DOWNLOAD_DIR) < 0) {
			printf("SYSTEM:Initializing Failed!\n");
			return -2;
		}
	}
	//1.请求协议版本
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		printf("SYSTEM:Create Protocol Failed!\n");
	}
	printf("SYSTEM:Create Protocol Successfully!\n");
	//2.创建Socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == serverSocket) {
		printf("SYSTEM:Create Socket Failed!\n");
		WSACleanup();
		return -2;
	}
	printf("SYSTEM:Create Socket Successfully!\n");
	//3.创建地址族
	SOCKADDR_IN serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;									//协议版本
	serverAddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);				//本机ipv4地址
	serverAddr.sin_port = htons(SERVER_PORT);							//端口
	//4.绑定
	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) {
		printf("SYSTEM:Server Bind Port: %d Failed!\n", SERVER_PORT);
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}
	printf("SYSTEM:Server Bind Port: %d Successfully!\n", SERVER_PORT);
	//5.监听 (serverSocket用于监听)
	if (listen(serverSocket, 20)) {
		printf("SYSTEM:Server Listen Failed!\n");
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}
	printf("SYSTEM:Server Listen Successfully!\n");
	//6.服务端一直运行等待客户端连接
	while (1) {
		SOCKADDR_IN clientAddr = { 0 };
		int clientAddr_length = sizeof(clientAddr);
		SOCKET new_serverSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddr_length);
		if (new_serverSocket < 0) {
			printf("SYSTEM:Server Accept Failed!\n");
			closesocket(new_serverSocket);
			break;
		}
		printf("SYSTEM:Client %s Connect Successfully!\n", inet_ntoa(clientAddr.sin_addr));
		//7.接收数据
		unsigned long size;
		int r;
		char fileName[MAX_FILE_NAME_SIZE];
		//接收文件名
		r = recv(new_serverSocket, fileName, MAX_FILE_NAME_SIZE, 0);
		if (r < 0) {
			printf("SYSTEM:Receive Data Failed!\n");
			closesocket(new_serverSocket);
			continue;
		}
		printf("收到文件:%s.\n",fileName);

		//接收文件大小
		r = recv(new_serverSocket, (char*)&size, sizeof(unsigned long), 0);
		if (r < 0) {
			printf("SYSTEM:Receive Data Failed!\n");
			closesocket(new_serverSocket);
			continue;
		}
		printf("文件大小:%d.\n",size);
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		//文件夹路径字符串拼接
		char* separator = "\\";
		char* download_dir = DOWNLOAD_DIR;
		char* path = (char*)malloc(strlen(download_dir) + strlen(separator));
		strcpy(path, download_dir);
		strcat(path, separator);
		char* downloadDir_fileName = (char*)malloc(strlen(path) + strlen(fileName));
		strcpy(downloadDir_fileName, path);
		strcat(downloadDir_fileName, fileName);

		FILE* file;
		fopen_s(&file, downloadDir_fileName, "wb+");
		//接收文件
		mergeFile(file, buffer, new_serverSocket, size);
	}

	closesocket(serverSocket);
	WSACleanup();

	return 0;
}

void mergeFile(FILE* file, char* buffer, SOCKET new_serverSocket, unsigned long size) {
	int r;
	int file_length;
	int received_file_length = 0;
	//判断如果在serverSocket中任然能够收到报文时，并且不发生错误，则一直接受来自serverSocket中的信息
	while ((r = recv(new_serverSocket, buffer, BUFFER_SIZE, 0)) > 0) {
			//如果少加了圆括号r总是为“1”，因为">"的优先级高于 "="
		buffer[r] = "\0";
		if (r < BUFFER_SIZE) {
			if (file_length = fwrite(buffer, sizeof(char), r, file) < r) {
				printf("FILE:Write Failed!\n");
				return;
			}
			memset(buffer, 0, BUFFER_SIZE);
			break;
		}
		if (file_length = fwrite(buffer, sizeof(char), BUFFER_SIZE, file) < r) {
			printf("FILE:Write Failed!\n");
			return;
		}
		memset(buffer, 0, BUFFER_SIZE);
	}
	fclose(file);
	printf("FILE:File Transfer Completed!\n");

	return;
}
