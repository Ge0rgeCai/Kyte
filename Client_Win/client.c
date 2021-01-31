#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include "process.h"

#pragma comment (lib, "ws2_32.lib")

#define SERVER_PORT 10086
#define BUFFER_SIZE 1024
#define MAX_FILE_NAME_SIZE 256

unsigned long getFileSize(FILE* file);													//获得文件大小
void splitFile(FILE* file, unsigned long size, char* buffer, SOCKET clientSocket, char* bar);		//当文件过大时，将文件分割成多个小块

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: ./%s ServerIpAddress FileName!\n", argv[0]);
		return -2;
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
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == clientSocket) {
		printf("SYSTEM:Create Socket Failed!\n");
		WSACleanup();
		return -2;
	}
	printf("SYSTEM:Create Socket Successfully!\n");

	//3.创建地址族
	SOCKADDR_IN serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;									//协议版本
	serverAddr.sin_addr.S_un.S_addr = inet_addr(argv[1]);				//本机ipv4地址
	serverAddr.sin_port = htons(SERVER_PORT);							//端口

	//4.连接
	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) {
		printf("SYSTEM:Connect Server Failed!\n");
		closesocket(clientSocket);
		WSACleanup();
		return -2;
	}
	printf("SYSTEM:Connect Server Successfully!\n");

	//5.与服务端保持连接，并通信
	char buffer[BUFFER_SIZE];
	//初始化进度条
	char* bar = (char*)malloc(sizeof(char) * 103);
	bar = initProcess();
	FILE* file;
	fopen_s(&file, argv[2], "rb");
	if (file == NULL) {
		printf("FILE:Can Read Such File!\n");
		return -2;
	}
	memset(buffer, 0, BUFFER_SIZE);

	//传输文件名
	char fileName[MAX_FILE_NAME_SIZE];
	memset(fileName, 0, MAX_FILE_NAME_SIZE);
	strncpy(fileName, argv[2], strlen(argv[2]));
	printf("File Name:%s\n", fileName);
	send(clientSocket, fileName, MAX_FILE_NAME_SIZE, 0);

	//传输文件大小
	unsigned long totall_file_size = getFileSize(file);
	printf("File Size:%d Byte.\n", totall_file_size);
	send(clientSocket, (char*)&totall_file_size, sizeof(unsigned long), 0);

	//分割文件并传输
	splitFile(file, totall_file_size, buffer, clientSocket, bar);
	closesocket(clientSocket);
	fclose(file);
	return 0;
}

//获得文件大小
unsigned long getFileSize(FILE* file) {
	//将文件指针偏移到文件尾部
	fseek(file, 0, SEEK_END);
	//获得当前文件指针到文件头的距离(字节数)
	unsigned long size =	ftell(file);
	//将文件指针回正(回到文件头)
	fseek(file, 0, SEEK_SET);
	//返回文件大小
	return size;
}

//分割文件
void splitFile(FILE* file, unsigned long size, char* buffer, SOCKET clientSocket, char* bar) {
	unsigned long handled_file_size = 0;
	while (!feof(file)) {	
		//保证最后一次分割文件时，读取文件到buffer中，不出错
		int n = size - ftell(file);
		if (size - ftell(file) < sizeof(char)*BUFFER_SIZE) {
			fread(buffer, sizeof(char), n, file);
			send(clientSocket, buffer, n, 0);
			doProcess(n, size, &handled_file_size, bar);
		//	printf("buffer_last:%s\n", buffer);
			memset(buffer, 0, BUFFER_SIZE);
			break;
		}
		
		//每次将文件指针偏移"sizeof(char)*BUFFER_SIZE"个字节
		fread(buffer, sizeof(char), BUFFER_SIZE, file);
		//printf("buffer:%s\n", buffer);
		send(clientSocket, buffer, BUFFER_SIZE, 0);
		doProcess(BUFFER_SIZE, size, &handled_file_size, bar);
		memset(buffer, 0, BUFFER_SIZE);
	}
	printf("FILE:File Transfer Completed!\n");
	return;
}
