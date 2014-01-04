/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-17							**
******************************************************************/

//Wind.h

#ifndef WIND_H_
#define WIND_H_

#include "DataStruct.h"

#define rowPara 19
#define colPara 7

class Wind
{
public:
	Wind(void);
	~Wind(void);

	//��ʼ������
	void init();

	//���÷��ٷ���
	void setPara(double speedTrue, double angTrue);

	//���ô�������,����
	void setHead(const double head);

	//���ô����ٶ�
	void setNu(const Nu nuShip);

	//��ȡ��Դ���������
	Force6 getWindTao();

	//�������
	void cal();

	//��ֵ���� ������Ϊ�Ƕ�
	void interp(const double valInit[rowPara][colPara], double result[colPara], double ang);

private:
	double L_oa;  			//��ȫ�� (m)
	double B0;	            //���� (m)
	double H_s;             //������ˮ�����ϲ��ֲ������ĵ�ˮ�߾��� (m)
	double A_s;             //ˮ�����ϴ�����ͶӰ��� (m^2)
	double A_f;             //ˮ�����ϴ�����ͶӰ��� (m^2)--������ͶӰ
	double A_ss;            //�������ϲ㽨���Ĳ�ͶӰ��� (m^2)
	double wind_c;	        //����ˮ�ߺʹ�Φ��ͨ���豸��ϸ������Ĵ�ģ�͵Ĳ�ͶӰ�ܳ� (m)
	double wind_e;			//�Ӵ��׵���ͶӰ����ľ��ĵľ��� (m)
	double wind_M;          // ��ͶӰ���ڿɼ��Ĵ�Φ�������Ĳ�ͬ��������������������ǰ��������
	double rho_a;           //20 C �Ŀ����ܶ� kg/m^3
	double V_T;             //���Է���
	double ang_T;			//���Է���
	double angEnc;			//������
	double psi;				//�����
	double V_R;				//��Է���

	Nu nu;					//�����ٶ�

	Force6 tao;				//��Դ���������

	//xyn�����ѹ��ϵ������
	double AA[rowPara][colPara], BB[rowPara][colPara], CC[rowPara][colPara];
	double A[rowPara], B[rowPara], C[rowPara];
	double Cx, Cy, Cn;
};


#endif//WIND_H_