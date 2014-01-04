/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//OptController.h

#ifndef OPTCONTROLLER_H_
#define OPTCONTROLLER_H_

#include "DataStruct.h"
#include <list>
using namespace std;

#define COUNT	20
#define LMTX	10000
#define LMTY	10000

class OptController
{
public:
	OptController(void);
	~OptController(void);

	//��ʼ������
	void init();

	//����ʱ����
	void setStep(const double Intval);

	//���õ�ǰ������������
	void setPsi(const double preTagHead);

	//���õ�ǰ�ƽ���������ת��
	void setTao(const Force6 Thrust);

	//����PID����
	void setPID(const double Pval, const double Ival, const double Dval);

	//�������ֵ
	double meanData(list<double> &valLst, const double inVal);

	//PID����
	void cal();

	//���������������
	double OptPsi();

private:
	//ʱ����
	double step;

	//�洢�����������������
	list<double> storOptPsi;
	//�洢�������ͺ��������б�
	list<double> storXForce, storYForce;	

	//���㻷����������ʱʹ�õ�����ת��
	double taoX, taoY, taoN, tagTaoY;
	//ԭʼ������ת��
	double xForce, yForce, nMoment;

	//������΢�֡����Ļ��ֺ�ǰһ�����
	double errY, diffErrY, intErrY, preErrY;

	//��������
	double optPsi, preOptPsi;

	//��������������ʹ�õ�PIDϵ��
	double P, I, D;

	//�����ֵʱʹ�õ���ʱ����
	double sum;

	//������������־
	double signX;
};

#endif//OPTCONTROLLER_H_