/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//Wave.cpp

#include "DataStruct.h"
#include "Wave.h"
#include "Tool.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
using namespace std;

#define DEPTH_MAX 10000

Wave::Wave()
{
	psiMean = 120.0*angToRad;
	hs = 3.0;
	init();
}

Wave::Wave(double dir, double Hs)
	: psiMean(dir/180.0*PI), hs(Hs)
{
	init();
}

Wave::~Wave()
{
	delete [] spec;
	//delete [] omegaVec;
	delete [] omVec;
	delete [] wVec;
	//for (int i = 0; i < int(nFreq)+1; i ++)
	//{
	//	delete [] psiVec[i];
	//}

	delete [] Zeta;
	delete [] Omega;
	delete [] Phase;
	delete [] WaveNum;
	delete [] Psi;
	delete [] idxPsi;
	delete [] idxW;
	delete [] posPhs;
}

//Ϊ�򵥣�������ֱ�Ӹ�ֵΪ����
void Wave::init()
{
	omegaPeak = -1.0;
	spread = 2;
	depth = DEPTH_MAX;
	nFreq = 20.0;
	nDir = 10.0;
	engLim = 0.005;
	fCut = 3.0;
	dirCut = 0.0;
	kId = 0;

	//������ʼ����

	hs = hs<0.0 ? 0.0:hs;

	depth = depth<0.001 ? 0.001:depth;

	nFreq = nFreq<=0.0 ? 1.0:nFreq;

	nDir = nDir<=0.0 ? 1.0:nDir;

	fCut = fCut<2.0 ? 2.0:fCut;

	omegaPeak = omegaPeak<=0.0001 ? (2.0*PI/(4.883+2.68*pow(hs, 0.54))):omegaPeak;

	if (spread < 1)
	{
		spread = 1;
	} else if (spread > 5)
	{
		spread = 5;
	}

	if (dirCut < 0.0)
	{
		dirCut = 0.0;
	} else if (dirCut > 3.0/8.0*PI)
	{
		dirCut = 3.0/8.0*PI;
	}

	if (engLim < 0.0)
	{
		engLim = 0.0;
	} else if (engLim > 1.0 && engLim > nFreq*nDir)
	{
		engLim = nFreq*nDir;
	}

	spec = new double [int(nFreq)];
	//omegaVec = new double [int(nFreq*nDir)];
	omVec = new double [int(nFreq)];
	psiVec = new double *[int(nFreq)];

	Zeta = new double [int(nFreq*nDir)];
	Omega = new double [int(nFreq*nDir)];
	Phase = new double [int(nFreq*nDir)];
	WaveNum = new double [int(nFreq*nDir)];
	Psi = new double [int(nFreq*nDir)];

	data = new Data;

	//��ȡʱ�䣬������ͬ�������������
	srand((unsigned)time( NULL ));
}

//����Data
void Wave::setData(Data *dt)
{
	data = dt;
}

//�����˸ߺ�����
void Wave::setPara(double Hs, double ang)
{
	hs = Hs;
	psiMean = ang*angToRad;
}

void Wave::calWave()
{

	//Cutoff frequency
	omegaMax = fCut * omegaPeak;

	//Frequency step
	deltaOmega = omegaMax / nFreq;

	//Direction step
	deltaPsi = (PI - 2*dirCut) / nDir;

	//Start direction
	psiStart = psiMean - PI/2.0 + deltaPsi/2.0 + dirCut;

	//Max direction
	psiMax = psiMean + PI/2.0 - deltaPsi/2.0 - dirCut;

	if (fabs(hs) > 0.0001)
	{
		engMean = pow(4*hs, 2.0) / (nFreq*nDir);
	} else {
		engMean = exp(-5.0);
	}

	double rd = 0.0, rdPsi = 0.0, rdOmega = 0.0; 

	//���㲨����
	int index = 0, idx = 0, kO = 0;
	int r = 0;

	//frequency loop
	for (double om = deltaOmega; om < omegaMax; om += deltaOmega)
	{
		spec[index] = waveSpec(hs, 2.0*PI/omegaPeak, om);
		omVec[index] = om;
		idx = 0;
		psiVec[index] = new double [int(nDir)];

		//direction loop
		for (double psi = psiStart; psi < psiMax; psi += deltaPsi)
		{
			if (nDir > 1.0)
			{
				K = (pow(2.0, 2.0*spread-1)*fact(int(spread))*fact(int(spread)-1))/(PI*fact(int(2.0*spread-1)));
				Spsi = K*pow(cos(psi-psiMean), 2.0*spread);		//��Ҫȷ���˷���λ��
			} else {
				Spsi = 1.0/deltaPsi;
			}
			//��¼psi
			psiVec[index][idx] = Tool::infToPi(psi);
			idx ++;

			//Wave component energy
			engComp = spec[index]*Spsi*deltaOmega*deltaPsi;

			//Energy test
			if ((engComp/engMean>=engLim)||engLim>1.0)
			{
				Zeta[kId] = sqrt(2.0*spec[index]*Spsi*deltaOmega*deltaPsi);

				rd = double(rand())/(double(RAND_MAX));

				rdPsi = deltaPsi*(rd-0.5);

				rdOmega = deltaOmega*(rd-0.5);

				//Frequency
				Omega[kO] = om + rdOmega;

				kO += 1;

				//Wave number
				//if (DEPTH_MAX < depth)
				//{
				WaveNum[kId] = pow(Omega[kO], 2.0)/g;
				//cout << "wave num:\t" << kId << "\t" << WaveNum[kId] << endl;
				//} else {
				//��ʽΪmatlab���򣬼���waveNum
				//	//Wavenum(k) = fzero(@(w) w*tanh(w*depth) - Omega(k)^2/g,[wavenum_0,1E10]);
				//}

				//Direction
				if (1.0 < nDir)
				{
					Psi[kId] = psi + rdPsi;
				} else {
					Psi[kId] = psi;
				}

				//ƽ������ͬ��Psi����Matlab�е���ֵ��ͬ

				//Phase
				Phase[kId] = rd*2*PI;

				//Spectrum value
				Stot = spec[index]*Spsi;

				// Spectrum value (for plotting)
				//	S_plot(m,n) = s_omega*s_psi;

				kId ++;
			} else {
				//Spectrum value for plotting set to zero
				// S_plot(m,n) = 0;
			}

			r ++;
		} 
		index ++;
	}

	//���沨������
	nWaves = kId;

	// If using the set number of waves approach (energylim < 0, nwaves = - energylim)
	// matlab�������м��㣬�˴�δ��д

	idxPsi = new int[kId];
	idxW = new int[kId];
	posPhs = new double[kId];

	setWVec(data->dataVes.forceRAO.w, 36);
}

//����w����
void Wave::setWVec(double w[], int size)
{
	wSize = size;
	wVec = new double[wSize];
	for(int i = 0; i < wSize; i ++)
	{
		wVec[i] = w[i];
	}
}

//��ȡ���˷�ֵ
double *Wave::getZeta()
{
	return Zeta;
}

//��ȡ���˷���
double *Wave::getPsi()
{
	return Psi;
}

//��ȡ����Ƶ��
double *Wave::getOmega()
{
	return Omega;
}

//��ȡ������
double *Wave::getWaveNum()
{
	return WaveNum;
}

//��ȡ���˵������λ
double *Wave::getPhase()
{
	return Phase;
}

//��ȡ���������ά��
int Wave::getDim()
{
	return kId;
}

//��ȡ����ֵ���������б��е�ֵ(2*pi��Ϊ36��)
void Wave::headValue(double head, double &value1, double &value2)
{
	double index = 0;
	while (0 > head)
	{
		head += 2.0*PI;
	}
	while (2.0*PI <= head)
	{
		head -= 2.0*PI;
	}

	index = int(head/(PI*2.0/wSize));
	value1 = index*(PI*2.0/wSize);
	if (wSize-1 != index)
		value2 = value1 + (PI*2.0/wSize);
	else
		value2 = 0.0;

	wt = fabs((value2-head) / (value2-value1));
}

//����һ�׶��ײ�����
void Wave::calLoadForHead(double WF[], double DF[], const Eta &eta, double head, double time)
{
	int speed = 0;

	double taoWF[DOF6] = {0.0};
	double taoWD[DOF6] = {0.0};

	for (int k = 0; k < DOF6; k ++)
	{
		for (int i = 0; i < kId; i ++)
		{
			idxPsi[i] = getIndexPsi(Psi[i]-head);
			idxW[i] = getIndexW(Omega[i]);
			taoWF[k] += data->dataVes.forceRAO.amp[k][speed][idxW[i]][idxPsi[i]] * Zeta[i] *
				sin(posPhase(eta, time)[i]+data->dataVes.forceRAO.phase[k][speed][idxW[i]][idxPsi[i]]);
			taoWD[k] += data->dataVes.driftfrc.amp[k][speed][idxW[i]][idxPsi[i]]*pow(Zeta[i],2.0);
		}
	}
	taoWD[5] = taoWD[2];
	taoWD[2] = 0.0;
	taoWD[3] = 0.0;
	taoWD[4] = 0.0;
	
	for (int k = 0; k < DOF6; k ++)
	{
		WF[k] = taoWF[k];
		DF[k] = taoWD[k];
	}
}

//����һ�׶��ײ�����
void Wave::calLoad(double WaveF[], double WaveD[])
{
	for (int k = 0; k < DOF6; k ++)
	{
		WaveF[k] = WaveF1[k]*wt + WaveF2[k]*(1.0-wt);
		WaveD[k] = WaveD1[k]*wt + WaveD2[k]*(1.0-wt);
	}
}

//���㲨����
void Wave::cal(const Eta &eta, double time_)
{
	double head1, head2;
	headValue(eta.psi, head1, head2);
	calLoadForHead(WaveF1, WaveD1, eta, head1, time_);
	calLoadForHead(WaveF2, WaveD2, eta, head2, time_);
	calLoad(WaveF, WaveD);
}

//��ȡPsiֵ������(��0��ʼ����36������)
int Wave::getIndexPsi(double ps)
{
	while (0 > ps)
	{
		ps += 2.0*PI;
	}
	while (2.0*PI <= ps)
	{
		ps -= 2.0*PI;
	}

	int index = int((ps+PI/wSize)/(PI*2.0/wSize));
	index = index<wSize ? index : 0;
	return index;
}

//��ȡһ�׶��ײ�����
void Wave::getLoad(Force6 &waveForce, Force6 &waveDrift)
{
	waveForce.xForce = WaveF[0];
	waveForce.yForce = WaveF[1];
	waveForce.zForce = WaveF[2];
	waveForce.kMoment = WaveF[3];
	waveForce.mMoment = WaveF[4];
	waveForce.nMoment = WaveF[5];

	waveDrift.xForce = WaveD[0];
	waveDrift.yForce = WaveD[1];
	waveDrift.zForce = WaveD[2];
	waveDrift.kMoment = WaveD[3];
	waveDrift.mMoment = WaveD[4];
	waveDrift.nMoment = WaveD[5];
}

//��ȡwֵ������(��0��ʼ����36������)
int Wave::getIndexW(double omega)
{
	if (omega < wVec[0])
		return 0;
	else if (omega > wVec[wSize-1])
		return wSize-1;
	else {
		for (int i = 0; i < wSize-1; i ++)
		{
			if (omega>wVec[i] && omega<wVec[i+1])
			{
				if (fabs(omega-wVec[i])<fabs(omega-wVec[i+1]))
					return i;
				else
					return i+1;
			}
		}
	}
}

//����λ����λ
double *Wave::posPhase(const Eta &eta, double time)
{
	for (int i = 0; i < kId; i ++)
	{
		posPhs[i] = Phase[i] + Omega[i]*time - (eta.n*cos(Psi[i]) + eta.e*sin(Psi[i]))*WaveNum[i];
	}
	return posPhs;
}

//������(ITTC˫������),��λm^2��s	�ο����������������Ͳ��ԡ�P108
double Wave::waveSpec(double Hs, double T0, double omega)
{
	double A, B, Sw;	//T1Ϊ��������

	//Pierson-Moskowitz 
	//A = 0.0081*pow(g, 2);
	//B = 3.11/pow(Hs, 2);
	//m0 = A/(4*B);
	//m1 = (1.0/3.0)*A/pow(B,0.75)*0.91906;
	//T1 = 2*PI*m0/m1;

	//Sw = 173*pow(Hs,2)/(pow(T1, 4)*pow(omega,5))*exp(-691/(pow(T1,4)*pow(omega, 4)));


	//ITTC-Modified Pierson-Moskowitz (Hs,T0)
	A = 487.0*pow(Hs,2.0)/pow(T0, 4.0);
	B = 1949/pow(T0, 4.0);

	Sw = A/(pow(omega,5)*exp(B/(pow(omega,4))));

	return Sw;
}

//����׳�
double Wave::fact(int n)
{
	double result = 0.0;
	if (0 >= n)
	{
		result = 0.0;
	} else if ( 1 == n)
	{
		result = 1.0;
	} else {
		result = fact(n-1)*double(n);
	}
	return result;
}
