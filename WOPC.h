/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//WOPC.h

#ifndef WOPC_H_
#define WOPC_H_

#include "DataStruct.h"
#include <stdlib.h>

using namespace std;

class WOPC
{
public:
	WOPC(void);
	~WOPC(void);

	//��ʼ��
	void init();

	//��������Բ��
	void centerControl();

	//���㻷����������
	void optHeadCal();

	//���������������
	void wohc();

	//���Ƽ���
	void calculat();

	//����ʱ����
	void setStep(const double dtime);

	//��������Բ�İ뾶
	void setRadius(const double radius);

	//����Ŀ��λ��
	void setPos(const Eta etaTarget);

	//ʵʱ��ȡ��ǰλ��������
	void setEta(const Eta etaIn);

	//ʵʱ��ȡ��ǰ�ٶ�����ٶ�
	void setNu(const Nu nuIn);

	//ʵʱ��ȡ��ǰ�Ĳ���
	void setThrust(Force6 thrust);

	//�������Բ��λ��
	pair<double, double> getCenterPos();

	//���ʵʱĿ��λ��
	pair<double, double> getRTPosDes();

	//���ʵʱĿ������(�ȶ�ʱ���ǻ�����������)
	double getPsiRTDes();

	//����ֵ��(-pi,pi)ת����(-inf,inf)
	double piToInf(double valueIn);

private:

	double xDes, yDes;			// Ŀ��λ��
	double xCenter, yCenter;	// ����Բ�ĵ�λ��
	double x, y, psi;			// ��ǰλ������̬
	double u, v, r;				// ��ǰ�ٶ�����ٶ�
	double xRTDes, yRTDes, psiRTDes;	// ʵʱĿ��λ��������
	double psiRTDesDegree;		// �Ƕ���ʽ��ʵʱĿ������
	double rad;					// �뾶
	double radRT;				// ����������Բ�ĵĵ�ǰ����

	double psiC, prePsiC;		// ������ǰλ���뵱ǰԲ�����ߵĽǶ� �� ǰһʱ�̽Ƕ�
	double psiD;				// ����Ŀ��λ���뵱ǰԲ�����ߵĽǶ�
	double psiCD;				// psiC��psiD֮��
	double dpsiC;				// psiC��΢��

	double kGain, hGain, mGain, pGain;	// ���㻷������ʱ������
	double mAdd;						// ���㻷������ʱ�ĸ�����
	double upLmt, lowLmt;				// ���㻷������ʱ��������
	double ep;							// ���㻷������ʱ���м����

	double tStep;				// ����
	bool usedFlag;				// �Ƿ����й��ı�־

	double xForce;				// �ƽ��������������
	double yForce;				// �ƽ�������ĺ�����

	int state;					//�Ƕȵ�����
	double current_angle, previous_angle;
	double accumulate;	//
};

#endif//WOPC_H_
