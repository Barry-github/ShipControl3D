/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//Wave.h
//�ο� matlab �к��� Wave_init.m

#ifndef WAVE_H_
#define WAVE_H_

#include "DataStruct.h"

class Wave
{
public:
	Wave();
	Wave(double dir, double Hs);

	~Wave();

	//��ʼ������
	void init();

	//����Data
	void setData(Data *dt);

	//�����˸ߺ�����
	void setPara(double Hs, double ang);

	//����
	void calWave();

	//����׳�
	double fact(int n);

	//������(ITTC˫������)
	double waveSpec(double Hs, double T0, double omega);

	//����w����
	void setWVec(double w[], int size);

	//��ȡ���˷�ֵ
	double *getZeta();
	//��ȡ���˷���
	double *getPsi();
	//��ȡ����Ƶ��
	double *getOmega();
	//��ȡ������
	double *getWaveNum();
	//��ȡ���������λ
	double *getPhase();
	//��ȡ���������ά��
	int getDim();

	//��ȡ����ֵ���������б��е�ֵ(2*pi��Ϊ36��)
	void headValue(double head, double &value1, double &value2);

	//��ȡPsiֵ������(��0��ʼ����36������)
	int getIndexPsi(double ps);
	
	//��ȡwֵ������(��0��ʼ����36������)
	int getIndexW(double omega);

	//����λ����λ
	double *posPhase(const Eta &eta, double time);


	//�������������һ�׶��ײ�����
	void calLoadForHead(double WF[], double DF[], const Eta &eta, double head, double time);

	//����һ�׶��ײ�����
	void calLoad(double WaveF[], double WaveD[]);

	//���㲨����
	void cal(const Eta &eta, double time_);

	//��ȡһ�׶��ײ�����
	void getLoad(Force6 &waveF, Force6 &waveD);
	
private:
	double fCut;	//�ض�Ƶ�� Frequencies cutoff factor
	double dirCut;	//�ضϷ��� Direction cutoff factor
	double nFreq;	//Ƶ����   Number of frequencies in grid
	double nDir;	//������   Number of directions in grid
	double psiMean;	//ƽ������(rad)	Mean wave direction
	double hs;		//���岨��(m)		Significant wave height Hs
	double omegaPeak;	//��ֵƵ��(rad/s) Peak frequency omega_0
	int	   spread;	//���˴�������	Wave spreading factor
	double engLim;	//wave coomponent energy limit
	double depth;	//ƽ��ˮ��(m)		Average water depth

	double omegaMax;	//
	double deltaOmega;	//Ƶ�ʼ��
	double deltaPsi;	//������
	double psiStart;	//��ʼ����
	double psiMax;		//�������
	double engMean;		//������ֵ
	double engComp;		//
	double K;			//ϵ��
	double Spsi;		//
	double *spec;		//������
	//double *omegaVec;	//��¼omega
	double *omVec;		//
	double **psiVec;	//��¼psi
	double Stot;		//Spectrum value;
	double nWaves;		//��������
	double *wVec;
	int    wSize;
	double wt;			//Ȩֵ

	int *idxPsi, *idxW;
	double *posPhs;

	double WaveF1[DOF6], WaveD1[DOF6], WaveF2[DOF6], WaveD2[DOF6],
		 WaveF[DOF6], WaveD[DOF6];

	Data *data;

	//������
	double *Zeta;		//
	double *Omega;
	double *Phase;
	double *WaveNum;		//������
	double *Psi;

	int kId;
};

#endif//WAVE_H_