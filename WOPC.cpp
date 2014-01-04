/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//WOPC.cpp

#include "WOPC.h"
#include "Tool.h"
#include <math.h>
#include <iostream>
using namespace std;

WOPC::WOPC(void)
{
	init();
}


WOPC::~WOPC(void)
{
}

//��ʼ��
void WOPC::init()
{
	xCenter = xDes;
	yCenter = yDes;

	psiC = 0.0;
	psiD = 0.0;
	prePsiC = 0.0;

	mAdd = 0.004;
	mGain = 0.05;
	hGain = 1.0;
	pGain = 0.00001;
	kGain = 1.0;

	ep = 0.0;

	upLmt = 0.03;
	lowLmt = 0.0;

	rad = 0.0;
	radRT = 0.0;

	usedFlag = false;

	xForce = 1.0;
	yForce = 0.0;

	state = 6;	//��ʾ�Ƕ���0
	current_angle = 0.0;
	previous_angle = 0.0;
	accumulate = 0.0;
}

//�趨������
void WOPC::centerControl()
{
	xCenter = xDes + rad*cos(psiD);
	yCenter = yDes + rad*sin(psiD);
}

//���㻷����������
void WOPC::optHeadCal()
{
	//����kGain
	psiC = atan2(yCenter-y, xCenter-x);
	psiC = piToInf(psiC);
	if (!usedFlag)
	{
		psiD = atan2(yCenter-yDes, xCenter-xDes);
		psiD = piToInf(psiD);
		usedFlag = true;
	}

	psiCD = psiC - psiD;

	psiCD = Tool::infToPi(psiCD);

	dpsiC = (psiC-prePsiC)/ tStep;

	ep += (fabs(dpsiC) -  ep) * mGain * tStep;
	
	kGain = ep + mAdd;

	//����kGain
	if (upLmt < kGain)
	{
		kGain = upLmt;
	} else if (lowLmt > kGain)
	{
		kGain = lowLmt;
	}

	//��¼��ǰpsiC�����´���
	prePsiC = psiC;

	//�����µ�psiD
	psiD += kGain * psiCD * pGain * tStep * yForce;
	
	psiD = Tool::infToPi(psiD);
}

//������������
void WOPC::wohc()
{
	xRTDes = xCenter - rad*cos(psiC);
	yRTDes = yCenter - rad*sin(psiC);
	psiRTDes = psiC;
}

//���Ƽ���
void WOPC::calculat()
{
	//���㻷����������
	optHeadCal();

	//��������Բ��
	centerControl();

	//������������
	wohc();
}

//����ʱ����
void WOPC::setStep(const double dtime)
{
	tStep = dtime;
}

//��������Բ�İ뾶
void WOPC::setRadius(const double radius)
{
	rad = radius;
}

//����Ŀ��λ��
void WOPC::setPos(const Eta etaTarget)
{
	xDes = etaTarget.n;
	yDes = etaTarget.e;
}

//ʵʱ��ȡ��ǰλ��������
void WOPC::setEta(const Eta etaIn)
{
	x = etaIn.n;
	y = etaIn.e;
	psi = etaIn.psi;
}

//ʵʱ��ȡ��ǰ�ٶ�����ٶ�
void WOPC::setNu(const Nu nuIn)
{
	u = nuIn.u;
	v = nuIn.v;
	r = nuIn.r;
}

//ʵʱ��ȡ��ǰ�Ĳ���
void WOPC::setThrust(Force6 thrust)
{
	xForce = thrust.xForce;
	yForce = thrust.yForce;
}

//�������Բ��λ��
pair<double, double> WOPC::getCenterPos()
{
	return pair<double, double>(xCenter, yCenter);
}

//���ʵʱĿ��λ��
pair<double, double> WOPC::getRTPosDes()
{
	return pair<double, double>(xRTDes, yRTDes);
}

//���ʵʱĿ������(�ȶ�ʱ���ǻ�����������)
double WOPC::getPsiRTDes()
{
	return psiRTDes;
}

//����ֵ��(-pi,pi)ת����(-inf,inf)
double WOPC::piToInf(double valueIn)
{
	double valueResult = 0.0;

	current_angle = valueIn;
	if( current_angle>0 && current_angle<(PI/2) ) {                // 1.quadrant
		if(state==1) { // Coming from the 1.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==2) { // Coming from the 2.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==3) { // Coming from the 3.quadrant
			if( (current_angle+abs(previous_angle))<=PI ) {
				accumulate = current_angle - previous_angle;
			}
			else {
				accumulate = previous_angle - current_angle + 2*PI;
			}
		}
		else if(state==4) { // Coming from the 4.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==5) { // Coming from pi/2
			accumulate = current_angle - previous_angle;
		}
		else if(state==6) { // Coming from 0
			accumulate = current_angle;
		}
		else if(state==7) { // Coming from -pi/2
			accumulate = current_angle - previous_angle;
		}
		else if(state==8) { // Coming from -pi/pi
			accumulate = current_angle - abs(previous_angle);
		}
		else { // Simulation has just started
			accumulate = current_angle - previous_angle;
		}
		state = 1;
	}
	else if( current_angle<0 && current_angle>(-PI/2) ) {           // 2.quadrant
		if(state==1) { // Coming from the 1.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==2) { // Coming from the 2.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==3) { // Coming from the 3.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==4) { // Coming from the 4.quadrant
			if( (abs(current_angle)+previous_angle)<=PI ) {
				accumulate = current_angle - previous_angle;
			}
			else {
				accumulate = current_angle - previous_angle + 2*PI;
			}
		}
		else if(state==5) { // Coming from pi/2
			accumulate = current_angle - previous_angle;
		}
		else if(state==6) { // Coming from 0
			accumulate = current_angle;
		}
		else if(state==7) { // Coming from -pi/2
			accumulate = current_angle - previous_angle;
		}
		else if(state==8) { // Coming from -pi/pi
			accumulate = current_angle + PI;
		}
		else { // Simulation has just started
			accumulate = current_angle - previous_angle;
		}
		state = 2;
	}
	else if( current_angle<(-PI/2) && current_angle>(-PI) ) {       // 3.quadrant
		if(state==1) { // Coming from the 1.quadrant
			if( (abs(current_angle)+previous_angle)<=PI ) {
				accumulate = current_angle - previous_angle;
			}
			else {
				accumulate = current_angle - previous_angle + 2*PI;
			}
		}
		else if(state==2) { // Coming from the 2.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==3) { // Coming from the 3.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==4) { // Coming from the 4.quadrant
			accumulate = current_angle - previous_angle + 2*PI;
		}
		else if(state==5) { // Coming from pi/2
			accumulate = 3*PI/2 - current_angle;
		}
		else if(state==6) { // Coming from 0
			accumulate = current_angle;
		}
		else if(state==7) { // Coming from -pi/2
			accumulate = current_angle - previous_angle;
		}
		else if(state==8) { // Coming from -pi/pi
			accumulate = current_angle + PI;
		}
		else { // Simulation has just started
			accumulate = current_angle - previous_angle;
		}
		state = 3;
	}
	else if( current_angle>(PI/2) && current_angle<(PI) ) {         // 4.quadrant
		if(state==1) { // Coming from the 1.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==2) { // Coming from the 2.quadrant
			if( (current_angle+abs(previous_angle))<=PI ) {
				accumulate = current_angle - previous_angle;
			}
			else {
				accumulate = previous_angle - current_angle + 2*PI;
			}
		}
		else if(state==3) { // Coming from the 3.quadrant
			accumulate = current_angle - previous_angle - 2*PI;
		}
		else if(state==4) { // Coming from the 4.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==5) { // Coming from pi/2
			accumulate = current_angle - previous_angle;
		}
		else if(state==6) { // Coming from 0
			accumulate = current_angle;
		}
		else if(state==7) { // Coming from -pi/2
			accumulate = current_angle -3*PI/2;
		}
		else if(state==8) { // Coming from -pi/pi
			accumulate = current_angle - PI;
		}
		else { // Simulation has just started
			accumulate = current_angle - previous_angle;
		}
		state = 4;
	}
	else if( current_angle==(PI/2) ) {         // pi/2 line
		if(state==1) { // Coming from the 1.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==2) { // Coming from the 2.quadrant
			accumulate = current_angle - previous_angle;
		}
		else if(state==3) { // Coming from the 3.quadrant
			accumulate = -3*PI/2 - previous_angle;
		}
		else if(state==4) { // Coming from the 4.quadrant
			accumulate = PI/2 - previous_angle;
		}
		else if(state==5) { // Coming from pi/2
			accumulate = 0;
		}
		else if(state==6) { // Coming from 0
			accumulate = current_angle;
		}
		else if(state==7) { // Coming from -pi/2
			accumulate = PI; // By choice (could also have subtracted pi, this would be equal from an assessment based on the Guide Rule)
		}
		else if(state==8) { // Coming from -pi/pi
			accumulate = -PI/2;
		}
		else { // Simulation has just started
			accumulate = current_angle - previous_angle;
		}
		state = 5;
	}
	else if( current_angle==0 ) {         // 0 line
		if(state==1) { // Coming from the 1.quadrant
			accumulate = -previous_angle;
		}
		else if(state==2) { // Coming from the 2.quadrant
			accumulate = -previous_angle;
		}
		else if(state==3) { // Coming from the 3.quadrant
			accumulate = -previous_angle;
		}
		else if(state==4) { // Coming from the 4.quadrant
			accumulate = -previous_angle;
		}
		else if(state==5) { // Coming from pi/2
			accumulate = -previous_angle;
		}
		else if(state==6) { // Coming from 0
			accumulate = 0;
		}
		else if(state==7) { // Coming from -pi/2
			accumulate = -previous_angle;
		}
		else if(state==8) { // Coming from -pi/pi
			accumulate = PI; // By choice (could also have subtracted pi, this would be equal from an assessment based on the Guide Rule)
		}
		else { // Simulation has just started
			accumulate = current_angle - previous_angle;
		}
		state = 6;
	}
	else if( current_angle==(-PI/2) ) {         // -PI/2 line
		if(state==1) { // Coming from the 1.quadrant
			accumulate = -previous_angle - PI/2;
		}
		else if(state==2) { // Coming from the 2.quadrant
			accumulate = -previous_angle - PI/2;
		}
		else if(state==3) { // Coming from the 3.quadrant
			accumulate = -previous_angle - PI/2;
		}
		else if(state==4) { // Coming from the 4.quadrant
			accumulate = 3*PI/2 - previous_angle;
		}
		else if(state==5) { // Coming from pi/2
			accumulate = PI; // By choice (could also have subtracted pi, this would be equal from an assessment based on the Guide Rule)
		}
		else if(state==6) { // Coming from 0
			accumulate = current_angle;
		}
		else if(state==7) { // Coming from -pi/2
			accumulate = 0;
		}
		else if(state==8) { // Coming from -pi/pi
			accumulate = PI/2;
		}
		else { // Simulation has just started
			accumulate = current_angle - previous_angle;
		}
		state = 7;
	}
	else { //( current_angle==-PI || current_angle==PI ) )         // -pi/pi line
		if(state==1) { // Coming from the 1.quadrant
			accumulate = PI - previous_angle;
		}
		else if(state==2) { // Coming from the 2.quadrant
			accumulate = -PI - previous_angle;
		}
		else if(state==3) { // Coming from the 3.quadrant
			accumulate = -PI - previous_angle;
		}
		else if(state==4) { // Coming from the 4.quadrant
			accumulate = PI - previous_angle;
		}
		else if(state==5) { // Coming from pi/2
			accumulate = PI/2;
		}
		else if(state==6) { // Coming from 0
			accumulate = PI; // By choice (could also have subtracted pi, this would be equal from an assessment based on the Guide Rule)
		}
		else if(state==7) { // Coming from -pi/2
			accumulate = PI/2;
		}
		else if(state==8) { // Coming from -pi/pi
			accumulate = 0;
		}
		else { // Simulation has just started
			accumulate = current_angle - previous_angle;
		}
		state = 8;
	}

	valueResult = previous_angle;
	previous_angle = valueIn;

	return valueResult+accumulate;
}