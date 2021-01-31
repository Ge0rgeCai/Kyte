#include "process.h"

//初始化进度条
char* initProcess() {
	char* bar = (char*)malloc(sizeof(char) * 103);
	//分配内存
	memset(bar, ' ', 103);
	bar[0] = '[';
	bar[101] = ']';
	bar[102] = '\0';

	return bar;
}

//制作进度条
void doProcess(unsigned long file_block_size, unsigned long totall_file_size, unsigned long* handled_file_size, char* bar) {
	int a, i;
	char label[5] = "-\\|/\0";
	*handled_file_size += file_block_size;
	a = totall_file_size / 100;
	if (0 == a) {//防止传输的文件小于100时产生错误
		i = 100;
	}
	else {
		i = *handled_file_size / a;
	}
	if (0 == i) i = 1;
	if (100 <= i) i = 100;
	int temp = i;
	while (temp != 0) {
		bar[temp--] = '#';
	}
	printf(" %s [%d%%] [%c]\r", bar, i, label[i%4]);
	fflush(stdout);
	
	return;
}
