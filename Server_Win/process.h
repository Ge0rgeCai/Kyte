#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//��ʼ��������
char* initProcess();
//��������������
void doProcess(unsigned long file_block_size, unsigned long totall_file_size, unsigned long* handled_file_block, char* bar);					//������