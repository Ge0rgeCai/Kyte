#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#pragma comment (lib, "ws2_32.lib")

#define SERVER_PORT 10086
#define BUFFER_SIZE 1024
#define MAX_FILE_NAME_SIZE 256

unsigned long getFileSize(FILE* file);													//����ļ���С
void splitFile(FILE* file, unsigned long size, char* buffer, SOCKET clientSocket);		//���ļ�����ʱ�����ļ��ָ�ɶ��С��

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: ./%s ServerIpAddress FileName!\n", argv[0]);
		return -2;
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
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == clientSocket) {
		printf("SYSTEM:Create Socket Failed!\n");
		WSACleanup();
		return -2;
	}
	printf("SYSTEM:Create Socket Successfully!\n");

	//3.������ַ��
	SOCKADDR_IN serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;									//Э��汾
	serverAddr.sin_addr.S_un.S_addr = inet_addr(argv[1]);				//����ipv4��ַ
	serverAddr.sin_port = htons(SERVER_PORT);							//�˿�

	//4.����
	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) {
		printf("SYSTEM:Connect Server Failed!\n");
		closesocket(clientSocket);
		WSACleanup();
		return -2;
	}
	printf("SYSTEM:Connect Server Successfully!\n");

	//5.�����˱������ӣ���ͨ��
	char buffer[BUFFER_SIZE];
	FILE* file;
	fopen_s(&file, argv[2], "rb");
	if (file == NULL) {
		printf("FILE:Can Read Such File!\n");
		return -2;
	}
	memset(buffer, 0, BUFFER_SIZE);

	//�����ļ���
	char fileName[MAX_FILE_NAME_SIZE];
	memset(fileName, 0, MAX_FILE_NAME_SIZE);
	strncpy(fileName, argv[2], strlen(argv[2]));
	send(clientSocket, fileName, MAX_FILE_NAME_SIZE, 0);

	//�����ļ���С
	unsigned long size = getFileSize(file);
	send(clientSocket, (char*)&size, sizeof(unsigned long), 0);

	//�ָ��ļ�������
	splitFile(file, size, buffer, clientSocket);
	closesocket(clientSocket);
	fclose(file);
	return 0;
}

//����ļ���С
unsigned long getFileSize(FILE* file) {
	//���ļ�ָ��ƫ�Ƶ��ļ�β��
	fseek(file, 0, SEEK_END);
	//��õ�ǰ�ļ�ָ�뵽�ļ�ͷ�ľ���(�ֽ���)
	unsigned long size =	ftell(file);
	//���ļ�ָ�����(�ص��ļ�ͷ)
	fseek(file, 0, SEEK_SET);
	//�����ļ���С
	return size;
}

//�ָ��ļ�
void splitFile(FILE* file, unsigned long size, char* buffer, SOCKET clientSocket) {
	while (!feof(file)) {	
		//��֤���һ�ηָ��ļ�ʱ����ȡ�ļ���buffer�У�������
		int n = size - ftell(file);
		if (size - ftell(file) < sizeof(char)*BUFFER_SIZE) {
			fread(buffer, sizeof(char), n, file);
			send(clientSocket, buffer, n, 0);
			printf("buffer_last:%s\n", buffer);
			memset(buffer, 0, BUFFER_SIZE);
			break;
		}
		
		//ÿ�ν��ļ�ָ��ƫ��"sizeof(char)*BUFFER_SIZE"���ֽ�
		fread(buffer, sizeof(char), BUFFER_SIZE, file);
		printf("buffer:%s\n", buffer);
		send(clientSocket, buffer, BUFFER_SIZE, 0);
		memset(buffer, 0, BUFFER_SIZE);
	}
	return;
}
