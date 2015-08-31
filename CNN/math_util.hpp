#ifndef _MATH_UTIL_HPP_
#define _MATH_UTIL_HPP_

#define _CRT_SECURE_NO_WARNINGS
#include<cmath>
#include<ctime>
#include<cstdlib>
#include<string>
#include "Win_Util.h"
using namespace std;

/**
 * ��ѧ����ͨ����
**/
class Math_Util {
public:
	/* ������������ �����ԣ����������޴ο�΢
	 * |net|��Сʱ��Ȩֵ��С�����ɽ������Ժ�������������������С�ź�
	 * |net|�ϴ�ʱ��Ȩֵ�ϴ󣩣��ɽ�����ֵ����������������������ź�
	 */
	//sigmoid������S�ͺ�����������΢��ֵ��(0��1)��
	static double sigmoid(double x) {
		return 1.0/(1.0+exp(-x));
	}
	//˫������S�ͺ�����������΢��ֵ��(-1��1)��
	static double tansig(double x) {
		return 2 * sigmoid(x) - 1;
	}
	//������һ�����������
	static void setSrand() {
		static bool first = 1;
		if(first) {
			srand(unsigned(time(NULL)));
			first = 0;
		}
	}
	//����һ��_min��_max֮��ľ��ȷֲ������������
	static float randFloat() {
		return (float)rand() / RAND_MAX;
	}
	//����һ��_min��_max֮��ľ��ȷֲ������������
	static float randFloat(double _min, double _max) {
		return (float)(randFloat() * (_max - _min) + _min);
	}
	//����һ����ָΪE����ΪD�ĸ�˹�ֲ������������
	static float randGauss(double E=0,double D=1) {
		float V1, V2=0, S=1, X;
		static bool phase = 0;
		if (phase == 0) {
			do {
				V1 = 2 * randFloat() - 1;
				V2 = 2 * randFloat() - 1;
				S = V1 * V1 + V2 * V2;
			} while (S >= 1 || S < 1e-5);
			X = V1 * sqrt(-2 * log(S) / S);
		} else
			X = V2 * sqrt(-2 * log(S) / S);
		phase = !phase;
		return float(X * D + E);
	}
	//��ȡ��ǰʱ��,����Ϊ�ַ���
	static char* getTime(char* timeStr) {
		time_t now_time=time(0);
		struct tm *newtime=0;
		LOCALTIME(newtime, &now_time);
		strftime(timeStr,10,"%H.%M.%S", newtime);
		return timeStr;
	}
	//��ȡ��ǰ���ں�ʱ��,����Ϊ�ַ���
	static string getDateTime(bool noDate=0, char format=':') {
		char dt[20];
		time_t now_time=time(0);
		struct tm *newtime=0;
		LOCALTIME(newtime, &now_time);
		if(format=='.') {
			if(noDate)
				strftime(dt,20,"%H.%M.%S", newtime);
			else
				strftime(dt,20,"%Y-%m-%d %H.%M.%S", newtime);
		} else {
			if(noDate)
				strftime(dt,20,"%X", newtime);
			else
				strftime(dt,20,"%Y-%m-%d %X", newtime);
		}
		return dt;
	}
	//�е�ģ��
	template<class T1,class T2>
	static bool myEqual(T1 a,T2 b) {
		return (a-b)<1e-5 && (b-a)<1e-5;
	}
	//ʹһ��������ֵ������ĳ����Χ��
	template<typename T1, typename T2, typename T3>
	static void makeInRange(T1& var, T2 low, T3 high, bool edgeToRand=true) {
		if(var<low)
			var = edgeToRand ? randFloat(low,high) : (T1)low;
		else if(var>high)
			var = edgeToRand ? randFloat(low,high) : (T1)high;
	}
	//���ؾ���ֵ
	template<typename T>
	static T myAbs(T num) {
		return num > 0 ? num : -num;
	}
	//ת��ģ�桪��������һ������
	template<class T1,class T2>
	static T1 trans(T2 num,T2 max_T2,T1 stand) {
		return  T1((double)num / max_T2 * stand + .5);
	}
};

#endif // _MATH_UTIL_HPP_
