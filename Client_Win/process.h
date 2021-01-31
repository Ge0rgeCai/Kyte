#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//初始化进度条
char* initProcess();
//用于制作进度条
void doProcess(unsigned long file_block_size, unsigned long totall_file_size, unsigned long* handled_file_block, char* bar);					//进度条