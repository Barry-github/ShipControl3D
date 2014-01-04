/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-31							**
******************************************************************/

//Tool.h

#ifndef TOOL_H_
#define TOOL_H_

#include "DataStruct.h"

class Tool
{
public:
	Tool(void);
	~Tool(void);

	//Force6ת��Ϊ����
	static void Force6ToArray(const Force6 &force, double tao[]);

	//����ת��ΪForce6
	static void ArrayToForce6(const double taoArray[], Force6 &tao);

	//Etaת��Ϊ����
	static void Eta6ToArray(const Eta &eta, double etaArray[]);

	//����ת��ΪEta
	static void ArrayToEta(const double etaArray[], Eta &eta);

	//Nuת��Ϊ����
	static void Nu6ToArray(const Nu &nu, double nuArray[]);

	//����ת��ΪNu
	static void ArrayToNu(const double nuArray[], Nu &nu);

	//����ֵת����-PI~PI
	static double infToPi(double inValue);

	//��ʼ��Nu
	static void initNu(Nu &nu);

	//��ʼ��Force6
	static void initForce6(Force6 &force);

	//����Eta
	static Eta setEta(const double x, const double y, const double psi);

	//��ʼ��Eta
	static void initEta(Eta &eta);

	//��ʼ��Ŀ��eta
	static void initEtaTarget(Eta &eta, double x, double y, double psi);

	//��˹��Ԫ����������
	static bool inv(double (*A)[DOF6], double (*B)[DOF6]);

	//����˷���6x6������6x1�������
	static void multiMx(const double (*dataMx1)[DOF6], const double dataMx2[], double resultMx[]);

	//����ӷ���6x6������6x6�������
	static void plusMx( const double (*dataMx1)[DOF6], const double (*dataMx2)[DOF6], double (*resultMx)[DOF6]);

	//����ӷ���3x3������3x1�������
	static void multiVector(double A[][DOF3], double B[], double C[], int sz);

	//�����ɶȵ���ת��Ϊ��ά����
	static void Force6ToArr3(Force6 force, double arr[], int sz);

	//�����ɶȵ��ٶȽ��ٶ�ת��Ϊ��ά����
	static void NuToArr3(Nu nu0, double arr[], int sz);

	//�����ɶ�λ����̬ת��Ϊ��ά����
	static void EtaToArr3(Eta eta0, double arr[], int sz);

	//��ά����ת��Ϊ�����ɶ�λ����̬
	static void Arr3ToEta(double arr[], Eta &eta0, int sz);

	//��ά�������
	static void subArr3(double arr1[], double arr2[], double arr[], int sz);

	//��ά�������
	static void addArr3(double arr1[], double arr2[], double arr[], int sz);

	//��ת����
	static void rotMat(double psi, double inValue[], double outValue[], int sz);
	static void transRot(double psi, double inValue[], double outValue[], int sz);

	//���ɱ�������ϵת��Ϊ��������ϵ
	static Force6 NedToboat(const Force6 &force, const Eta &eta);
};

#endif//TOOL_H_