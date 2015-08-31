#include <direct.h>
#include "MyCNN.hpp"
#include "multiThread.h"

double MyCNN::F_CNN, MyCNN::F_Matrix;//��startCNNLearn��Ա�����д������ļ���ȡ��ʼ������

MyCNN::MyCNN(uint modelNums,uint _inputSize,bool _mustEqual): Ada_ML(CS_NUM[4],_mustEqual),CNN_Base(_inputSize,popSize)
{
    bestPopIndex = 0;
    setWaitCol(10);	//����Ĭ��ѯ�ʵȴ����ʱ��
    setOutputModels(modelNums);
}

MyCNN::~MyCNN() {}
//��ʼ�������ѧϰ������ͨ�������ѧϰ��Ϊ����һ����
void MyCNN::startCNNLearn(double permit_error,cchar *readFromFile)
{
    F_CNN = Param::F_CNN;
    F_Matrix = Param::F_Matrix;
    if(cnnTrain.empty()) {
#ifndef  __AFXWIN_H__	//��MFC����
        cout<<"\r��δ��������ͼ����,������Ƿ����������룡"<<endl;
        throw "��ѵ��������,��startCNNLearn���˳���";
        return;
#else		//MFC����
        MessageBox(AfxGetMainWnd()->m_hWnd,"��δ��������ͼ����,������Ƿ����������룡","���ѧϰ����",0);
        if(pS)
            pS->SetPaneText(0,_T("���ѧϰ����--�������0x0001"));
        return;
#endif
    }
    setBlue();
    permitError = (float)(permit_error>0 && permit_error<100 ? permit_error : 8);
    int i, gen = 0;
    matTrain.resize(cnnTrain.size(),{vectorF(mlDimension)});
    if(!cnnTest.empty())
        matTest.resize(cnnTest.size(),{vectorF(mlDimension)});
    for(i = 0; i < popSize; i++)
        PEout[i] = new int[dataLen];
    cout<<"\n���ڳ�ʼ��������...";
    init();	//��ʼ�������粿��
    loadBestPop(readFromFile);
    for(i = 0; i < popSize; ++i)
        getFitValue(cnnPop[i],pop[i]);	//����ÿ���������Ӧֵ
    findBest();
#ifndef  __AFXWIN_H__
    cout<<"\r����ѧϰ"<<cnnTrain.size()<<"��ѵ����,��ѵ�����������..."<<endl;
#else
    if(pS)
        pS->SetPaneText(0,_T("�ѿ�ʼ�ݻ����̣������ĵȴ�..."));
#endif
    setWhite();
    cout<<"\n��ǰ����:\tƽ����ȷ��:\t�˴���ʱ(ms):\t��ǰ������ȷ��:\t��ʷ������ȷ��:"<<endl;
    flag_stop = -1;
    int t1 = clock(), t_start = t1;
    double ind_right = 100;		//ƽ�����
    static double last_ind_right = 0;	//��һ�ε�ƽ����ȷ��
    float bestTestRight = 0;	//��ʷ��ò�����ȷ��
    _mkdir("Gen_TestRight");	//�����ļ���
    char tmpbuf[128];
    SPRINTF(tmpbuf,"Gen_TestRight/Gen_TestRight_%s.txt",getDateTime(0,'.').c_str());
    ofstream saveGen_TestRight(tmpbuf);
    saveGen_TestRight<<"ѵ�����ļ���������"<<Param::trainFile<<"  "<<Param::trainSetNum<<endl;
    saveGen_TestRight<<"���Լ��ļ���������"<<Param::testFile<<"  "<<Param::testSetNum<<endl;
    saveGen_TestRight<<"F_CNN = "<<F_CNN<<",F_Matrix = "<<F_Matrix<<endl<<endl
                     <<"����: \t\tƽ����ȷ��: \t������ȷ��: "<<endl;
    while (gen++<Param::MaxGen) {
        int size = popSize;
        //������̸߳�������һ�����
        if(Param::multiThread) {
            MyCNN_Index cnn_index = {this,Mutate_Half};
            _beginthread(MyCNN_Thread,0,&cnn_index);
            size /= 2;
        }
        for (i=0; i<size; i++)
            mutate(i); //�Ը���ִ�б���
        if(Param::multiThread)
            waitForFinish();
        findBest();
        ind_right = 1-(cnnPop[bestPopIndex].fitValue) / cnnTrain.size();
        cout<<"\r "<<setw(17)<<setiosflags(ios::left)<<gen;
        setGreen();
        cout<<setw(16)<<ind_right;
        setWhite();
        cout<<setw(17)<<clock()-t_start;
        if(ind_right*100 > 100-permitError+1e-5)
            break;
        //flag_stop��ʶ�û�����˶Ի�����ĸ�ֵ��1��ʾȷ����-1��ʾȡ��
        if(clock()-t1>wait && flag_stop==-1)
#ifdef  __AFXWIN_H__
            AfxBeginThread(waiting,&t1);	//MFCר���̺߳���
#else
            _beginthread(waiting,0,&t1);
#endif
        if(flag_stop==1)
            break;
        if(!myEqual(last_ind_right,ind_right)) {
            float testRight = 0;
            //�������ļ���ȡ�Ƿ�Ҫ���в���
            Param::loadINI();
            if(Param::enableTest) {
				testRight = compareTestOut();	//��ǰ��ø���Բ��Լ�����ȷ��
				setGreen();
				cout << setw(17) << testRight;
        		setWhite();
				if (bestTestRight < testRight)
					bestTestRight = testRight;
				if (testRight < bestTestRight)
					cout << bestTestRight;
				else
					cout << "==";
            }
            cout<<endl;	//�ӻ��п�����ÿ�α仯����ʾǰһ�εĴ���
            if(saveGen_TestRight.is_open())
                saveGen_TestRight<<gen<<"\t\t"<<setw(16)<<setiosflags(ios::left)<<ind_right<<testRight<<endl;
            //���浱ǰ��ø���Ŀ��ݻ��������ļ�-��������
            saveBestToFile("BestPop.bak");
            last_ind_right = ind_right;
        }
        t_start = clock();
    }
    if(saveGen_TestRight.is_open())
        saveGen_TestRight.close();
    flag_stop=1;
#ifdef  __AFXWIN_H__
    SPRINTF(ml_info,"�ݻ���� (���ݻ����� %d ����ƽ����ȷ��Ϊ��%.3g)",gen,ind_right);
    if(pS)
        pS->SetPaneText(0,_T(ml_info));
#else
    cout<<endl;
#endif
    for(i = 0; i < popSize; i++)
        delete[] PEout[i];
}
//�õ����Լ������
void MyCNN::getTestOut()
{

}
//�ԱȲ��Լ��������ȷ��---ǰ������֪ÿ��������ݵ����
float MyCNN::compareTestOut()
{
    int error = 0;	//��¼�ȽϺ����ĸ���
    uint i_test;
    for(i_test=0; i_test<cnnTest.size(); ++i_test) {
        updatePerLayer(cnnTest[i_test].data,cnnPop[bestPopIndex]);
        getCNNOut(matTest[i_test].data);
        error += (int)(cnnTest[i_test].tag != getPEOut(pop[bestPopIndex], matTest[i_test].data));
    }
    if(i_test==0) {
        cout<<"��δ������Լ�,�޷����ʶ��"<<endl;
        return -1;
    } else {
        float right = 1 - (float)error/i_test;
        //cout<<"�Բ������ݵ�ʶ����ȷ��Ϊ: "<<right<<"("<<error<<"/"<<i_test<<")"<<endl;
        return right;
    }
}
//������ø���ľ�������ѵ������
void MyCNN::saveBestCnnPop(const char* file)
{
    ofstream outFile(file);
    uint index = bestPopIndex, i, j ,k;
    for(i=0; i<kernelNum; ++i) {
        outFile<<"cnnPop-best"<<"  kernel-"<<i<<endl;
        for(j=0; j<cnnPop[index].c_kernel[i].size(); ++j) {
            for(k=0; k<cnnPop[index].c_kernel[i][j].size(); ++k)
                outFile<<cnnPop[index].c_kernel[i][j][k]<<"  ";
            outFile<<endl;
        }
        outFile<<"��������������������������������������������������������"<<endl;
    }
    outFile<<"bias-"<<index<<endl;
    for(i=0; i<cnnPop[index].bias.size(); ++i)
        outFile<<cnnPop[index].bias[i]<<"  ";
    outFile<<endl;
    outFile.close();
}
//������ø���ľ����������
void MyCNN::saveBestMatPop(const char* file)
{
    ofstream savePre(file);
    if(savePre.is_open()) {
        int i,j,k;
        for(i = 0; i < chromLength; i++)
            savePre<<(int)cBest.chrom[i]<<" ";
        savePre<<endl;
        for(j=0; j<mlDimension; j++)
            for(k=0; k<mlDimension+1; k++)
                savePre<<setw(12)<<setiosflags(ios::left)<<cBest.ww[j][k]<<" ";
        savePre<<endl;
        savePre.close();
    }
}
//���浱ǰ��Ⱥ��ø���Ŀ��ݻ��������ļ���
void MyCNN::saveBestToFile(const char* file)
{
    ofstream outFile(file);
    if(outFile.is_open()) {
        outFile<<"### ������: "<<getDateTime()<<"\t��ȷ��Ϊ: "
               <<1-(cnnPop[bestPopIndex].fitValue) / cnnTrain.size()<<endl<<endl;
        uint i,j,k;
        for(i=0; i<kernelNum; ++i) {
            for(j=0; j<cnnPop[bestPopIndex].c_kernel[i].size(); ++j) {
                for(k=0; k<cnnPop[bestPopIndex].c_kernel[i][j].size(); ++k)
                    outFile<<cnnPop[bestPopIndex].c_kernel[i][j][k]<<"  ";
                outFile<<endl;
            }
            outFile<<endl;
        }
        outFile<<endl;
        for(i=0; i<cnnPop[bestPopIndex].bias.size(); ++i)
            outFile<<cnnPop[bestPopIndex].bias[i]<<"  ";
        outFile<<"\n\n"<<cnnPop[bestPopIndex].stepInGuass[0]
               <<"\t"<<cnnPop[bestPopIndex].stepInGuass[1]<<endl<<endl;
        outFile.close();
        //������󲿷ֵ����ļ�
        savePopMatrix(file,bestPopIndex,ios::out|ios::app);
        //cout<<"��������ļ�"<<file<<"��"<<endl;
    }
}
//���ļ����뱸�ݵ�����
void MyCNN::loadBestPop(cchar *filePath)
{
    ifstream bestPopFile(filePath);
    if(bestPopFile.is_open()) {
        if(MessageBox(0,_T("��⵽�б����ļ�������,�Ƿ����룿"), _T("��ܰ��ʾ"),MB_ICONQUESTION|MB_YESNO)==IDYES) {
            string tmp = "";
            getline(bestPopFile,tmp);	//��ȡ��һ��ע�ͣ�����
            uint i,j,k;
            for(i=0; i<kernelNum; ++i) {
                for(j=0; j<cnnPop[0].c_kernel[i].size(); ++j) {
                    for(k=0; k<cnnPop[0].c_kernel[i][j].size(); ++k)
                        bestPopFile>>cnnPop[0].c_kernel[i][j][k];
                }
            }
            for(i=0; i<cnnPop[0].bias.size(); ++i)
                bestPopFile>>cnnPop[0].bias[i];
            bestPopFile>>cnnPop[0].stepInGuass[0]>>cnnPop[0].stepInGuass[1];
            //������󲿷ֵĲ���
            int read;
            for(i=0; i<chromLength; ++i)
                if(bestPopFile>>read)
                    pop[0].chrom[i] = read;
                else
                    break;
            for(i=0; i<mlDimension; ++i)
                for(j=0; j<mlDimension+1; ++j)
                    if(!(bestPopFile>>pop[0].ww[i][j]))
                        break;
        }
        bestPopFile.close();
    }
}
//����������ִ�б���,����������粿�ֺ;������粿��
void MyCNN::mutate(uint index)
{
    CNNIndividual tmpCnnPop;
    Individual tmpMatPop;
    tmpCnnPop = cnnPop[index];
    tmpMatPop = pop[index];
    mutateByGauss_MultiPoint(tmpCnnPop,tmpMatPop);
    getFitValue(tmpCnnPop,tmpMatPop);	//�õ�����������Ӧֵ
    if(tmpCnnPop.fitValue < cnnPop[index].fitValue) {
        cnnPop[index] = tmpCnnPop;
        pop[index] = tmpMatPop;
    }
}
//ʹ�ø�˹����Ͷ����칹�����¸���---mutate��һ��ʵ�ַ�ʽ
void MyCNN::mutateByGauss_MultiPoint(CNNIndividual& tmpCnnPop, Individual& tmpMatPop)
{
    static const float gaussRange = 1.5;    //��˹����ķ���
    uint i,j,k;
    //�Ծ����ִ�и�˹����
    for(i=0; i<kernelNum; ++i)
        for(j=0; j<kernelSize; ++j)
            for(k=0; k<kernelSize; ++k)
                if(randFloat()<=F_CNN) {
                    tmpCnnPop.c_kernel[i][j][k] += tmpCnnPop.stepInGuass[0]
                                                   * randGauss(0,gaussRange);
                    makeInRange(tmpCnnPop.c_kernel[i][j][k],
                                           Min_kernel,Max_kernel);
                }
    if(randFloat()<=F_CNN) {
        tmpCnnPop.stepInGuass[0] += tmpCnnPop.stepInGuass[0] * randGauss(0,gaussRange);
        makeInRange(tmpCnnPop.stepInGuass[0],Min_kernel,Max_kernel);
    }
    //��ƫ�ò���ִ�и�˹����
    for(i=0; i<tmpCnnPop.bias.size(); ++i)
        if(randFloat()<=F_CNN) {
            tmpCnnPop.bias[i] += tmpCnnPop.stepInGuass[1] * randGauss(0, gaussRange);
            makeInRange(tmpCnnPop.bias[i],Min_bias,Max_bias);
        }
    if(randFloat()<=F_CNN) {
        tmpCnnPop.stepInGuass[1] += tmpCnnPop.stepInGuass[1] * randGauss(0,gaussRange);
        makeInRange(tmpCnnPop.stepInGuass[1],Min_bias,Max_bias);
    }
    //�Ծ�����ߵ���Ԫ����ִ�и�˹����
    for (i = 0; i < (mlDimension+1)*numPerCol; i++)
        if (randFloat() <= F_Matrix) {
            j = i/(mlDimension+1), k = i%(mlDimension+1);
            tmpMatPop.ww[j][k] += randGauss(0, 1);
            makeInRange(tmpMatPop.ww[j][k],-1,1);
        }
    //�Ծ��󲿷�ִ�ж�����
    for (i=0; i<chromLength; i++)
        if(randFloat()<=F_Matrix) {
            if(((i+1)%3)==0)	//ÿ������������Ϊ������������
                tmpMatPop.chrom[i] ^= rand() % OP_NUMS;
            else
                tmpMatPop.chrom[i] ^= rand() % numPerCol;
        }
}
//�Ӿ������õ������Ϊ�������������,�������_cnnPop��_pop��Ϻ����Ӧֵ
float MyCNN::getFitValue(CNNIndividual& _cnnPop, Individual& _pop)
{
    //int t_start = clock();
    uint error = 0,errorBiggerThan10=0;		//��¼��ǩʶ��������
    uint i_input;
    for(i_input=0; i_input<cnnTrain.size(); ++i_input) {
        updatePerLayer(cnnTrain[i_input].data,_cnnPop);
        getCNNOut(matTrain[i_input].data);
        int tag = cnnTrain[i_input].tag;	//�õ��ø���ı�ǩ
        int getTag = getPEOut(_pop, matTrain[i_input].data);
        if(tag!=getTag) {
            if(getTag>9)
                ++errorBiggerThan10;
            else
                ++error;
        }
    }
    _cnnPop.fitValue = errorBiggerThan10*2.0f+error;
    if(_cnnPop.fitValue>cnnTrain.size())
        _cnnPop.fitValue = (float)cnnTrain.size();
    //cout<<clock()-t_start<<endl;
    //cin.get();
    return _cnnPop.fitValue;
}
