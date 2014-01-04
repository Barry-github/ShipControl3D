/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-25							**
******************************************************************/

//EnvObserver.h

#ifndef ENVOBSERVER_H_
#define ENVOBSERVER_H_

#include "DataStruct.h"

class EnvObserver
{
public:
	EnvObserver(void);
	~EnvObserver(void);

	//��ʼ��
	void init();

	//����ʱ����
	void setStep(const double deltaT);

	//���ò���
	void setK(const double k1, const double k2, const double k3);

	//����M����(3ά)
	void setM(const double (*Mmat)[DOF3]);

	//����D����(3ά)
	void setD(const double (*Dmat)[DOF3]);

	//�����ƽ�������������
	void setTao(const Force6 thrust);

	//���봬���˶��ٶ�
	void setNu(const Nu nuIn);

	//���������(3���ɶ�)
	Force3 force();

	//����˷���6x6������6x1�������
	void multiMx(const double (*dataMx1)[DOF3], const double dataMx2[], double resultMx[]);

	//��������
	void cal();

private:
	//M��D����
	double M[DOF3][DOF3], D[DOF3][DOF3];
	//����K0
	double K0[DOF3][DOF3];
	//�м�����¡��䵼�����м����
	double beta[DOF3], dBeta[DOF3], betaTmp[DOF3];
	//����������������
	Force3 env;
	//�ƽ��������������뻷��������������
	double tao[DOF3], envTao[DOF3];
	//�ٶ���
	double nu[DOF3];
	//��������м����
	double D_nu[DOF3], M_nu[DOF3], K_M_nu[DOF3];
	//ʱ����
	double tStep;
};

#endif//ENVOBSERVER_H_
