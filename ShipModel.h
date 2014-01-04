/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//ShipControl.h

#ifndef SHIPMODEL_H_
#define SHIPMODEL_H_

#include "DataStruct.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <list>
using namespace std;

class ShipModel
{
public:
	ShipModel(void);
	~ShipModel(void);

	//��ʼ��
	void init();

	//��ʼ����������
	void setData(Data *data_);

	//����ʱ����
	void setStep(const double step);

	//������
	void setForce(Force6 force);

	//���봬���ĳ�ʼλ����̬
	void setInitEta(Eta etaIn);

	//���λ����̬
	Eta getEta();

	//����ٶȽ��ٶ�
	Nu getNu();

	//����M����
	void calM();

	//Cross-flow drag and surge resistance
	Force6 crosFlowDrag(Nu nu);

	//������Y��N Dragʱ���ú���
	double func(const Nu &nu, const double &x, bool flag=true);

	//����,�ۼ�
	double integrt(Nu &nu, double max, double min, double delta, bool flag=true);
	//Hoerner����
	double Hoerner(double BValue,double TValue);
	//����������
	void swap(double &a, double &b);
	
	//��״̬�������
	double solStateSpaceFunc(const double (*Ar)[visDampNum], const double Br[], const double Cr[], const double Dr, const double var, double xTemp[]);

	//����ճ���������
	Force6 viscousDamp(const Nu &nu);

	//roll damping
	double rollDamp(const double &varIn);
	
	//������������
	void cal();

	//�����ɶȱ�������ϵ�£����ٶȡ����ٶ���λ�á���̬ת��
	//eta0:����ı������ꣻnu������Ĵ������ꣻ
	//intev��ʱ���������Ϊת����ı�������
	void nuToEta(Eta &eta0, Nu nu, double intev);

private:
	//��������
	Data *data;

	double M[DOF6][DOF6], datMinv[DOF6][DOF6];
	double MinvTao[DOF6];

	//��ת����
	double RotMx[DOF3][DOF3], TransMx[DOF3][DOF3];

	//ʱ����
	double tStep;

	//�������ЧӦ
	Force6 mu;
	//��ק��
	Force6 drag;

	double muArray[DOF6], dragArray[DOF6], sprStifArray[DOF6], dampArray[DOF6];

	//���������������ƽ�������
	Force6 taoForce;
	double taoArray[DOF6];

	//�������ĺ���
	Force6 totalForce;
	double totForceArray[DOF6];

	//����λ����̬
	Eta eta;
	double etaArray[DOF6];

	//�����ٶȽ��ٶ�
	Nu nu;
	double nuArray[DOF6];
};

#endif//SHIPMODEL_H_