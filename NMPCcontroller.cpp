/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//NMPCcontroller.cpp

#include "NMPCcontroller.h"
#include <math.h>
#include <iostream>
using namespace std;

NMPCcontroller::NMPCcontroller(void)
{
	init();
}

NMPCcontroller::~NMPCcontroller(void)
{
	outFile.close();
}

//������ʼ��
void NMPCcontroller::init()
{
	//��ʼ��Ԥ��ʱ�䳤��T��Ȩֵ
	T = 9.0;

	lambda[0] = 20.0;		//>=0
	lambda[1] = 20.0;			//>0
	lambda[2] = 0.00000001;			//>=0

	//��ʼ���ƽ������������ص���ֵ
	lmtForce[0] = MAXSURGE;
	lmtForce[1] = MAXSWAY;
	lmtForce[2] = MAXYAW;

	//��ʼ���ƽ���������������
	force.xForce = 0.0;
	force.yForce = 0.0;
	force.zForce = 0.0;
	force.kMoment = 0.0;
	force.mMoment = 0.0;
	force.nMoment = 0.0;

	//��ʼ������ϵͳ���Ծ�����������
	for (int i = 0; i < DOF3; i ++)
	{
		for (int j = 0; j < DOF3; j ++)
		{
			m[i][j] = 0.0;
			d[i][j] = 0.0;
			a[i][j] = 0.0;

			B[i][j] = 0.0;
			C[i][j] = 0.0;

			M3[i][j] = 0.0;
		}
		b[i] = 0.0;
		U[i] = 0.0;
	}

	//ϵͳ���Ծ���(ԭ6���ɶ�ϵͳ���Ծ���򻯶���)
	m[0][0] = 0.0026e+10;
	m[1][1] = 0.0033e+10;
	m[2][2] = 6.5209e+10;

	//�������(ԭ6���ɶ��������򻯶���)
	d[0][0] = 0.0002e+8;
	d[1][1] = 0.0022e+8;
	d[2][2] = 7.1506e+8;

	//��������е��м�ϵ��a
	a[0][0] = -d[0][0]/m[0][0];
	a[1][1] = -(m[2][2]*d[1][1]-m[1][2]*d[2][1])/(m[1][1]*m[2][2]-m[1][2]*m[2][1]);
	a[1][2] = -(m[2][2]*d[1][2]-m[1][2]*d[2][2])/(m[1][1]*m[2][2]-m[1][2]*m[2][1]);
	a[2][1] = -(m[1][1]*d[2][1]-m[2][1]*d[1][1])/(m[1][1]*m[2][2]-m[1][2]*m[2][1]);
	a[2][2] = -(m[1][1]*d[2][2]-m[2][1]*d[1][2])/(m[1][1]*m[2][2]-m[1][2]*m[2][1]);

	//��������е��м�ϵ��b
	b[0] = 1.0/m[0][0];
	b[1] = 1.0/m[1][1];
	b[2] = 1.0/m[2][2];
	
	//��ʼ����������еľ������
	for (int i = 0; i < MAXDIM; i ++)
	{
		for (int j = 0; j < MAXDIM; j ++)
		{
			M[i][j] = 0.0;
		}
		for (int j = 0; j < DOF3; j ++)
		{
			q1[i][j] = 0.0;
			q1T[j][i] = 0.0;
		}
		F[i] = 0.0;
		q0[i] = 0.0;
		yd[i] = 0.0;
	}

	outFile.open("E:/projectProgram/data/nmpcTest.txt");
}

//����M����
void NMPCcontroller::calM()
{
	//����M3��M����
	for (int i = 0; i < DOF3; i ++)
	{
		M3[i][i] = lambda[2]*T;
	}

	M[0][0] = lambda[0] + lambda[1]*T;
	M[0][1] = lambda[0]*T + lambda[1]*pow(T,2.0)/2.0;
	M[0][2] = lambda[0]*pow(T,2.0)/2.0 + lambda[1]*pow(T,3.0)/6.0;
	M[0][3] = lambda[0]*pow(T,3.0)/6.0 + lambda[1]*pow(T,4.0)/24.0;

	M[1][0] = M[0][1];
	M[1][1] = lambda[0]*pow(T,2.0) + lambda[1]*pow(T,3.0)/3.0;
	M[1][2] = lambda[0]*pow(T,3.0)/2.0 + lambda[1]*pow(T,4.0)/8.0;
	M[1][3] = lambda[0]*pow(T,4.0)/6.0 + lambda[1]*pow(T,5.0)/30.0;

	M[2][0] = M[0][2];
	M[2][1] = M[1][2];
	M[2][2] = lambda[0]*pow(T,4.0)/4.0 + lambda[1]*pow(T,5.0)/20.0;
	M[2][3] = lambda[0]*pow(T,5.0)/12.0 + lambda[1]*pow(T,6.0)/72.0;

	M[3][0] = M[0][3];
	M[3][1] = M[1][3];
	M[3][2] = M[2][3];
	M[3][3] = lambda[0]*pow(T,6)/36.0 + lambda[1]*pow(T,7.0)/252.0;

	//�����������еĸ�Ԫ��
	for (int i = 0; i < MAXDIM/3; i ++)
	{
		for (int j = 0; j < MAXDIM/3; j ++)
		{
			M[i+4][j+4] = M[i][j];
			M[i+8][j+8] = M[i][j];
		}
	}
}

//����������
void NMPCcontroller::cal()
{
	//ÿ�μ��㶼��Ҫ��ʼ���ľ������
	for (int i = 0; i < DOF3; i ++)
	{
		for (int j = 0; j < MAXDIM; j ++)
		{
			A[i][j] = 0.0;
			D[i][j] = 0.0;
		}
	}
	for (int i = 0; i < DOF3; i ++)
	{
		for (int j = 0; j < DOF3; j ++)
		{
			B[j][i] = 0.0;
		}
		U[i] = 0.0;
	}

	//����q0����12x1
	q0[0]=x;
	q0[1]=u*cos(psi)-v*sin(psi);
	q0[2]=-u*r*sin(psi)-v*r*cos(psi)+a[0][0]*u*cos(psi)-a[1][1]*v*sin(psi)-a[1][2]*r*sin(psi);
	q0[3]=(-u*r*cos(psi)+v*r*sin(psi)-a[0][0]*u*sin(psi)-a[1][1]*v*cos(psi)-a[1][2]*r*cos(psi))*r
		+(-r*sin(psi)+a[0][0]*cos(psi))*(a[0][0]*u)
		+(-r*cos(psi)-a[1][1]*sin(psi))*(a[1][1]*v+a[1][2]*r)
		+(-u*sin(psi)-v*cos(psi)-a[1][2]*sin(psi))*(a[2][1]*v+a[2][2]*r);

	q0[4]=y;
	q0[5]=u*sin(psi)+v*cos(psi);
	q0[6]=u*r*cos(psi)-v*r*sin(psi)+a[0][0]*u*sin(psi)+a[1][1]*v*cos(psi)+a[1][2]*r*cos(psi);
	q0[7]=(-u*r*sin(psi)-v*r*cos(psi)+a[0][0]*u*cos(psi)-a[1][1]*v*sin(psi)-a[1][2]*r*sin(psi))*(r)
		+(r*cos(psi)+a[0][0]*sin(psi))*(a[0][0]*u)
		+(-r*sin(psi)+a[1][1]*cos(psi))*(a[1][1]*v+a[1][2]*r)
		+(u*cos(psi)-v*sin(psi)+a[1][2]*cos(psi))*(a[2][1]*v+a[2][2]*r);

	q0[8]=psi;
	q0[9]=r;
	q0[10]=a[2][1]*v+a[2][2]*r;
	q0[11]=a[2][1]*a[1][1]*v+a[2][1]*a[1][2]*r+a[2][2]*a[2][1]*v+a[2][2]*a[2][2]*r;

	//����q1����12x3
	q1[2][0]=b[0]*cos(psi);
	q1[2][1]=-b[1]*sin(psi);
	q1[2][2]=0;

	q1[3][0]=b[0]*(-2*r*sin(psi)+a[0][0]*cos(psi));
	q1[3][1]=b[1]*(-2*r*cos(psi)-a[1][1]*sin(psi));
	q1[3][2]=b[2]*(-u*sin(psi)-v*cos(psi)-a[1][2]*sin(psi));


	q1[6][0]=b[0]*sin(psi);
	q1[6][1]=b[1]*cos(psi);
	q1[6][2]=0;

	q1[7][0]=b[0]*(2*r*cos(psi)+a[0][0]*sin(psi));
	q1[7][1]=b[1]*(-2*r*sin(psi)+a[1][1]*cos(psi));
	q1[7][2]=b[2]*(u*cos(psi)-v*sin(psi)+a[1][2]*cos(psi));

	q1[10][0]=0;
	q1[10][1]=0;
	q1[10][2]=b[2];

	q1[11][0]=0;
	q1[11][1]=a[2][1]*b[1];
	q1[11][2]=a[2][2]*b[2];
	
	//����q1T����12x3
	for (int i = 0; i < DOF3; i ++)
	{
		for (int j = 0; j < MAXDIM; j ++)
		{
			q1T[i][j] = q1[j][i];
		}
	}

	//����A=q1T��M����3x12
	for (int i = 0; i < DOF3; i ++)
	{
		for (int j = 0; j < MAXDIM; j ++)
		{
			for (int p = 0; p < MAXDIM; p ++)
			{
				A[i][j] += q1T[i][p] * M[p][j];
			}
		}
	}

	//����B=q1T��M��q1=A��q1����3x3
	for (int i = 0; i < DOF3; i ++)
	{
		for (int j = 0; j < DOF3; j ++)
		{
			for (int p = 0; p < MAXDIM; p ++)
			{
				B[i][j] += A[i][p] * q1[p][j];
			}
		}
	}

	//����C=B+M3����3x3
	for (int i = 0; i < DOF3; i ++)
	{
		for (int j = 0; j < DOF3; j ++)
		{
			C[i][j] = B[i][j] + M3[i][j];
		}
	}

	//�������C����
	inv(C, invC);
	
	//����D=invC��A����3x12
	for (int i = 0; i < DOF3; i ++)
	{
		for (int j = 0; j < MAXDIM; j ++)
		{
			for (int p = 0; p < DOF3; p ++)
			{
				D[i][j] += invC[i][p] * A[p][j];
			}
			outFile << D[i][j] << "\t";
		}
	}
	outFile << endl;

	//����F=q0-yd����12x1
	for (int i = 0; i < MAXDIM; i ++)
	{
		F[i] = q0[i] - yd[i];
	}

	//����U= - DxF ����3x1
	for (int i = 0; i < DOF3; i ++)
	{
		for (int j = 0; j < MAXDIM; j ++)
		{
			U[i] += D[i][j] * F[j];
		}
	}

	//�����ƽ���������������
	for (int i = 0; i < DOF3; i ++)
	{
		if (lmtForce[i] < U[i])
		{
			U[i] = lmtForce[i];
		} else if (-lmtForce[i] > U[i])
		{
			U[i] = -lmtForce[i];
		}
	}

	force.xForce = -U[0] - env.xForce;
	force.yForce = -U[1] - env.yForce;
	force.nMoment = -U[2] - env.nMoment;

}

//����Ԥ������
void NMPCcontroller::setT(const double period)
{
	T = period;
}

//����Ȩֵ
void NMPCcontroller::setWeight(const double lmd1, const double lmd2, const double lmd3)
{
	lambda[0] = lmd1;
	lambda[1] = lmd2;
	lambda[2] = lmd3;
}

//�ٶ�����
void NMPCcontroller::setNu( const Nu nu )
{
	u = nu.u;
	v = nu.v;
	r = nu.r;
}

//λ������
void NMPCcontroller::setEta( const Eta eta )
{
	x = eta.n;
	y = eta.e;
	psi = eta.psi;
}

//����Ŀ��λ����̬
void NMPCcontroller::setTarget( const Eta eta )
{
	yd[0] = eta.n;
	yd[4] = eta.e;
	yd[8] = eta.psi;
}

//������������
void NMPCcontroller::setEnv(const Force3 forcEnv)
{
	env.xForce = forcEnv.xForce;
	env.yForce = forcEnv.yForce;
	env.nMoment = forcEnv.nMoment;
}

//���������
Force6 NMPCcontroller::Force()
{
	return force;
}

//----------------------------------------------
//���ܣ����ò�����Ԫ�ĸ�˹��ȥ����3x3����A�������B
//��ڲ��������뷽��A���������B,�������n
//����ֵ��true or false
//----------------------------------------------
bool NMPCcontroller::inv(double (*A)[DOF3], double (*B)[DOF3])
{
	int i, j, k;
	double max, temp;
	double t[DOF3][DOF3];		//��ʱ����

	//��A����������ʱ����t[][]��
	for (i = 0; i < DOF3; i++)
	{
		for (j = 0; j < DOF3; j++)
		{
			t[i][j] = A[i][j];
		}
	}
	//��ʼ��B����Ϊ��λ��
	for (i = 0; i < DOF3; i++)
	{
		for (j = 0; j < DOF3; j++)
		{
			B[i][j] = (i == j)? (double) 1:0;
		}
	}
	for (i = 0; i < DOF3; i++)
	{
		//Ѱ����Ԫ
		max= t[i][i];
		k= i;
		for (j = i+1; j < DOF3; j++)
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
			for (j = 0;j < DOF3;j++)
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
		for (j = 0; j < DOF3; j++)
		{
			t[i][j] = t[i][j] / temp; //�����Խ����ϵ�Ԫ�ر�Ϊ1
			B[i][j] = B[i][j] / temp; //�������
		}
		for (j = 0; j < DOF3; j++) //��0��->��DOF3-1��
		{
			if (j != i)			//���ǵ�i��
			{
				temp= t[j][i];
				for (k = 0; k < DOF3; k ++)	//��j��Ԫ��-i��Ԫ��*��j��i��Ԫ��
				{
					t[j][k] = t[j][k]- t[i][k]*temp;
					B[j][k] = B[j][k]- B[i][k]*temp;
				}
			}
		}
	}
	return true;
}
