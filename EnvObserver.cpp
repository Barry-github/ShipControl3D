/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-25							**
******************************************************************/

//EnvObserver.cpp

#include "EnvObserver.h"

EnvObserver::EnvObserver(void)
{
	//��ʼ��
	init();
}

EnvObserver::~EnvObserver(void)
{
}

//��ʼ��
void EnvObserver::init()
{
	tStep = 0.0;
	for (int i = 0; i < DOF3; i ++)
	{
		beta[i] = 0.0;
		tao[i] = 0.0;
		envTao[i] = 0.0;
		nu[i] = 0.0;
		D_nu[i] = 0.0;
		M_nu[i] = 0.0;
		K_M_nu[i] = 0.0;

		for (int j = 0; j < DOF3; j ++)
		{
			K0[i][j] = 0.0;
			M[i][j] = 0.0;
			D[i][j] = 0.0;
		}
	}
	//ϵͳ���Ծ���(ԭ6���ɶ�ϵͳ���Ծ���򻯶���)
	M[0][0] = 0.0026e+10;
	M[1][1] = 0.0033e+10;
	M[2][2] = 6.5209e+10;

	//�������(ԭ6���ɶ��������򻯶���)
	D[0][0] = 0.0002e+8;
	D[1][1] = 0.0022e+8;
	D[2][2] = 7.1506e+8;
	
}


//����ʱ����
void EnvObserver::setStep(const double deltaT)
{
	tStep = deltaT;
}

//���ò���
void EnvObserver::setK(const double k1, const double k2, const double k3)
{
	K0[0][0] = k1;
	K0[1][1] = k2;
	K0[2][2] = k3;
}

//����M����(3ά)
void EnvObserver::setM(const double (*Mmat)[DOF3])
{
	for (int i = 0; i < DOF3; i ++)
	{
		for ( int j = 0; j < DOF3; j ++)
		{
			M[i][j] = Mmat[i][j];
		}
	}
}

//����D����(3ά)
void EnvObserver::setD(const double (*Dmat)[DOF3])
{
	for (int i = 0; i < DOF3; i ++)
	{
		for ( int j = 0; j < DOF3; j ++)
		{
			D[i][j] = Dmat[i][j];
		}
	}
}

//�����ƽ�������������
void EnvObserver::setTao(const Force6 thrust)
{
	tao[0] = thrust.xForce;
	tao[1] = thrust.yForce;
	tao[2] = thrust.nMoment;
}

//���봬���˶��ٶ�
void EnvObserver::setNu(const Nu nuIn)
{
	nu[0] = nuIn.u;
	nu[1] = nuIn.v;
	nu[2] = nuIn.r;
}

//���������(3���ɶ�)
Force3 EnvObserver::force()
{
	env.xForce = envTao[0];
	env.yForce = envTao[1];
	env.nMoment = envTao[2];
	return env;
}

//����˷���6x6������6x1�������
void EnvObserver::multiMx(const double (*dataMx1)[DOF3], const double dataMx2[], double resultMx[])
{
	for (int i = 0; i < DOF3; ++ i)
	{
		resultMx[i] = 0;
		for (int j = 0; j < DOF3; ++ j)
		{
			resultMx[i] += dataMx1[i][j]*dataMx2[j];
		}
	}
}

//��������
void EnvObserver::cal()
{
	//�����м��������ά����
	multiMx(D, nu, D_nu);
	multiMx(M, nu, M_nu);
	multiMx(K0, M_nu, K_M_nu);

	//�����м����
	for (int i = 0; i < DOF3; i ++)
	{
		betaTmp[i] = -(beta[i] - D_nu[i] + tao[i] + K_M_nu[i]);
	}

	//����µĵ���
	multiMx(K0, betaTmp, dBeta);

	//������뻷��������
	for (int i = 0; i < DOF3; i ++)
	{
		beta[i] += dBeta[i]*tStep;
		envTao[i] = beta[i] + K_M_nu[i];
	}
}
