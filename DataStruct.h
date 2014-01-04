/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//DataStruct.h

#ifndef DATASTRUCT_H_
#define DATASTRUCT_H_

#include <string>
using namespace std;

#define DOF3 3
#define DOF6 6
#define wNum 36
#define velTotal 5
#define visDampNum 5
#define PI	3.141592653589793
#define E	2.718281828459046

//�ٶȵ���ÿ��ת������
#define M2KNOT 0.5144

//�Ƕ�ת����ϵ��
#define angToRad (PI/180)
//����ת�Ƕ�ϵ��
#define radToAng (180/PI)
//�������ٶ�
#define g 9.81
//vesselABC��freqs��
#define freqNum 1146
//Hoerner���ݵĸ���
#define  HoerNum 20

#pragma pack(1)

//����ת�ؽṹ��
struct Force3
{
	double xForce;		//������
	double yForce;		//������
	double nMoment;		//ת��
};

struct Force6
{
	double xForce;		//������
	double yForce;		//������
	double zForce;		//������
	double kMoment;		//ת��
	double mMoment;		//ת��
	double nMoment;		//ת��
};

//λ�ú���̬�ṹ��
struct Eta
{
	double n;			//��λ��
	double e;			//��λ��
	double d;			//
	double phi;			//��ҡ�� ����
	double theta;		//��ҡ�� ����
	double psi;			//����� ����
};

//���ٶȺͽ��ٶȽṹ��
struct Nu
{
	double u;			//�����ٶ�
	double v;			//�����ٶ�
	double w;			//�����ٶ�
	double p;			//��ҡ���ٶ�
	double q;			//��ҡ���ٶ�
	double r;			//��ҡ���ٶ�
};

struct Coord
{
	double x;
	double y;
	double z;
};

struct VesselMain
{
	string shipName;
	double Lpp;
	double T;
	double B;
	double rho;
	double m;
	double nabla;
	double GM_L;
	double GM_T;
	double C_B;
	double Lwl;
	double S;
	Coord CG;
	Coord CB;
};

struct ForceRAO
{
	double amp[DOF6][velTotal][wNum][wNum];
	double phase[DOF6][velTotal][wNum][wNum];
	double w[wNum];
};

struct Driftfrc
{
	double amp[DOF6][velTotal][wNum][wNum];
	double w[wNum];
};

//vessel�ڵ�����
struct VesselData
{
	VesselMain main;
	double MRB[DOF6][DOF6];
	ForceRAO forceRAO;
	Driftfrc driftfrc;
	double vel[velTotal];
};

//vesselABC�ڵ�����
struct VesselABCData
{
	double Ainf[DOF6][DOF6];
	double Binf[DOF6][DOF6];
	double Ar[DOF6][DOF6][velTotal][velTotal];
	double Br[DOF6][DOF6][velTotal];
	double Cr[DOF6][DOF6][velTotal];
	double Dr[DOF6][DOF6];
	double B44_inf[velTotal];
	double A44[velTotal][velTotal][velTotal];
	double B44[velTotal][velTotal];
	double C44[velTotal][velTotal];
	double D44[velTotal];
	double MA[DOF6][DOF6];
	double G[DOF6][DOF6];
	double Minv[DOF6][DOF6];
	Coord r_g;
	double A[freqNum][DOF6][DOF6];
	double B[freqNum][DOF6][DOF6];
	double freqs[freqNum];

};

struct Data
{
	VesselData dataVes;
	VesselABCData dataVesABC;
	double Mmx[DOF6/2][DOF6/2];
	double Dmx[DOF6/2][DOF6/2];
	double CDdata[2][HoerNum];
};

struct xStatOut
{
	double x1;
	double x2;
	double x3;
	double x4;
	double x5;
};

#pragma pack()

#endif//DATASTRUCT_H_