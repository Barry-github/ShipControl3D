/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//ShipModel.cpp

#include "ShipModel.h"
#include "Tool.h"
#include <mat.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

ShipModel::ShipModel(void)
{
	//��ʼ��
	init();	
}


ShipModel::~ShipModel(void)
{
	//delete data;
}

//��ʼ��
void ShipModel::init()
{	
	tStep = 0.0;

	for (int i = 0; i < DOF6; i ++)
	{
		MinvTao[i] = 0.0;

		muArray[i] = 0.0;
		dragArray[i] = 0.0;
		sprStifArray[i] = 0.0;
		dampArray[i] = 0.0;

		taoArray[i] = 0.0;
		totForceArray[i] = 0.0;
		etaArray[i] = 0.0;
		nuArray[i] = 0.0;

		for (int j = 0; j < DOF6; j ++)
		{
			M[i][j]			= 0.0;
			datMinv[i][j]	= 0.0;
		}
	}

	for (int i = 0; i < DOF3; i ++)
	{
		for (int j = 0; j < DOF3; j ++)
		{
			RotMx[i][j]		= 0.0;
			TransMx[i][j]	= 0.0;
		}
	}

	data = new Data;

	//��ʼ�������ٶ�
	Tool::initNu(nu);
	Tool::initForce6(taoForce);
}

//��ʼ����������
void ShipModel::setData(Data *data_)
{
	data = data_;
}

//����ʱ����
void ShipModel::setStep(const double step)
{
	tStep = step;
}

//������
void ShipModel::setForce(Force6 force)
{
	taoForce = force;
}

//���봬���ĳ�ʼλ����̬
void ShipModel::setInitEta(Eta etaIn)
{
	eta = etaIn;
}

//���λ����̬
Eta ShipModel::getEta()
{
	return eta;
}

//����ٶȽ��ٶ�
Nu ShipModel::getNu()
{
	return nu;
}

//����M����
void ShipModel::calM()
{
	//������Ծ���M
	Tool::plusMx(data->dataVesABC.Ainf, data->dataVes.MRB, M);
	//������Ծ���M����
	Tool::inv(M, datMinv);
}

//Cross-flow drag and surge resistance
//�����Ĺ�ʽ�У�ϵ��Ϊ��ֵ����simulink������ϵ���Ǹ�ֵ
Force6 ShipModel::crosFlowDrag(Nu nu)
{
	Force6 force;
	double rho_w, Lpp, B, T, Ax, Ay, CX, CY_2D;
	double xDrag = 0.0, yDrag = 0.0, nDrag = 0.0;
	rho_w = data->dataVes.main.rho;
	Lpp = data->dataVes.main.Lpp;
	B = data->dataVes.main.B;
	T = data->dataVes.main.T;
	Ax = 0.9*T*B;
	Ay = 0.9*T*Lpp;
	CX = 1;
	CY_2D = Hoerner(B, T);
	xDrag = -0.5*rho_w*CX*Ax*nu.u*fabs(nu.u);
	yDrag = -0.5*rho_w*Ay/Lpp*CY_2D*integrt(nu, 0.5*Lpp, -0.5*Lpp, 0.01, true);
	nDrag = -0.5*rho_w*Ay/Lpp*CY_2D*integrt(nu, 0.5*Lpp, -0.5*Lpp, 0.01, false);

	force.xForce = xDrag;
	force.yForce = yDrag;
	force.nMoment = nDrag;
	force.zForce = 0;
	force.kMoment = 0;
	force.mMoment = 0;
	return force;
}
//������Y��N Dragʱ���ú���;flagΪ��ʱ�����Ϊ��yDrag���ã����򣬽��Ϊ��nDrag����
double ShipModel::func(const Nu &nu, const double &x, bool flag)
{
	if (flag)
	{
		return (nu.v + x*nu.r)*fabs(nu.v + x*nu.r);
	} else {
		return x*(nu.v + x*nu.r)*fabs(nu.v + x*nu.r);
	}
}

//����,�ۼ�
double ShipModel::integrt(Nu &nu, double max, double min, double delta, bool flag)
{
	double funPre = 0.0, funPost = 0.0;
	double result = 0.0;
	for (double elem = min; elem < max; elem += delta)
	{
		funPre = func(nu, elem, flag);
		funPost = func(nu, elem+delta, flag);
		result += funPre*delta + (funPost-funPre)*delta/2.0;
	}
	return result;
}

//Hoerner����
double ShipModel::Hoerner(double BValue,double TValue)
{
	double x[HoerNum], y[HoerNum];
	double hData = BValue/(2*TValue);
	double result = 0;
	for(int i = 0; i < HoerNum; ++ i)
	{
		x[i] = data->CDdata[0][i];
		y[i] = data->CDdata[1][i];
	}
	for (int i = 0; i < HoerNum-1; ++ i)
	{
		for (int j = i+1; j < HoerNum; ++ j)
		{
			if (x[i]>x[j])
			{
				swap(x[i],x[j]);
				swap(y[i],y[j]);
			}
		}
	}
	if (hData>x[0]&&hData<x[HoerNum-1])
	{
		int index = 0;
		while (!(hData>=x[index]&&hData<=x[index+1]))
		{
			index ++;
		}
		result = (y[index+1]-y[index])/(x[index+1]-x[index])*(hData-x[index])+y[index];
	} else if (hData<x[0]) {
		result = (y[1]-y[0])/(x[1]-x[0])*(hData-x[0])+y[0];
	} else {
		result = (y[HoerNum-1]-y[HoerNum-2])/(x[HoerNum-1]-x[HoerNum-2])*(hData-x[HoerNum-2])+y[HoerNum-2];
	}
	return result;
}

//����������
void ShipModel::swap(double &a, double &b)
{
	double temp;
	temp = a;
	a = b;
	b = temp;
}

//��״̬�������
double ShipModel::solStateSpaceFunc(const double (*Ar)[visDampNum], const double Br[], const double Cr[], const double Dr, const double var, double xTemp[])
{
	double x[visDampNum] = {0.0}, y = 0.0;

	for (int i = 0; i < visDampNum; ++ i)
	{
		for (int j = 0; j < visDampNum; ++ j)
		{
			x[i] += Ar[i][j]*xTemp[j];
		}
		x[i] = xTemp[i] + (x[i] + Br[i]*var)*tStep;
	}

	for (int i = 0; i < visDampNum; ++ i)
	{
		y += Cr[i]*x[i];
		xTemp[i] = x[i];
	}
	y += Dr*var;
	return y;
}

//����ճ���������
Force6 ShipModel::viscousDamp(const Nu &nu)
{
	Force6 muResult;
	double result[DOF6] = {0}, xTemp[DOF6] = {0};
	static double xArray[DOF6][DOF6][visDampNum] = {0.0};
	double varIn[DOF6] = {nu.u, nu.v, nu.w, nu.p, nu.q, nu.r};

	for (int i = 0; i < DOF6; ++ i)
	{
		for (int j = 0; j < DOF6/2; ++ j)
		{

			if (3 == i && 1 == j)
			{
				result[i] += rollDamp(varIn[j*2+1]);
			} else {
				if (0 == i%2)
				{
					result[i] += solStateSpaceFunc(data->dataVesABC.Ar[i][j*2], data->dataVesABC.Br[i][j*2], data->dataVesABC.Cr[i][j*2], data->dataVesABC.Dr[i][j*2], varIn[j*2], xArray[i][j*2]);
				} else {
					result[i] += solStateSpaceFunc(data->dataVesABC.Ar[i][j*2+1], data->dataVesABC.Br[i][j*2+1], data->dataVesABC.Cr[i][j*2+1], data->dataVesABC.Dr[i][j*2+1], varIn[j*2+1], xArray[i][j*2+1]);
				}
			}
		}
	}
	Tool::ArrayToForce6(result, muResult);

	return muResult;
}

//roll damping
double ShipModel::rollDamp(const double &varIn)
{
	double kVisc = 0.0, kAdd = 0.0;
	double BValue[visDampNum], x[visDampNum] = {0.0};
	static double xTemp[visDampNum] = {0.0};
	kAdd = varIn*(data->dataVesABC.B44_inf[0]+data->dataVesABC.D44[0]);
	for (int i = 0; i < visDampNum; ++ i)
	{
		BValue[i] = varIn*data->dataVesABC.B44[0][i];
		for (int j = 0; j < visDampNum; ++ j)
		{
			x[i] += data->dataVesABC.A44[0][i][j] * xTemp[j];
		}
		x[i] += BValue[i];
		xTemp[i] = x[i]*tStep;
	}

	for (int i = 0; i < visDampNum; ++ i)
	{
		kVisc += xTemp[i] * data->dataVesABC.C44[0][i];
	}
	kVisc += kAdd;
	return kVisc;
}

//������������
void ShipModel::cal()
{
	Tool::Force6ToArray(taoForce, taoArray);
	//�������
	for (int i = 0; i < DOF6; ++ i)
	{
		totForceArray[i] = taoArray[i] + dragArray[i] - sprStifArray[i] - muArray[i] - dampArray[i];
	}

	Tool::multiMx(datMinv, totForceArray, MinvTao);

	//���㴬���ٶ�
	for (int i = 0; i < DOF6; ++ i)
	{
		nuArray[i] += MinvTao[i]*tStep;
	}

	Tool::ArrayToNu(nuArray, nu);

	//����Spring stiffness, damping
	Tool::multiMx(data->dataVesABC.Binf, nuArray, dampArray);

	//h-frame potential / viscous damping
	mu = viscousDamp(nu);

	//Cross-flow drag and surge resistance
	drag = crosFlowDrag(nu);

	//���㴬��λ�ú���̬
	nuToEta(eta, nu, tStep);
	
	//�����ɶȵ���������ת��Ϊ����
	Tool::Eta6ToArray(eta, etaArray);
	Tool::Force6ToArray(mu, muArray);
	Tool::Force6ToArray(drag, dragArray);

	//����Spring stiffness
	Tool::multiMx(data->dataVesABC.G, etaArray, sprStifArray);

}

//�����ɶ��£��б�������ϵ��������ϵ��ת
void ShipModel::nuToEta(Eta &eta, Nu nu, double intev)
{
	double phi = eta.phi;
	double theta = eta.theta;
	double psi = eta.psi;
	RotMx[0][0] = cos(psi)*cos(theta);
	RotMx[0][1] = -sin(psi)*cos(phi) + cos(psi)*sin(theta)*sin(phi);
	RotMx[0][2] = sin(psi)*sin(phi) + cos(psi)*cos(phi)*sin(theta);
	RotMx[1][0] = sin(psi)*cos(theta);
	RotMx[1][1] = cos(psi)*cos(phi) + sin(phi)*sin(theta)*sin(psi);
	RotMx[1][2] = -cos(psi)*sin(phi) + sin(theta)*sin(psi)*cos(phi);
	RotMx[2][0] = -sin(theta);
	RotMx[2][1] = cos(theta)*sin(phi);
	RotMx[2][2] = cos(theta)*cos(phi);

	TransMx[0][0] = 1;
	TransMx[0][1] = sin(phi)*tan(theta);
	TransMx[0][2] = cos(phi)*tan(theta);
	TransMx[1][0] = 0;
	TransMx[1][1] = cos(phi);
	TransMx[1][2] = -sin(phi);
	TransMx[2][0] = 0;
	TransMx[2][1] = sin(phi)/cos(theta);
	TransMx[2][2] = cos(phi)/cos(theta);

	eta.n += (RotMx[0][0]*nu.u+RotMx[0][1]*nu.v+RotMx[0][2]*nu.w)*intev;
	eta.e += (RotMx[1][0]*nu.u+RotMx[1][1]*nu.v+RotMx[1][2]*nu.w)*intev;
	eta.d += (RotMx[2][0]*nu.u+RotMx[2][1]*nu.v+RotMx[2][2]*nu.w)*intev;
	eta.phi   += (TransMx[0][0]*nu.p+TransMx[0][1]*nu.q+TransMx[0][2]*nu.r)*intev;
	eta.theta += (TransMx[1][0]*nu.p+TransMx[1][1]*nu.q+TransMx[1][2]*nu.r)*intev;
	eta.psi   += (TransMx[2][0]*nu.p+TransMx[2][1]*nu.q+TransMx[2][2]*nu.r)*intev;
}
