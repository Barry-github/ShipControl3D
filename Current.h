/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//Current.h

#ifndef CURRENT_H_
#define CURRENT_H_

#include "DataStruct.h"

#define NUM		37

class Current
{
public:
	Current(void);
	~Current(void);

	//��ʼ��
	void init();

	//�������ٺ������(��)
	void setPara(const double speed, const double dir);

	//���봬������(����)
	void setPsi(const double psiIn);
	
	//��������
	void cal();

	//�������
	Force6 force();

	//��ֵ����������ϵ��
	//���룺����ϵ�������������������
	double interp(const double *cc, const double num, const double bt);

private:
	//����ϵ������
	double cxc[NUM], cyc[NUM], cnc[NUM];
	//����ϵ��
	double cxcValue, cycValue, cncValue;
	//���߼䳤
	double Lpp;
	//ƽ����ˮ
	double Tm;
	//���������µ����١�����(����)
	double Vc, dirC;
	//���������(����)
	double beta;
	//��������
	double psi;
	//����
	Force6 forceCur;
	//��ˮ�ܶ�
	double rho_w;
};

#endif//CURRENT_H_