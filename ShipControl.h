/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-31							**
******************************************************************/

//ShipControl.h

#ifndef SHIPCONTROL_H
#define SHIPCONTROL_H

#include "DataStruct.h"
#include "ShipModel.h"
#include "PIDController.h"
#include "NMPCcontroller.h"
#include "OptController.h"
#include "EnvObserver.h"
#include "Filter.h"
#include "WOPC.h"
#include "Wind.h"
#include "Wave.h"
#include "Current.h"
#include <fstream>

class ShipControl
{
public:
	ShipControl(void);
	~ShipControl(void);

	//��ʼ��
	void init();

	//���ô�������
	void setData(Data *data_);
	
	//���ļ�
	void openFiles();

	//�ر��ļ�
	void closeFiles();

	//�û��ӿ�
	void userInterFace();

	//���ò���
	void setParameter();

	//������������
	void run();
		
	//����<<������
	friend ostream& operator << (ostream &os, const Eta &eta);

private:
	Data *data;

	ShipModel model;
	
	double tStep, time;	//�������������̲���
	ofstream etaFile, outFltFile, nuFile;
	ofstream centerFile, optHeadFile, targetFile;
	ofstream wave1File, wave2File, windFile, curFile;
	ofstream taoFile, thrustFile;
	ofstream paraFile;

	//����λ����̬��Ŀ��λ����̬�� �˲����λ����̬
	Eta eta, etaTarget, etaFlt;
	
	//�����ٶȽ��ٶ�
	Nu nu;

	//�����
	Wind wind;

	//���˸���
	Wave wave;

	//��������
	Current cur;

	//�ƽ�������
	Force6 thrust;

	//���������
	Force6 windForce;

	//������
	Force6 wave1Force, wave2Force;

	//����������
	Force6 curForce;

	double thrustArray[DOF6], wave1Array[DOF6], wave2Array[DOF6],
		windArray[DOF6], curArray[DOF6];

	//�ƽ����뻷���ĺ���
	Force6 tao;
	double taoArray[DOF6];

	//PID������
	PIDController pid;

	//NMPC������
	NMPCcontroller nmpc;

	//�����������������
	OptController optPsiCtrl;

	//WOPC
	WOPC wopc;

	//�����۲���
	EnvObserver envObs;

	//�����˲���
	Filter filter;

	//�����������Ƶ�ʼ�����
	int ctrlCount;
	//��������������(��0.05s��ctrlCyc��)
	int ctrlCyc;
	
	//����������
	Force3 envEst;

	//��������
	double optPsi;

	//������ʱ��
	double maxTime;

	//�趨���٣�����(��)
	double SpeedWind, DirWind;

	//�趨�˸ߣ�����(��)
	double HeightWave, DirWave;

	//�趨���٣�����(��)
	double SpeedCurrent, DirCurrent;

	//��ʼλ��������
	double xOrigin, yOrigin, psiOrigin;

	//Ŀ��λ��������
	double xTarget, yTarget, psiTarget;

	//�������Ŷ�����λ�뾶
	double radius;

	//�������ȵĸ��ֲ���
	//PID����
	double kp, ki, kd;
	//NMPC��Ԥ������
	double Tpre;
	//NMPC��Ȩֵ
	double w1, w2, w3;
	//ZPC-W����������
	double kpZ, kiZ, kdZ;
	//�����۲�������������
	double k1Env, k2Env, k3Env;

	//������λ�������ͣ�1.���涯����λ��2.�������Ŷ�����λ
	int dpFlag;
	//������λ���Ʒ�����1.PID���ƣ�2.������ģ��Ԥ�����
	int ctlFlag;
	//�������Ŷ�����λ���Ʋ������ͣ�
	//1.WOPC��ZPC-W��Ϻ�Ļ������Ŷ�����λ���Ʋ���
	//2.WOPC���û������ƵĻ������Ŷ�����λ���Ʋ���
	//3.ZPC-W�������Ŷ�����λ���Ʋ���
	int wopcFlag;

	//�û����ù�����ʹ�õ���ʱ����
	char xOriginStr[20], yOriginStr[20], PsiOriginStr[20];
	char *xOriginStrEnd, *yOriginStrEnd, *PsiOriginStrEnd;

	char xTargStr[20], yTargStr[20], PsiTargStr[20];
	char *xTargStrEnd, *yTargStrEnd, *PsiTargStrEnd;

	char SpeedWindStr[20], DirWindStr[20], HeightWaveStr[20], DirWaveStr[20], SpeedCurrentStr[20], DirCurrentStr[20];
	char *SpeedWindStrEnd, *DirWindStrEnd, *HeightWaveStrEnd, *DirWaveStrEnd, *SpeedCurrentStrEnd, *DirCurrentStrEnd;

	char dpFlagStr[20], ctlFlagStr[20], wopcFlagStr[20];

	char kpStr[20], kiStr[20], kdStr[20];
	char *kpStrEnd, *kiStrEnd, *kdStrEnd;

	char kpZStr[20], kiZStr[20], kdZStr[20];
	char *kpZStrEnd, *kiZStrEnd, *kdZStrEnd;

	char tStr[20], w1Str[20], w2Str[20], w3Str[20];
	char *tStrEnd, *w1StrEnd, *w2StrEnd, *w3StrEnd;

	double valueStr;
};


#endif//SHIPCONTROL_H