#include "Param.hpp"

int  Param::trainSetNum;	//ѵ��������
int  Param::testSetNum;		//���Լ�����
bool Param::testHaveTag;	//���Լ����������ޱ�ǩ
char Param::trainFile[128];	//ѵ�����ļ�
char Param::testFile[128];	//���Լ��ļ�
bool Param::enableTest;		//��������
float Param::F_CNN,Param::F_Matrix;	//�������;�������ı�������
int Param::MaxGen;		//����ݻ�����
bool Param::multiThread;//�������߳�
bool Param::askLoadBak;		//ѯ�����뱸���ļ�
bool Param::loadBakFile;	//���뱸���ļ�,��ѯ��ʱʹ��
int Param::outputNums;				//���ģ���� 
INI_Util Param::ini;
