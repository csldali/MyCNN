#include"CNN_Base.hpp"

const uint  CNN_Base::CS_NUM[6] = {5,5,10,10,20,11};
const float CNN_Base::Min_kernel = -1, CNN_Base::Max_kernel = 1;
const float CNN_Base::Min_bias = -1.5,  CNN_Base::Max_bias = 1.5;

CNN_Base::CNN_Base(uint _inputSize,uint _popSize)
{
	inputSize = _inputSize;
    cnnPop.resize(_popSize);
    constructCS();
}
CNN_Base::~CNN_Base() {}
//����CS����Ĳ�������,����ʼ������˺�ƫ�ò���
void CNN_Base::constructCS()
{
    uint i,j,k,index;
    setSrand();	//�����������
    for(index=0; index<cnnPop.size(); ++index) {
        //��ʼ�������. ��Χ�Ǿ���ֵ,�����ں��ڵ���
        for(i=0; i<kernelNum; ++i) {
            cnnPop[index].c_kernel[i].resize(kernelSize);
            for(j=0; j<kernelSize; ++j) {
                cnnPop[index].c_kernel[i][j].resize(kernelSize);
                for(k=0; k<kernelSize; ++k)
                    cnnPop[index].c_kernel[i][j][k] = randFloat(Min_kernel,Max_kernel);
            }
        }
        //��ʼ��ƫ�ò���.ÿ��Map��Ӧһ��ƫ��. ��Χ�Ǿ���ֵ,�����ں��ڵ���
        cnnPop[index].bias.resize(61);
        for(i=0; i<cnnPop[index].bias.size(); ++i)
            cnnPop[index].bias[i] = randFloat(Min_bias,Max_bias);
        //��ʼ������
        cnnPop[index].stepInGuass[0] = randFloat(Min_kernel,Max_kernel);
        cnnPop[index].stepInGuass[1] = randFloat(Min_bias,Max_bias);
    }
    try {
        //��ʼ��C1
        int C1Size = inputSize-kernelSize+1;
        C1.resize(CS_NUM[0],vectorF2D(C1Size,vectorF(C1Size)));
        //��ʼ��S2
        int S2Size = (C1Size+subSampleSize-1)/subSampleSize;
        S2.resize(CS_NUM[1],vectorF2D(S2Size,vectorF(S2Size)));
        //��ʼ��C3
        int C3Size = S2Size-kernelSize+1;
        C3.resize(CS_NUM[2],vectorF2D(C3Size,vectorF(C3Size)));
        //��ʼ��S4
        int S4Size = (C3Size+subSampleSize-1)/subSampleSize;
        S4.resize(CS_NUM[3],vectorF2D(S4Size,vectorF(S4Size)));
        //��ʼ��C5
        int C5Size = 1;//��C5��Сֱ������Ϊ1\\S4Size-kernelSize+1;
        C5.resize(CS_NUM[4],vectorF2D(C5Size,vectorF(C5Size)));
        //��ʼ��S6
        int S6Size = (C5Size+subSampleSize-1)/subSampleSize;
        S6.resize(CS_NUM[5],vectorF2D(S6Size,vectorF(S6Size)));
    } catch(...) {
        throw "����ͼ��СinputSize���ò���ȷ���޷���������������磬����!";
    }
}
//�����µ�����ͼ
void CNN_Base::addTrain(const vectorF2D& grayData, int tag)
{
    if(grayData.size()<inputSize || grayData[0].size()<inputSize) {
        cout<< "wrong size of input grayData,input grayData must be "<<inputSize<<"*"<<inputSize<<endl;
        return;
    }
    S_CNNInput input = {grayData,tag};
    cnnTrain.push_back(input);
}
//�����������ݣ���loadTrain��loadTest����
int CNN_Base::loadInput(const char* file,int size,vector<S_CNNInput> &dataVec,bool haveTag)
{
    ifstream loadFile(file);
    if(loadFile.is_open()) {
        if(size>0)
			dataVec.reserve(size);
        uint i,j,k;
        bool success = true;	//���ĳ�����ݶ�ȡ������ʧ��
        S_CNNInput input = {vectorF2D(inputSize,vectorF(inputSize)),0};
        for(i = 0; size < 1 || i < size; ++i) {
            for(j = 0; j < inputSize && success; ++j)
                for(k = 0; k < inputSize && success; ++k)
                    if(!(loadFile>>input.data[j][k])) {
						success = false;
						break;
					}
            //��������û�ж�ȡ�������򲻼���ѵ����
            if(!success || haveTag && !(loadFile>>input.tag))
                break;
			dataVec.push_back(input);
        }
        loadFile.close();
        return i;
    }else
		return 0;
}
//���ļ�����ѵ������
void CNN_Base::loadTrain(const char* file,int size)
{
    if(file==0 || file[0]==0 || size==0){
		cout<<"\r����ȷ����ѵ�����ļ�·���͸���"<<endl;
        throw "����ȷ����ѵ�����ļ�·���͸���";
    }
	int i = loadInput(file,size,cnnTrain,1);
	if(i>0)
        cout<<"\r\t\t\t\t\t\r>>�ɹ�����"<<i<<"��ѵ����"<<endl;
	else
		cout<<"\r\t\t\t\t\t\r�ļ���"<<file<<"����ʧ�ܣ�"<<endl;
}
//���ļ������������
void CNN_Base::loadTest(const char* file,int size,bool haveTag)
{
    if(file==0 || file[0]==0 || size==0){
		cout<<"\r����ȷ���ò��Լ��ļ�·���͸���"<<endl;
        throw "����ȷ���ò��Լ��ļ�·���͸���";
    }
	int i = loadInput(file,size,cnnTest,haveTag);
	if(i>0)
        cout<<"\r\t\t\t\t\t\r>>�ɹ�����"<<i<<"����Լ�"<<endl;
	else
		cout<<"\r\t\t\t\t\t\r�ļ���"<<file<<"����ʧ�ܣ�"<<endl;
}
//��inputData��Ϊ�������C1~S6ÿ��feature Map��ֵ
void CNN_Base::updatePerLayer(const vectorF2D& inputData, const CNNIndividual& indiv)
{
    uint i;
    //����C1��,C1����CS_NUM[0](=5)������ͼMap,ȡǰ5��(��0~4��)����˶�����ͼinputData���о��
    for(i=0; i<C1.size(); ++i)//��i�ž����������ͼ����ټ���ƫ�õõ�C1[i]
        convoluteMap(C1[i],inputData,indiv.c_kernel[i],indiv.bias[i]);
    //����S2��,S2����CS_NUM[1](=5)��Map,��C1�����Ӧ����ͼ��subSampleSize*subSampleSize�������,�ټ�ƫ��
    for(i=0; i<S2.size(); ++i)
        subSampleMap(S2[i],C1[i],indiv.bias[CS_NUM[0]+i],SSM_Max);
    //����C3��,C3����CS_NUM[2](=10)������ͼMap,ȡ5~14�ž������S2����ӦMap���о��
    updateC3Layer(indiv.c_kernel,indiv.bias);
    //����S4��,S4����CS_NUM[3](=10)��Map,��C3�����Ӧ����ͼ��subSampleSize*subSampleSize�������,�ټ�ƫ��
    static const uint CS_NUM_SUM = CS_NUM[0]+CS_NUM[1]+CS_NUM[2];
    for(i=0; i<S4.size(); ++i)
        subSampleMap(S4[i],C3[i],indiv.bias[CS_NUM_SUM+i],SSM_Max);
    //����C5��,C5����CS_NUM[4](=20)������ͼMap,ȡ0~20�ž���˷ֱ���S4��ÿ��Map���о��,C5��ÿ��Mapֻ��һ��ֵ
    updateC5Layer(indiv.c_kernel,indiv.bias);
    //����S6��,S6����CS_NUM[5](=11)������ͼMap,S6��ÿ�����������ֵ��C5�����ȡ����Mapȡƫ����͵õ�
    //updateS6Layer(indiv.bias);
}
//�Կ�ѵ������(����˺�ƫ��)ִ�б���
void CNN_Base::kernelBiasMutate()
{

}
//����C3��,S2��C3��ӳ���е㸴��,�ڲ�ͬ������¿��ܻᱻ��д��C3����CS_NUM[2](=10)������ͼMap,
//ȡ5~14�ž������S2����ӦMap���о��������ļ���ֻ����S2��5��Map,C3��10��Map�����
void CNN_Base::updateC3Layer(const kernelType* c_kernel,const biasType& bias)
{
    uint i;
    static const uint biasStart = CS_NUM[0]+CS_NUM[1];
    vectorF2D tmp(C3[0].size(),vectorF(C3[0][0].size(),0)),tmp2[4];
    tmp2[0] = tmp2[1] = tmp2[2] = tmp2[3] = tmp;//tmp2[0~3]�ѳ�ʼ����С
    for(i=0; i<4; ++i) {
        convoluteMap(tmp2[0],S2[i]  ,c_kernel[5+i], bias[biasStart+i]);
        convoluteMap(C3[i]  ,S2[i+1],c_kernel[5+i], bias[biasStart+i]);
        addVector(C3[i],tmp2[0]);
		sigmoidMap(C3[i]);
    }
    for(i=0; i<3; ++i) {
        convoluteMap(tmp2[0],S2[i]  ,c_kernel[9+i],bias[biasStart+4+i]);
        convoluteMap(tmp2[1],S2[i+1],c_kernel[9+i],bias[biasStart+4+i]);
        convoluteMap(C3[4+i],S2[i+2],c_kernel[9+i],bias[biasStart+4+i]);
        addVector(C3[4+i],tmp2[0],tmp2[1]);
		sigmoidMap(C3[4+i]);
    }
    for(i=0; i<2; ++i) {
        convoluteMap(tmp2[0],S2[i]  ,c_kernel[12+i],bias[biasStart+7+i]);
        convoluteMap(tmp2[1],S2[i+1],c_kernel[12+i],bias[biasStart+7+i]);
        convoluteMap(tmp2[2],S2[i+2],c_kernel[12+i],bias[biasStart+7+i]);
        convoluteMap(C3[7+i],S2[i+3],c_kernel[12+i],bias[biasStart+7+i]);
        addVector(C3[7+i],tmp2[0],tmp2[1],tmp2[2]);
		sigmoidMap(C3[7+i]);
    }
    convoluteMap(tmp2[0],S2[0],c_kernel[14],bias[biasStart+9]);
    convoluteMap(tmp2[1],S2[1],c_kernel[14],bias[biasStart+9]);
    convoluteMap(tmp2[2],S2[2],c_kernel[14],bias[biasStart+9]);
    convoluteMap(tmp2[3],S2[3],c_kernel[14],bias[biasStart+9]);
    convoluteMap(C3[9]  ,S2[4],c_kernel[14],bias[biasStart+9]);
    addVector(C3[9],tmp2[0],tmp2[1],tmp2[2],tmp2[3]);
	sigmoidMap(C3[9]);
}
//ͨ��ȫ������ʽ����C3��,ÿһ��C3��ͼ��ÿһ��S2���ͼ����Ҫһ�������
void CNN_Base::updateC3AllConnect(const kernelType* c_kernel,const biasType& bias)
{
	uint i,j, s2_Size = S2.size(), c3_size = C3.size();
    static const uint biasStart = CS_NUM[0]+CS_NUM[1];
	static vectorF2D tmp(C3[0].size(),vectorF(C3[0][0].size()));
	for(i=0;i<c3_size;++i) {
		zeroVector(C3[i]);
		for(j=0;j<s2_Size;++j) {
			convoluteMap(tmp,S2[j],c_kernel[i+CS_NUM[2]*j+CS_NUM[0]], bias[biasStart+i]);
			addVector(C3[i],tmp);
		}
		sigmoidMap(C3[i]);
	}
}
//����C5��,C5����CS_NUM[4](=20)������ͼMap,ȡ15~34�ž���˷ֱ���S4��ÿ��Map���о��,C5��ÿ��Mapֻ��һ��ֵ
void CNN_Base::updateC5Layer(const kernelType* c_kernel,const biasType& bias)
{
    uint i,j;
    static const uint CS_NUM_SUM = CS_NUM[0]+CS_NUM[1]+CS_NUM[2]+CS_NUM[3];
    static const uint kernelStart = CS_NUM[0] + CS_NUM[2];
    float c5Elem;	//������Ҫ��֤C5��Map����1*1��С,��һ��float��
    for(i=0; i<C5.size(); ++i) {
        c5Elem = 0;
        for(j=0; j<S4.size(); ++j) { //�ö�Ӧ�������S4��ÿ��ͼ����ټ���ƫ�õõ�C5[i]
            convoluteMap(C5[i],S4[j],c_kernel[i+kernelStart],bias[CS_NUM_SUM+i]);
            c5Elem += C5[i][0][0];
        }
        C5[i][0][0] = c5Elem;
    }
}
//����S6��,S6����CS_NUM[5](=11)������ͼMap,S6��ÿ�����������ֵ��C5�����ȡ����Mapȡƫ����͵õ�
void CNN_Base::updateS6Layer(const biasType& bias)
{
    static const uint CS_NUM_SUM = CS_NUM[0]+CS_NUM[1]+CS_NUM[2]+CS_NUM[3]+CS_NUM[4];
    for(uint i=0; i<S6.size(); ++i)	//������Ҫ��֤S6��Map����1*1��С
        S6[i][0][0] = 2 * bias[CS_NUM_SUM+i] + C5[i][0][0] + C5[(i+S6.size())%C5.size()][0][0];
}
//��һ������ͼinput������kernelִ�о�������ټ�ƫ��bias��,�õ���Ӧ������ӳ��ͼoutput
void CNN_Base::convoluteMap(vectorF2D& output,const vectorF2D& input,const kernelType& kernel,float _bias)
{
    uint j,k;
    for(j=0; j<output.size(); ++j)
        for(k=0; k<output[j].size(); ++k)
            output[j][k] = getConvoluteOut(input,kernel,j,k) + _bias;//�����Ϊ(j,k)�ĵ㿪ʼ�����
}
//��һ������ͼinputͨ��mode��ʽ�²������ټ�ƫ��bias,�õ���Ӧ��������ȡͼoutput
void CNN_Base::subSampleMap(vectorF2D& output,const vectorF2D& input,float _bias,int mode)
{
    uint j,k;
    for(j=0; j<output.size(); ++j)
        for(k=0; k<output[j].size(); ++k)
            output[j][k] = sigmoid(getSubSampleOut(input,j*subSampleSize,k*subSampleSize,mode) + _bias);
}
//��out��vec1��ӱ��浽out
void CNN_Base::addVector(vectorF2D& output,const vectorF2D& vec1)
{
    uint i,j;
    for(i=0; i<output.size() && i<vec1.size(); ++i)
        for(j=0; j<output[i].size() && j<vec1[i].size(); ++j)
            output[i][j] += vec1[i][j];
}
//�����������ͼ��ӡ���ļ���
void CNN_Base::printCSToFile(const char* file)
{
    ofstream outFile(file);
    if(outFile.is_open()) {
        uint i,j,k;
        for(i=0; i<C1.size(); ++i) {
            outFile<<"C1-"<<i<<endl;
            for(j=0; j<C1[0].size(); ++j) {
                for(k=0; k<C1[0][0].size(); ++k)
                    outFile<<setw(4)<<C1[i][j][k]<<" ";//<<setw(5)
                outFile<<endl;
            }
            outFile<<"��������������������������������������������������������"<<endl;
        }
        for(i=0; i<S2.size(); ++i) {
            outFile<<"S2-"<<i<<endl;
            for(j=0; j<S2[0].size(); ++j) {
                for(k=0; k<S2[0][0].size(); ++k)
                    outFile<<setw(4)<<S2[i][j][k]<<" ";//<<setw(5)
                outFile<<endl;
            }
            outFile<<"��������������������������������������������������������"<<endl;
        }
        for(i=0; i<C3.size(); ++i) {
            outFile<<"C3-"<<i<<endl;
            for(j=0; j<C3[0].size(); ++j) {
                for(k=0; k<C3[0][0].size(); ++k)
                    outFile<<setw(4)<<C3[i][j][k]<<" ";//<<setw(5)
                outFile<<endl;
            }
            outFile<<"��������������������������������������������������������"<<endl;
        }
        for(i=0; i<S4.size(); ++i) {
            outFile<<"S4-"<<i<<endl;
            for(j=0; j<S4[0].size(); ++j) {
                for(k=0; k<S4[0][0].size(); ++k)
                    outFile<<setw(4)<<S4[i][j][k]<<" ";//<<setw(5)
                outFile<<endl;
            }
            outFile<<"��������������������������������������������������������"<<endl;
        }
        for(i=0; i<C5.size(); ++i) {
            outFile<<"C5-"<<i<<endl;
            for(j=0; j<C5[0].size(); ++j) {
                for(k=0; k<C5[0][0].size(); ++k)
                    outFile<<setw(4)<<C5[i][j][k]<<" ";//<<setw(5)
                outFile<<endl;
            }
            outFile<<"��������������������������������������������������������"<<endl;
        }
        for(i=0; i<S6.size(); ++i) {
            outFile<<"S6-"<<i<<endl;
            for(j=0; j<S6[0].size(); ++j) {
                for(k=0; k<S6[0][0].size(); ++k)
                    outFile<<setw(4)<<S6[i][j][k]<<" ";//<<setw(5)
                outFile<<endl;
            }
            outFile<<"��������������������������������������������������������"<<endl;
        }
        outFile<<"����������������������������������������������������������������������������������������������������������������"<<endl;
        for(uint index=0; index<cnnPop.size(); ++index) {
            for(i=0; i<kernelNum; ++i) {
                outFile<<"cnnPop-"<<index<<"  kernel-"<<i<<endl;
                for(j=0; j<cnnPop[index].c_kernel[i].size(); ++j) {
                    for(k=0; k<cnnPop[index].c_kernel[i][j].size(); ++k)
                        outFile<<cnnPop[index].c_kernel[i][j][k]<<"  ";
                    outFile<<endl;
                }
                outFile<<"��������������������������������������������������������"<<endl;
            }
            outFile<<"cnnPop["<<index<<"].stepInGuass[0] = "<<cnnPop[index].stepInGuass[0]<<endl;
        }
        outFile<<"����������������������������������������������������������������������������������������������������������������"<<endl;
        for(uint index=0; index<cnnPop.size(); ++index) {
            outFile<<"bias-"<<index<<endl;
            for(i=0; i<cnnPop[index].bias.size(); ++i)
                outFile<<cnnPop[index].bias[i]<<"  ";
            outFile<<endl;
            outFile<<"cnnPop["<<index<<"].stepInGuass[1] = "<<cnnPop[index].stepInGuass[1]<<endl;
        }
        outFile<<"����������������������������������������������������������������������������������������������������������������"<<endl;
        for(i=0; i<cnnTrain.size(); ++i) {
            outFile<<"cnnTrain-"<<i<<endl;
            for(j=0; j<cnnTrain[i].data.size(); ++j) {
                for(k=0; k<cnnTrain[i].data[j].size(); ++k)
                    outFile<<setw(3)<<cnnTrain[i].data[j][k]<<" ";
                outFile<<endl;
            }
            outFile<<"   "<<cnnTrain[i].tag<<endl;
            outFile<<"��������������������������������������������������������"<<endl;
        }
        outFile.close();
        cout<<"��������ļ�"<<file<<"��"<<endl;
    }
}
