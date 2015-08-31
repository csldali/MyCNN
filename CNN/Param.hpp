/**
	������ȡ��
    @Author: �Ų�(webary)
**/
#pragma once
#ifndef  _Param_HPP_
#define  _Param_HPP_

#include "INI_Util.hpp"
#include "Win_Util.h"
#include <cstdlib>

#define _STR(s) #s<<" = "
#define _STR_STR(s) #s<<" = "<<s
class Param{
public:
	static INI_Util ini;
	static int  trainSetNum;	//ѵ��������
	static int  testSetNum;		//���Լ�����
	static bool testHaveTag;	//���Լ����������ޱ�ǩ
	static char trainFile[128];	//ѵ�����ļ�
	static char testFile[128];	//���Լ��ļ�
	static bool enableTest;		//��������
	static float F_CNN, F_Matrix;//�������;�������ı�������
	static int 	MaxGen;         //����ݻ�����
	static bool multiThread;	//�������߳�
	static bool askLoadBak;		//ѯ�����뱸���ļ�
	static bool loadBakFile;	//���뱸���ļ�,��ѯ��ʱʹ��
	static int outputNums;		//���ģ���� 
public:
	//��ini�ļ��������ò���
	static void loadINI(const char *iniFileName="",const char *appName=""){
		ini.loadINI(iniFileName);
		if(appName!=0 && appName[0]!=0)
			ini.setDefaultNode(appName);
		if(ini.isNodeExist(ini.getDefaultNode())==0) {
			writeINI();
			char msg[128];
			SPRINTF(msg,"δ��⵽��ȷ�������ļ�����!\n\n����ȷ���������ļ�'%s'�е�[%s]�ڵ������"
					,iniFileName,appName);
			cout<<msg<<endl;
			throw "δ��⵽��ȷ�������ļ�����!";
		}
		trainSetNum = getInt("trainSetNum");
		testSetNum = getInt("testSetNum");
		outputNums = getInt("outputNums"); 
		MaxGen = getInt("MaxGen");
		testHaveTag = 0!=getInt("testHaveTag");
		enableTest = 0!=getInt("enableTest");
		multiThread = 0!=getInt("multiThread");
		askLoadBak = 0!=getInt("askLoadBak");
		loadBakFile = 0!=getInt("loadBakFile");
		F_CNN = (float)getDouble("F_CNN");
		F_Matrix = (float)getDouble("F_Matrix");
		SPRINTF(testFile,"%s",ini.getRecord("testFile").c_str());
		SPRINTF(trainFile,"%s",ini.getRecord("trainFile").c_str());
	}
	//���������浽ini�ļ�,���ļ�������ʱд��Ĭ��ֵ
	static void writeINI() {
		ofstream write(ini.getLastFileName().c_str(),ios::out|ios::app);
		if(write.is_open()) {
			write<<"\n["<<ini.getDefaultNode()<<"]"<<endl
			<<_STR(trainFile)<<endl
			<<_STR(testFile)<<endl
			<<_STR(trainSetNum)<<endl
			<<_STR(testSetNum)<<endl
			<<_STR(outputNums)<<endl
			<<_STR(testHaveTag)<<1<<endl
			<<_STR(enableTest)<<0<<endl
			<<_STR(F_CNN)<<0.07<<endl
			<<_STR(F_Matrix)<<0.08<<endl
			<<_STR(MaxGen)<<100000000<<endl
			<<_STR(multiThread)<<1<<endl
			<<_STR(askLoadBak)<<1<<endl
			<<_STR(loadBakFile)<<1<<endl;
			write.close();
		}
	}

	static int getInt(const string & str) {
		string res = ini.getRecord(str);
		if(res!="")
			return atoi(res.c_str());
		else
			return 0;
	}
	static double getDouble(const string & str) {
		string res = ini.getRecord(str);
		if(res!="")
			return atof(res.c_str());
		else
			return 0;
	}
	static string getState() {
		return ini.getState();
	}
};

#ifndef _MSC_VER
#include"Param.cpp"
#endif

#endif // _INI_Util_HPP_
