#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <io.h>

#pragma comment (lib, "ws2_32.lib")

#define SERVER_PORT 10086
#define BUFFER_SIZE 1024
#define MAX_FILE_NAME_SIZE 256
#define DOWNLOAD_DIR	"Download_Dir"

void mergeFile(FILE* file, char* buffer, SOCKET new_serverSocket, unsigned long size);						//�ϲ��ļ�	

int main() {
	//�ж������ļ����Ƿ����
	if (_access(DOWNLOAD_DIR, 6) < 0) {
		if (mkdir(DOWNLOAD_DIR) < 0) {
			printf("SYSTEM:Initializing Failed!\n");
			return -2;
		}
	}
	//1.����Э��汾
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		printf("SYSTEM:Create Protocol Failed!\n");
	}
	printf("SYSTEM:Create Protocol Successfully!\n");
	//2.����Socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == serverSocket) {
		printf("SYSTEM:Create Socket Failed!\n");
		WSACleanup();
		return -2;
	}
	printf("SYSTEM:Create Socket Successfully!\n");
	//3.������ַ��
	SOCKADDR_IN serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;									//Э��汾
	serverAddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);				//����ipv4��ַ
	serverAddr.sin_port = htons(SERVER_PORT);							//�˿�
	//4.��
	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) {
		printf("SYSTEM:Server Bind Port: %d Failed!\n", SERVER_PORT);
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}
	printf("SYSTEM:Server Bind Port: %d Successfully!\n", SERVER_PORT);
	//5.���� (serverSocket���ڼ���)
	if (listen(serverSocket, 20)) {
		printf("SYSTEM:Server Listen Failed!\n");
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}
	printf("SYSTEM:Server Listen Successfully!\n");
	//6.�����һֱ���еȴ��ͻ�������
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
		//7.��������
		unsigned long size;
		int r;
		char fileName[MAX_FILE_NAME_SIZE];
		//�����ļ���
		r = recv(new_serverSocket, fileName, MAX_FILE_NAME_SIZE, 0);
		if (r < 0) {
			printf("SYSTEM:Receive Data Failed!\n");
			closesocket(new_serverSocket);
			continue;
		}
		printf("�յ��ļ�:%s.\n",fileName);

		//�����ļ���С
		r = recv(new_serverSocket, (char*)&size, sizeof(unsigned long), 0);
		if (r < 0) {
			printf("SYSTEM:Receive Data Failed!\n");
			closesocket(new_serverSocket);
			continue;
		}
		printf("�ļ���С:%d.\n",size);
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		//�ļ���·���ַ���ƴ��
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
		//�����ļ�
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
	//�ж������serverSocket����Ȼ�ܹ��յ�����ʱ�����Ҳ�����������һֱ��������serverSocket�е���Ϣ
	while ((r = recv(new_serverSocket, buffer, BUFFER_SIZE, 0)) > 0) {
			//����ټ���Բ����r����Ϊ��1������Ϊ">"�����ȼ����� "="
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
