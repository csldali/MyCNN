/** ��������Ӧ����ѧϰ����ģ��
	���༭ʱ��: 2015.4.2  21:00
    �ĵ�˵��: ����Ҫ��������ݼ������ݻ�,���ҵ����ʵ�·��ʵ�ֻ���ѧϰ
    ��������: mlDimension��������ģ�������ά��;numPerCol����ģ��ÿ�и���,
        ����Dimension�������Զ�����;chromLength�������峤��,��·������;
        popSize����ͬʱ�����ݻ�����Ⱥ��С��MaxChromLength���������峤��;
        MaxGen��������ݻ�����;Exp����������ָ��;dataLen���ݼ�����(����);
        bestChrom������ø���;peout����������ͨ��·��������ֵ;dataSet�������ݼ�
    ���θ�������:
		2015:
		(8.24)������ѵ�����Ͳ��Լ��ľ�̬�����Ϊvector
		(7.26)�������峤������12,�þ������4λ0-1����������ʾ0-15
		(3.31)������߼���������(һ����Ԫ),ģ���ڲ����ݷ�Χ��0~255��Ϊ0~1
		2014:
		(11.22)�淶��������:
			popsize=>popSize,individual=>Individual,chromlength=>chromLength,
			fitnessvalue()=>fitnessValue(),findbest()=>findBest()
		(10.28)�޸Ĳ��ֱ��ʽ����һЩ�̶�������Ϊ�ɵ�����OP_NUMS��
		(6.08)��ΪC++�����ʽ��װ
		(5.14)��������,����Ӧ��ͬ����µĻ���ѧϰ
		(4.10)�������߳�,�ڳ���ѯ�ʶԻ���ʱ��̨��Ȼ�ݻ�
		(3.24)����ݻ��ٶ���ʱ����,�޷����û�����ֹͣ������
    @Author: �Ų�(webary)
**/
#pragma once
#ifndef  _ADAMACHINELEARNING_HPP_
#define  _ADAMACHINELEARNING_HPP_

#define _USE_MATH_DEFINES	//ʹ��math.h���һЩ��������
#include <iostream>
#include <string>		//getline()����
#include <fstream>		//getline()����
#include <iomanip>		//setw(n)
#include <vector>
#include <cstdlib>		//rand()����
#include <cmath>		//��ѧ����
#include <ctime>
#include <cstring>
#include <windows.h>
#include <process.h>	//_beginthread()����
#include "math_util.hpp"
#include "Win_Util.h"
using namespace std;
typedef const char cchar;

#ifdef __AFXWIN_H__
extern int testOut[1000];
extern char ml_info[10000];
#endif
extern int flag_stop;

typedef unsigned uint;
typedef vector<float> vectorF;
class Ada_ML : virtual public Math_Util {
protected:
	int mlDimension, numPerCol, col, chromLength;
	const static int OP_NUMS = 8, OP_NUMSx3 = 3*OP_NUMS;	//ģ�����������
	const static int MaxDimension = 300;//��������ܽ��ܵ���������ά��
	const static int MaxCol = 15;		//�����������
	const static int popSize = 3;		//��Ⱥ��ģ
	const static int MaxChromLength = (MaxDimension+OP_NUMSx3-1)/OP_NUMSx3*OP_NUMSx3*MaxCol+3*10;
	const static int MaxGen = 100000000;//�ݻ�ѧϰ������ݻ�����
	const static int Exp = 2;			//����ָ��
	const static int Max_MatInput = (MaxDimension+OP_NUMSx3-1)/OP_NUMSx3 * OP_NUMS;//�������������
	int outPutModels;	//���ģ����
	typedef int ModelMatrixType;		//ģ�������ֵ������
	ModelMatrixType PE[Max_MatInput][MaxCol];//ģ������ֵ
	int best,bestChrom[MaxChromLength];	//��ø��������,��ø���
	int dataLen, **PEout;		//���ݼ�����,�ܸ���,ģ���������

	typedef struct {
		vectorF data;			//�������ݲ���
		int tag;				//�����ǩ(1 or 0 or ...)
	} S_Input;					//��������
	vector<S_Input> matTrain;	//ѵ����-��������ѵ�����ݺͱ�ǩ��Ϣ
	vector<S_Input> matTest;	//���Լ�-��������ѵ������(Ҳ�ɰ�����ǩ��Ϣ)
	//typedef float DataType;	//ģ���ܹ����ܵ���������
	//DataType dataSet[5000][MaxDimension+1], testSet[5000][MaxDimension];//���ݼ������Լ�
	float F,CR;	//����ϵ��,��������
	float wait,permitError;//�ȴ�ʱ��,�������
	typedef struct _Individual {
		unsigned char chrom[MaxChromLength];	//ע��˴�����Ϊchar���͵ı�Ҫ��(�ڴ�ռ�ø�С)
		float ww[Max_MatInput][MaxDimension+1];	//��ǰ���ò���Ϊ[0~mlDimension-1][0~mlDimension]
		bool  wwOut[Max_MatInput];
		double fitness;
	} Individual;
	Individual pop[popSize], cBest;
	int popRand[3];	//���ڱ���ǰ���������������
	//��¼�Ƿ񱣴����и���������浱ǰ���·�����������·�����Ƿ�������
	bool b_saveTmp, b_savePre, b_saveBest, b_mustEqual;
#ifdef __AFXWIN_H__
	CStatusBar *pS;		//MFC�����ڵ�״̬��ָ��
#endif

public:
	Ada_ML(int MLDimension,bool _mustEqual=1,int _col=6);
	virtual ~Ada_ML();
	//��ʼ����ѧϰ
	virtual void startLearn(int len=0,double permit_error=8,cchar *readFromFile=0,bool b_saveData=false);
#ifdef __AFXWIN_H__
	//��MFC���޸�״̬������ʾʵʱѧϰ����
	virtual void startLearn(int len=0,CStatusBar *p,double permit_error=8,cchar* readFromFile=0,bool b_saveData=false);
#endif
	//���ļ�����ѵ����
	int loadTrainSet(const string &fileName, int size = -1);
	//���ļ�������Լ�
	int loadTestSet(const string &fileName, int size=-1, bool haveTag=0);
	//�õ����Խ�������浽int testOut[]������
	double getTestOut(const string &fileName, int len = -1,int print01Ratio = -1);
	//����ָ������dataRow���ض�����PEģ��˵����
	int getPEOut(Individual& indiv, const vectorF &dataRow);
	//����ָ����������ø���PEģ��˵õ������
	int getBestPEOut(const vectorF &dataRow);
	//�����ɸ���indiv�����������������������
	int getMatOut(const Individual& indiv);
	//������ø���
	Individual& getBest() {
		return cBest;
	}
	//����PEģ�������
	const int& getCol()const {
		return col;
	}
	//���ظ��峤��
	const int& getChromlen()const {
		return chromLength;
	}
	static int getPopSize() {
		return popSize;
	}
	int getTestLen()const {
		return matTest.size();
	}
	//���û���ѧϰ�Ĳ���
	void setML(int MLDimension,bool _mustEqual=1,int _col=8);
	//����ѯ�ʵȴ�ʱ���ģ��˵�����
	void setWaitCol(double wt,int _col=0);
	//�������ģ����
	void setOutputModels(int outModels);
	//���ò��������Ƿ񱣴�
	void setSave(bool saveBest = 0,bool saveTmp = 0,bool savePre = 0);
	//����ĳ������
	void savePopMatrix(const char* file,int index,ios::openmode mode=ios::out);
protected:
	//���ļ�������������
	int loadInputData(const string &fileName, int size, vector<S_Input> &dataVec, bool haveTag);
	//���ļ������ݱ��浽����
	int  loadBest(cchar *filePath);
	//��ʼ������ѧϰģ��
	void init(cchar *readFromFile = 0,bool b_saveData = false);
	//��X,Y����ģ���ָ��������������
	int  operationOnXY(int x,int y,int op);
	//�����ii���������Ӧֵ
	void fitnessValue(Individual& indiv, int ii);
	//�ҵ���ø���
	int  findBest();
	//��˹�Ͷ�㷽ʽ�������ľ��󲿷ֺ������粿��
	void mutateMatrixNNByGauss(Individual& tmp);
	//�ڱ���ǰ����������ͬ��ii����������ڽ��沽��
	void generatePopRand(int ii);
	//��ַ�ʽ�������ľ��󲿷ֺ������粿��
	void mutateMatrixNNByDE(Individual& tmp,int ii,int *_popRand=0);
	//������첢ѡ����һ������,Ҫ����Ⱥ������Ҫ3������
	void mutate(int ii,int *_popRand=0);
	//������ii�Ÿ�����indiv��ѡ����ʤ���������һ��
	void selectPop(Individual& indiv,int ii);
	//������������ii������Ե�row�����ݵ����
	void getNNOut(Individual& indiv,const vectorF &dataRow);
	//�������и���ĳһά��Ϊ�����Χ�ڵ�ֵ
	int getChromRandValue(int i);
};

#ifdef __cplusplus
extern "C" {
//�����������߳̽��к�̨������ж�
RETURN_TYPE waiting(void* tt);
} 
#endif

#ifndef _MSC_VER
#include"adaMachineLearning_w.cpp"
#endif

#endif // _ADAMACHINELEARNING_HPP_
