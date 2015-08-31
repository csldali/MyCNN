#include "multiThread.h"

volatile bool thread_state = 0;//���߳�״̬
//�ȴ����߳̽���
void waitForFinish()
{
	while (!thread_state)
		Sleep(10);
}

//MyCNN���һ�����̸߳������㷽��
RETURN_TYPE MyCNN_Thread(void* para)
{
	thread_state = 0;	//���δ���
	MyCNN_Index cnn_index = *(MyCNN_Index*)para;
	if (Param::multiThread) {
		switch (cnn_index.index) {
		case Load_Test: //������Լ�
			cnn_index.cnn->loadTest(Param::testFile, Param::testSetNum, Param::testHaveTag);
			break;
		case Mutate_Half:
			int popSize = cnn_index.cnn->getPopSize(), i;
			for (i = popSize / 2; i < popSize; ++i)
				cnn_index.cnn->mutate(i);
			break;
		}
	}
	thread_state = 1;	//��������
	return RETURN_VALUE;
}
