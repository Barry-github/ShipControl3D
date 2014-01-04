/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//NMPController.h

#ifndef NMPCONTROLLER_H_
#define NMPCONTROLLER_H_

#include "DataStruct.h"
#include <fstream>

#define MAXDIM 12
#define MAXSURGE	1600000
#define MAXSWAY		1600000
#define MAXYAW		60000000

class NMPCcontroller
{
public:
	NMPCcontroller(void);
	~NMPCcontroller(void);

	//������ʼ��
	void init();

	//����M����
	void calM();

	//����������
	void cal();

	//����Ԥ������
	void setT(const double period);

	//����Ȩֵ
	void setWeight(const double lmd1, const double lmd2, const double lmd3);

	//�ٶ�����
	void setNu(const Nu nu);

	//λ������
	void setEta(const Eta eta);

	//����Ŀ��λ����̬
	void setTarget(const Eta eta);
	
	//������������
	void setEnv(const Force3 forcEnv);

	//���������
	Force6 Force();

	//��˹��Ԫ����������
	bool inv(double (*A)[DOF3], double (*B)[DOF3]);

private:
	double M[MAXDIM][MAXDIM], A[DOF3][MAXDIM], B[DOF3][DOF3],
		D[DOF3][MAXDIM], F[MAXDIM], U[DOF3];
	double C[DOF3][DOF3], invC[DOF3][DOF3];

	double M3[DOF3][DOF3];

	double q0[MAXDIM], q1[MAXDIM][DOF3], q1T[DOF3][MAXDIM];

	//Ŀ�����ֵ
	double yd[MAXDIM];
	
	double m[DOF3][DOF3], d[DOF3][DOF3];
	double a[DOF3][DOF3], b[DOF3];

	//λ�����ٶȱ���
	double x, y, psi, u, v, r;

	//Ԥ��ʱ�䳤��T
	double T;

	//����ָ�꺯���е�Ȩֵ
	double lambda[DOF3];

	//�ƽ���Լ��
	double lmtForce[DOF3];

	//��������
	Force6 force;

	//��绷����
	Force3 env;

	ofstream outFile;
};

#endif//NMPCONTROLLER_H_