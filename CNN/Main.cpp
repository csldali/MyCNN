#include "MyCNN.hpp"
#include "multiThread.h"

int main()
{
	cout << "start at " << Math_Util::getDateTime() << endl;
	SetText(FG_HL | FG_G | FG_B);
	try {
		//���������ļ��еĲ�������
		Param::loadINI("set.ini", "MyCNN");
		MyCNN cnn(Param::outputNums, 28);	//���ģ����,����ͼ��С
		cout << "��������ѵ�����ݺͲ�������...";
		time_t t_start = clock();
		if (Param::multiThread) { //��������˶��߳�,�������̶߳�ȡ���Լ�
			MyCNN_Index cnn_index = { &cnn,Load_Test };
			_beginthread(MyCNN_Thread, 0, &cnn_index);
			cnn.loadTrain(Param::trainFile, Param::trainSetNum);
			waitForFinish();
		}
		else {
			cnn.loadTrain(Param::trainFile, Param::trainSetNum);
			cnn.loadTest(Param::testFile, Param::testSetNum, Param::testHaveTag);
		}
		cout << "\r����ѵ�����ݺͲ������ݺ�ʱ: " << (clock() - t_start) << "ms" << endl;
		t_start = clock();
		cnn.startCNNLearn(1, "BestPop.bak");
		cout << "\n�ݻ�����ʱ: " << (clock() - t_start) / 1000.0 << " s" << endl;
		t_start = clock();
		cout << "�Բ��Լ���ʶ����ȷ��Ϊ: " << cnn.compareTestOut() << endl;
		t_start = clock();
		cnn.printCSToFile("CS.txt");
	}
	catch (const char* str) {
		cout << "\n--error:" << str << endl;
	}
	catch (...) {
		cout << "\nOops, there are some jokes in the runtime \\(�s-�t)/" << endl;
	}
	return 0;
}
