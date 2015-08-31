/** ������粿�ֿ�ʼ-->
	7.16: ������̼߳�������;����������ļ��������
          ������˹���ȥ��;��CNN��S5��Ϊ�ұ����������
	7.15: ��˹�����м��벽��;�����Զ����ݺ����뱸�ݵĹ���
**/

#pragma once
#ifndef _CNN_BASE_HPP_
#define _CNN_BASE_HPP_

#include"math_util.hpp"
#include<iostream>
#include<fstream>
#include<iomanip>
#include<vector>
#include<cfloat>
using namespace std;
typedef unsigned uint;
typedef vector<float> vectorF,biasType;
typedef vector<vectorF> vectorF2D,kernelType;
typedef vector<vector<unsigned> >  vectorU2D;
typedef vector<vector<bool> >  vectorB2D;
/**
	���������ֻ࣬����������粿�ֵ����ԺͲ���
**/
class CNN_Base : public Math_Util{
protected:
    enum SS_Mode {SSM_Mean, SSM_Max, SSM_Min};	//�ǲ���ģʽ.subSample Mode

    static const uint kernelNum 	= 75;//����˸���
    static const uint kernelSize 	= 5;//����˴�С
    static const uint subSampleSize = 2;//�ǲ����˴�С
    static const float Max_kernel,Min_kernel, Max_bias,Min_bias;	//����˺�ƫ�õķ�Χ
    //����ͼ������Ĵ�С�ɾ���˴�С���ǲ�����С����
    //static const uint inputSize = kernelSize*(subSampleSize*(subSampleSize+1)+1)-subSampleSize-1;
    uint inputSize;
    static const uint CS_NUM[6];		//ÿ��feature Map��
    vector<vectorF2D> C1,S2,C3,S4,C5,S6;//C1~S6��
    typedef struct {
        vectorF2D data;					//������������ͼ���ݲ���
        int tag;						//������������ͼ��ǩ(��or�� or ...)
    } S_CNNInput;						//���������������
    vector<S_CNNInput> cnnTrain;		//�������ѵ����-��������ͼ��ĻҶ����ݺͱ�ǩ��Ϣ
    vector<S_CNNInput> cnnTest;			//���������Լ�-��������ͼ��ĻҶ�����(Ҳ�ɰ�����ǩ��Ϣ)
    typedef struct {
        kernelType c_kernel[kernelNum];	//�����
        biasType bias;					//ƫ�ò���
        float stepInGuass[2];			//��˹�����еĲ���(�ֱ���ƾ���˺�ƫ��)
        float fitValue;					//��Ӧֵ
    } CNNIndividual;					//�����������еĸ�������,�������п�ѵ�������ļ��ϼ�Ϊһ������
    vector<CNNIndividual> cnnPop;		//���������弯��
public:
    CNN_Base(uint _inputSize=32,uint _popSize=4);
    virtual ~CNN_Base();
    //�����µ�����ͼ
    void addTrain(const vectorF2D& grayData,int tag);
    //���ļ�����ѵ������
    void loadTrain(const char* file,int size=0);
    //���ļ������������
    void loadTest(const char* file,int size=0,bool haveTag=0);
    //��inputData��Ϊ�������C1~S6ÿ��feature Map��ֵ
    void updatePerLayer(const vectorF2D& inputData, const CNNIndividual& indiv);
    //�õ�CNN�����һ������
    template<typename T>
    void getCNNOut(T& output)
    {
        for(uint i=0; i<CS_NUM[4]; ++i)
            output[i] = C5[i][0][0];
    }
    //�Կ�ѵ������(����˺�ƫ��)ִ�б���
    virtual void kernelBiasMutate();
    //�����������ͼ��ӡ���ļ���
    void printCSToFile(const char* file);
    //����ǰ��ø���Ŀ��ݻ��������浽�ļ���
    virtual void saveBestToFile(const char* file)=0;
protected:
	//�����������ݣ���loadTrain��loadTest����
	int loadInput(const char* file,int size,vector<S_CNNInput> &dataVec,bool haveTag);
    //����CS����Ĳ�������,����ʼ��ÿ������ľ���˺�ƫ�ò���
    void constructCS();
    //����C3��,S2��C3��ӳ���е㸴��,�Բ�ͬ������¿��ܻᱻ��д.C3����CS_NUM[2](=10)������ͼMap,
    //ȡ5~14�ž������S2����ӦMap���о��...����ļ���ֻ�ʺ�S2��5��Map,C3��10��Map�����
    void updateC3Layer(const kernelType* c_kernel,const biasType& bias);
	//ͨ��ȫ������ʽ����C3��,ÿһ��C3��ͼ��ÿһ��S2���ͼ����Ҫһ�������
	void updateC3AllConnect(const kernelType* c_kernel,const biasType& bias);
    //����C5��,C5����CS_NUM[4](=20)������ͼMap,ȡ0~20�ž���˷ֱ���S4��ÿ��Map���о��,C5��ÿ��Mapֻ��һ��ֵ
    void updateC5Layer(const kernelType* c_kernel,const biasType& bias);
    //����S6��,S6����CS_NUM[5](=11)������ͼMap,S6��ÿ�����������ֵ��C5�����ȡ����Mapȡƫ����͵õ�
    void updateS6Layer(const biasType& bias);
protected:	//��̬��������
    //��out��vec1��ӱ��浽output
    static void addVector(vectorF2D& output,const vectorF2D& vec1);
	template<typename...Args>
	static void addVector(vectorF2D &out,const vectorF2D &vec1,const Args...args){
		addVector(out,vec1);
		addVector(out,args...);
	}
    //��һ������ͼinput������kernelִ�о�������ټ�ƫ��bias,�õ���Ӧ������ӳ��ͼoutput
    static void convoluteMap(vectorF2D& output,const vectorF2D& input,const kernelType& kernel,float _bias);
    //��һ������ͼinputͨ��mode��ʽ�²������ټ�ƫ��bias,�õ���Ӧ��������ȡͼoutput
    static void subSampleMap(vectorF2D& output,const vectorF2D& input,float _bias,int mode = SSM_Mean);
    //�õ���vec[row][col]λ�ÿ�ʼ������kel�����Ľ��
    static float getConvoluteOut(const vectorF2D& vec, const kernelType& kernel, int row, int col)
    {
        uint i,j;
        float result = 0;
        for(i=row; i<kernelSize+row && i<vec.size(); ++i)
            for(j=col; j<kernelSize+col && j<vec[i].size(); ++j)
                result += vec[i][j]*kernel[i-row][j-col];
        return result;
    }
    //�õ���vec[row][col]λ�ÿ�ʼ���ǲ������
    static float getSubSampleOut(const vectorF2D& vec, int row, int col,int mode = SSM_Mean)
    {
        uint i,j;
        float result;
        if(mode==SSM_Min)
            result = FLT_MAX;
        else if(mode==SSM_Max)
            result = -FLT_MAX;
        else
            result = 0;
        for(i=row; i<subSampleSize+row && i<vec.size(); ++i)
            for(j=col; j<subSampleSize+col && j<vec[i].size(); ++j) {
                if(mode == SSM_Mean)
                    result += vec[i][j];
                else if(mode == SSM_Max) {
                    if(result<vec[i][j])
                        result = vec[i][j];
                } else if(mode == SSM_Min) {
                    if(result>vec[i][j])
                        result = vec[i][j];
                }
            }
        if(mode==SSM_Mean)
            result /= (subSampleSize*subSampleSize);
        return result;
    }
	//��һ����ͼ��������Mapͼÿ������sigmoid
	static void sigmoidMap(vectorF2D &vec)
	{
		for(uint i=0;i<vec.size();++i)
			for(uint j=0;j<vec[i].size();++j)
				vec[i][j] = (float)sigmoid(vec[i][j]);
	}
	//��һ����άvector������ȫ�����,ÿ��Ԫ�ؾ���Ϊ0
	static void zeroVector(vectorF2D &vec)
	{
		for(uint i=0;i<vec.size();++i)
			for(uint j=0;j<vec[i].size();++j)
				vec[i][j] = 0;
	}
};

#ifndef _MSC_VER
#include"CNN_Base.cpp"
#endif

#endif
