/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//OptController.cpp

#include "OptController.h"
#include "Tool.h"

OptController::OptController(void) :
P(0.02), I(0.0), D(0.0)
{
	init();
}


OptController::~OptController(void)
{
}

//��ʼ������
void OptController::init()
{
	sum = 0.0;

	storOptPsi.clear();
	storXForce.clear();
	storYForce.clear();

	//��ʼ�����㻷����������ʱʹ�õ�����ת��
	taoX = 0.0;
	taoY = 0.0;
	taoN = 0.0;
	tagTaoY = 0.0;
	//��ʼ��ԭʼ������ת��
	xForce = 0.0;
	yForce = 0.0;
	nMoment = 0.0;

	//���ȵĳ�ʼ��
	errY = 0.0;
	intErrY = 0.0;
	diffErrY = 0.0;
	preErrY = 0.0;

	optPsi = 0.0;
	preOptPsi = 0.0;
	signX = 1.0;
}

//����ʱ����
void OptController::setStep(const double Intval)
{
	step = Intval;
}

//���õ�ǰ������������
void OptController::setPsi(const double preTagHead)
{
	preOptPsi = preTagHead;
}

//���õ�ǰ�ƽ���������ת��
void OptController::setTao(const Force6 Thrust)
{
	xForce = Thrust.xForce;
	yForce = Thrust.yForce;
	nMoment = Thrust.nMoment;
}

//����PID����
void OptController::setPID(const double Pval, const double Ival, const double Dval)
{
	P = Pval;
	I = Ival;
	D = Dval;
}

//�������ֵ
double OptController::meanData(list<double> &valLst, const double inVal)
{
	sum = 0.0;
	valLst.push_back(inVal);
	if (COUNT < valLst.size())
	{
		valLst.pop_front();
	}

	for (int i = 0; i < valLst.size(); i ++)
	{
		sum += valLst.front();
		valLst.push_back(valLst.front());
		valLst.pop_front();
	}
	return sum/static_cast<double>(valLst.size());	 
}

//PID����
void OptController::cal()
{
	//�ֱ������һ��ʱ��������������ƽ����
	taoX = meanData(storXForce, xForce);
	taoY = meanData(storYForce, yForce);
	
	if (LMTX < taoX)
	{
		signX = 1.0;
	} else if (-LMTX > taoX)
	{
		signX = -1.0;
	}
	
	if (fabs(taoY) > LMTY)
	{
		errY = taoY - tagTaoY;
		intErrY += errY*step;
		diffErrY = (errY - preErrY)/step;
		preErrY = errY;

		optPsi = preOptPsi + signX*(P*errY + I*intErrY + D*diffErrY);
		optPsi = meanData(storOptPsi, optPsi);
		//optPsi = Tool::infToPi(optPsi);
	}
}

//��ȡ������������
double OptController::OptPsi()
{
	return optPsi;
}