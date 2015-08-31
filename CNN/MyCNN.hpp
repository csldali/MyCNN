#pragma once
#ifndef _MYCNN_HPP_
#define _MYCNN_HPP_

#include "CNN_Base.hpp"
#include "adaMachineLearning_w.hpp"
#include "Param.hpp"

/**
@usage:������������CNN_Base�����������Ada_ML����������������ľ��������ģ��
**/
class MyCNN : public Ada_ML, public CNN_Base {
	static const unsigned CNNPopSize = popSize;//������������
	static double F_CNN, F_Matrix;				//�������ı�������,��������ı�������
	uint bestPopIndex;							//��ø��������
public:
	//�ұ������������CNN���������
	MyCNN(uint modelNums = 1, uint _inputSize = 32, bool _mustEqual = 1);
	virtual ~MyCNN();
	//��ʼ�������ѧϰ������ͨ�������ѧϰ��Ϊ����һ����
	void startCNNLearn(double permit_error = 8, cchar *readFromFile = 0);
	//�õ����Լ������
	void getTestOut();
	//�ԱȲ��Լ��������ȷ��---ǰ������֪ÿ��������ݵ����
	float compareTestOut();
	//���²�---�����������������ٶ�
	void updateLayer()
	{
		for (uint j = 0; j < popSize; ++j) {
			mutate(j);
			updatePerLayer(cnnTrain[0].data, cnnPop[j]);	//���¾������
		}
	}
	//������ø���ľ�������ѵ������
	void saveBestCnnPop(const char* file);
	//������ø���ľ����������
	void saveBestMatPop(const char* file);
	//���浱ǰ��Ⱥ��ø���Ŀ��ݻ��������ļ���
	void saveBestToFile(const char* file);
	//���ļ������뱸�ݺõ�����
	void loadBestPop(cchar *filePath);
	//����������ִ�б���,����������粿�ֺ;������粿��;����Ϊ���������߳̿ɷ���
	void mutate(uint index);
protected:
	//ʹ�ø�˹����Ͷ����칹�����¸���---mutate��һ��ʵ�ַ�ʽ
	void mutateByGauss_MultiPoint(CNNIndividual& _cnnPop, Individual& _pop);
	//�Ӿ������õ������Ϊ�������������,�������_cnnPop��_pop��Ϻ����Ӧֵ
	float getFitValue(CNNIndividual& _cnnPop, Individual& _pop);
	//�ҵ���ǰ��ø���
	int findBest()
	{
		bestPopIndex = 0;
		for (uint i = 1; i < popSize; ++i)
			if (cnnPop[i].fitValue < cnnPop[bestPopIndex].fitValue)
				bestPopIndex = i;
		return bestPopIndex;
	}
};

#ifndef _MSC_VER
#include"MyCNN.cpp"
#endif

#endif //_MYCNN_HPP_