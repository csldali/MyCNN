#pragma once
#ifndef _MULTITHREAD_H_
#define _MULTITHREAD_H_

#include "MyCNN.hpp"
#include "Win_Util.h"

///�����ǶԶ��߳�ִ�е�֧��ģ��
typedef struct {
	MyCNN *cnn;
	int index;	//�����������������ǹ���
} MyCNN_Index;

enum {
	Load_Test = 1,
	Mutate_Half = 2
};
extern volatile bool thread_state;//���߳�״̬

//�ȴ����߳̽���
void waitForFinish();

//MyCNN���һ�����̸߳������㷽��
RETURN_TYPE MyCNN_Thread(void* para);

#ifndef _MSC_VER
#include"multiThread.cpp"
#endif

#endif //_MYCNN_HPP_