#ifndef _WIN_UTIL_H_
#define _WIN_UTIL_H_

#include<windows.h>
///�����ǿ���̨�����ʽ���Ʋ��ֺ궨��
//����ǰ��
#define FG_R	FOREGROUND_RED
#define FG_G	FOREGROUND_GREEN
#define FG_B	FOREGROUND_BLUE
#define FG_HL	FOREGROUND_INTENSITY	//������ʾ
//���ñ���
#define BG_R	BACKGROUND_RED
#define BG_G	BACKGROUND_GREEN
#define BG_B	BACKGROUND_BLUE
#define BG_HL	BACKGROUND_INTENSITY	//������ʾ

#define SetText(attr) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),attr)

#define setBlue() SetText(FG_HL | FG_B)
#define setGreen() SetText(FG_HL | FG_G)
#define setWhite() SetText(FG_HL | FG_R | FG_G | FG_B)
#define setDefault() SetText(FG_R | FG_G | FG_B)

///�������̺߳���ʱ��Ҫ�õ���һЩ������߳�
#ifdef __AFXWIN_H__
#	define HAND AfxGetMainWnd()->m_hWnd
#	define RETURN_TYPE UINT
#	define RETURN_VALUE 0
#else
#	define HAND 0
#	define RETURN_TYPE void
#	define RETURN_VALUE
#endif

///��vs�е�һЩ��������
#ifndef FILE_VS
#	define FILE_VS
#	ifdef _MSC_VER
#		define FILEOPEN(fp,filename,mode) fopen_s(&fp,filename,mode)
#		define FPRINTF fprintf_s
#		define SPRINTF sprintf_s
#		define FSCANF  fscanf_s
#	else
#		define FILEOPEN(fp,filename,mode) fp = fopen(filename,mode)
#		define FPRINTF fprintf
#		define SPRINTF sprintf
#		define FSCANF  fscanf
#	endif // _MSC_VER
#endif // !FILE_VS

#ifdef _MSC_VER
#	include <tchar.h>
#   define LOCALTIME(_Tm,_Time) struct tm __tm;_Tm=&__tm;localtime_s(_Tm,_Time)
#else
#	define _T(x) x
#   define LOCALTIME(_Tm,_Time) _Tm = localtime(_Time)
#endif

#endif // _WIN_UTIL_H_