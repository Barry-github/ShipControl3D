/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-31							**
******************************************************************/

//Tool.cpp

#include "Tool.h"
#include <iostream>
using  namespace std;

Tool::Tool(void)
{
}


Tool::~Tool(void)
{
}

//Force6ת��Ϊ����
void Tool::Force6ToArray(const Force6 &tao, double taoArray[])
{
	taoArray[0] = tao.xForce;
	taoArray[1] = tao.yForce;
	taoArray[2] = tao.zForce;
	taoArray[3] = tao.kMoment;
	taoArray[4] = tao.mMoment;
	taoArray[5] = tao.nMoment;
}

//����ת��ΪForce6
void Tool::ArrayToForce6(const double taoArray[], Force6 &tao)
{
	tao.xForce  = taoArray[0]; 
	tao.yForce  = taoArray[1];
	tao.zForce  = taoArray[2]; 
	tao.kMoment = taoArray[3];
	tao.mMoment = taoArray[4];
	tao.nMoment = taoArray[5];
}


//����ֵת����-PI~PI
double Tool::infToPi(double inValue)
{
	while (inValue > PI)
	{
		inValue -= PI*2.0;
	}

	while (inValue <= -PI)
	{
		inValue += PI*2.0;
	}
	return inValue;
}

//��ʼ��Nu
void Tool::initNu(Nu &nu)
{
	nu.u = 0.0;
	nu.v = 0.0;
	nu.w = 0.0;
	nu.p = 0.0;
	nu.q = 0.0;
	nu.r = 0.0;
}

//��ʼ��Force6
void Tool::initForce6(Force6 &force)
{
	force.xForce = 0.0;
	force.yForce = 0.0;
	force.zForce = 0.0;
	force.kMoment = 0.0;
	force.mMoment = 0.0;
	force.nMoment = 0.0;
}

//����Eta
Eta Tool::setEta(const double x, const double y, const double psi)
{
	Eta eta;
	eta.n = x;
	eta.e = y;
	eta.d = 0.0;
	eta.phi = 0.0;
	eta.theta = 0.0;
	eta.psi = psi;
	return eta;
}

//��ʼ��Eta
void Tool::initEta(Eta &eta)
{
	eta.n = 0.0;
	eta.e = 0.0;
	eta.d = 0.0;
	eta.phi = 0.0;
	eta.theta = 0.0;
	eta.psi = 0.0;
}

//��ʼ��Ŀ��eta
void Tool::initEtaTarget( Eta &eta, double x, double y, double psi )
{
	eta.n = x;
	eta.e = y;
	eta.d = 0.0;
	eta.phi = 0.0;
	eta.theta = 0.0;
	eta.psi = psi;
}

//----------------------------------------------
//���ܣ����ò�����Ԫ�ĸ�˹��ȥ����6x6����A�������B
//��ڲ��������뷽��A���������B,�������n
//����ֵ��true or false
//----------------------------------------------
bool Tool::inv(double (*A)[DOF6], double (*B)[DOF6])
{
	double max = 0.0, temp = 0.0;
	double t[DOF6][DOF6];		//��ʱ����

	//��A����������ʱ����t[][]��
	for (int i = 0; i < DOF6; i++)
	{
		for (int j = 0; j < DOF6; j++)
		{
			t[i][j] = A[i][j];
		}
	}
	//��ʼ��B����Ϊ��λ��
	for (int i = 0; i < DOF6; i++)
	{
		for (int j = 0; j < DOF6; j++)
		{
			B[i][j] = (i == j)? (double) 1:0;
		}
	}
	for (int i = 0; i < DOF6; i++)
	{
		//Ѱ����Ԫ
		max= t[i][i];
		int k= i;
		for (int j = i+1; j < DOF6; j++)
		{
			if (fabs(t[j][i]) > fabs(max))
			{
				max= t[j][i];
				k=j;
			}
		}

		//�����Ԫ�����в���i�У������н���
		if (k != i)
		{
			for (int j = 0; j < DOF6;j++)
			{
				temp= t[i][j];
				t[i][j] = t[k][j];
				t[k][j] =temp;
				//B���潻��
				temp= B[i][j];
				B[i][j] = B[k][j];
				B[k][j] =temp;
			}
		}
		//�ж���Ԫ�Ƿ�Ϊ0�� ���ǣ� �����A�������Ⱦ��󣬲����������
		if (t[i][i] == 0)
		{
			cout <<"There is no inverse matrix!";
			return false;
		}
		//��ȥA�ĵ�i���г�ȥi������ĸ���Ԫ��
		temp= t[i][i];
		for (int j = 0; j < DOF6; j++)
		{
			t[i][j] = t[i][j] / temp; //�����Խ����ϵ�Ԫ�ر�Ϊ1
			B[i][j] = B[i][j] / temp; //�������
		}
		for (int j = 0; j < DOF6; j++) //��0��->��DOF6-1��
		{
			if (j != i)			//���ǵ�i��
			{
				temp= t[j][i];
				for (int k = 0; k < DOF6; k ++)	//��j��Ԫ��-i��Ԫ��*��j��i��Ԫ��
				{
					t[j][k] = t[j][k]- t[i][k]*temp;
					B[j][k] = B[j][k]- B[i][k]*temp;
				}
			}
		}
	}
	return true;
}

//����˷���6x6������6x1�������
void Tool::multiMx(const double (*dataMx1)[DOF6], const double dataMx2[], double resultMx[])
{
	for (int i = 0; i < DOF6; ++ i)
	{
		resultMx[i] = 0;
		for (int j = 0; j < DOF6; ++ j)
		{
			resultMx[i] += dataMx1[i][j]*dataMx2[j];
		}
	}
}

//����ӷ���6x6������6x6�������
void Tool::plusMx( const double (*dataMx1)[DOF6], const double (*dataMx2)[DOF6], double (*resultMx)[DOF6])
{
	for (int i = 0; i < DOF6; ++ i)
	{
		for (int j = 0; j < DOF6; ++ j)
		{
			resultMx[i][j] = dataMx1[i][j] + dataMx2[i][j];
		}
	}
}

void Tool::multiVector(double A[][DOF3], double B[], double C[], int sz)
{
	for (int i = 0; i < sz; i ++)
	{
		C[i] = 0.0;
		for (int j = 0; j < sz; j ++)
		{
			C[i] += A[i][j]*B[j];
		}
	}
}

//Etaת��Ϊ����
void Tool::Eta6ToArray(const Eta &eta, double etaArray[])
{
	etaArray[0] = eta.n;
	etaArray[1] = eta.e;
	etaArray[2] = eta.d;
	etaArray[3] = eta.phi;
	etaArray[4] = eta.theta;
	etaArray[5] = eta.psi;
}

//����ת��ΪEta
void Tool::ArrayToEta(const double etaArray[], Eta &eta)
{
	eta.n	= etaArray[0]; 
	eta.e	= etaArray[1];
	eta.d	= etaArray[2]; 
	eta.phi	= etaArray[3];
	eta.theta = etaArray[4];
	eta.psi	= etaArray[5];
}

//Nuת��Ϊ����
void Tool::Nu6ToArray(const Nu &nu, double nuArray[])
{
	nuArray[0] = nu.u;
	nuArray[1] = nu.v;
	nuArray[2] = nu.w;
	nuArray[3] = nu.p;
	nuArray[4] = nu.q;
	nuArray[5] = nu.r;
}

//����ת��ΪNu
void Tool::ArrayToNu(const double nuArray[], Nu &nu)
{
	nu.u = nuArray[0]; 
	nu.v = nuArray[1];
	nu.w = nuArray[2]; 
	nu.p = nuArray[3];
	nu.q = nuArray[4];
	nu.r = nuArray[5];
}

//�����ɶȵ���ת��Ϊ��ά����
void Tool::Force6ToArr3(Force6 force, double arr[], int sz)
{
	arr[0] = force.xForce;
	arr[1] = force.yForce;
	arr[2] = force.nMoment;
}

//�����ɶȵ��ٶȽ��ٶ�ת��Ϊ��ά����
void Tool::NuToArr3(Nu nu0, double arr[], int sz)
{
	arr[0] = nu0.u;
	arr[1] = nu0.v;
	arr[2] = nu0.r;
}

//�����ɶ�λ����̬ת��Ϊ��ά����
void Tool::EtaToArr3(Eta eta0, double arr[], int sz)
{
	arr[0] = eta0.n;
	arr[1] = eta0.e;
	arr[2] = eta0.psi;
}

//��ά����ת��Ϊ�����ɶ�λ����̬
void Tool::Arr3ToEta(double arr[], Eta &eta0, int sz)
{
	eta0.n = arr[0];
	eta0.e = arr[1];
	eta0.d = 0.0;
	eta0.phi = 0.0;
	eta0.theta = 0.0;
	eta0.psi = arr[2];
}

//��ά�������
void Tool::subArr3(double arr1[], double arr2[], double arr[], int sz)
{
	for (int i = 0; i < sz; i ++)
	{
		arr[i] = arr1[i] - arr2[i];
	}
}

//��ά�������
void Tool::addArr3(double arr1[], double arr2[], double arr[], int sz)
{
	for (int i = 0; i < sz; i ++)
	{
		arr[i] = arr1[i] + arr2[i];
	}
}

//��ת����
void Tool::rotMat(double psi, double inValue[], double outValue[], int sz)
{
	outValue[0] = cos(psi)*inValue[0] - sin(psi)*inValue[1];
	outValue[1] = sin(psi)*inValue[0] + cos(psi)*inValue[1];
	outValue[2] = inValue[2];
}

void Tool::transRot(double psi, double inValue[], double outValue[], int sz)
{
	outValue[0] = cos(psi)*inValue[0] + sin(psi)*inValue[1];
	outValue[1] = -sin(psi)*inValue[0] + cos(psi)*inValue[1];
	outValue[2] = inValue[2];
}

//���ɱ�������ϵת��Ϊ��������ϵ
Force6 Tool::NedToboat(const Force6 &force, const Eta &eta)
{
	Force6 forceResult;
	forceResult.xForce = force.xForce*cos(eta.psi)+force.yForce*sin(eta.psi);
	forceResult.yForce = -force.xForce*sin(eta.psi)+force.yForce*cos(eta.psi);
	forceResult.zForce = force.zForce;
	forceResult.kMoment = force.kMoment;
	forceResult.mMoment = force.mMoment;
	forceResult.nMoment = force.nMoment;
	return forceResult;
}