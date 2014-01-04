/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2013-12-31							**
******************************************************************/

//ShipControl.cpp

#include "ShipControl.h"
#include "Tool.h"
#include <mat.h>

ShipControl::ShipControl(void) :
tStep(0.05), time(0)
{
	//��ʼ��
	init();	

	//�û����г�ʼ��
	userInterFace();

	//��ʼ����������Ϣ
	wind.setPara(SpeedWind, DirWind);
	cur.setPara(SpeedCurrent, DirCurrent);
}


ShipControl::~ShipControl(void)
{
	//�ر��ļ�
	closeFiles();

	//delete data;
}

//��ʼ��
void ShipControl::init()
{
	//���ļ�
	openFiles();

	//������ʱ��
	maxTime = 1000.0;

	//�趨���٣�����
	SpeedWind	= 10.0;
	DirWind		= 90.0;

	//�趨�˸ߣ�����
	HeightWave	= 2.0;
	DirWave		= 150.0;

	//�趨���٣�����
	SpeedCurrent	= 1.0;
	DirCurrent		= 120.0;

	//��ʼλ��������
	xOrigin		= 0.0;
	yOrigin		= 0.0;
	psiOrigin	= 0.0;

	//Ŀ��λ��������
	xTarget		= 100.0;
	yTarget		= 100.0;
	psiTarget	= 30.0;

	//��ʼ��������������
	optPsi = 0.0;

	//��ʼ��PID����
	kp = 0.15;
	ki = 0.0;
	kd = 0.0;

	//��ʼ��NMPC��Ԥ������
	Tpre = 9.0;
	//��ʼ��NMPC������Ȩֵ
	w1 = 0.9;
	w2 = 0.0005;
	w3 = 0.0;

	//ZPC-W����������
	kpZ = 1e-8;
	kiZ = 0.0;
	kdZ = 0.0;

	//�������Ƶ���������
	k1Env = 0.8;
	k2Env = 2.0;
	k3Env = 2.0;


	for (int i = 0; i < DOF6; i ++)
	{
		thrustArray[i] = 0.0;
		wave1Array[i] = 0.0;
		wave2Array[i] = 0.0;
		windArray[i] = 0.0;
		curArray[i] = 0.0;
		taoArray[i] = 0.0;
	}

	data = new Data;

	srand(int(time));

	ctrlCount = 0;
	ctrlCyc = 6;

	//�������Ŷ�����λ�뾶
	radius = 60.0;

	//��ʼ�������ٶ�
	Tool::initNu(nu);

	//��ʼ��������λ������̬
	eta = Tool::setEta(xOrigin, yOrigin, psiOrigin);
	etaFlt = Tool::setEta(xOrigin, yOrigin, psiOrigin);

	//��ʼ��������
	Tool::initForce6(thrust);

	//��ʼ����������
	Tool::initForce6(windForce);

	//��ʼ��һ�ס����ײ�����
	Tool::initForce6(wave1Force);
	Tool::initForce6(wave2Force);

	//��ʼ����������
	Tool::initForce6(curForce);

	//��ʼ��������λ�������ͣ�1.���涯����λ��2.�������Ŷ�����λ
	dpFlag = 1;
	//��ʼ��������λ���Ʒ�����1.PID���ƣ�2.������ģ��Ԥ�����
	ctlFlag = 1;
	//��ʼ���������Ŷ�����λ���Ʋ������ͣ�
	//1.WOPC��ZPC-W��Ϻ�Ļ������Ŷ�����λ���Ʋ���
	//2.WOPC���û������ƵĻ������Ŷ�����λ���Ʋ���
	//3.ZPC-W�������Ŷ�����λ���Ʋ���
	wopcFlag = 1;

}

//���ô�������
void ShipControl::setData(Data *data_)
{
	data = data_;
}

//���ļ�
void ShipControl::openFiles()
{
	paraFile.open("E:/projectProgram/data/parameters.txt");
	etaFile.open("E:/projectProgram/data/eta.txt");
	nuFile.open("E:/projectProgram/data/nu.txt");
	outFltFile.open("E:/projectProgram/data/etaFilter.txt");
	centerFile.open("E:/projectProgram/data/center.txt");
	optHeadFile.open("E:/projectProgram/data/optHead.txt");
	targetFile.open("E:/projectProgram/data/target.txt");
	wave1File.open("E:/projectProgram/data/wave1.txt");
	wave2File.open("E:/projectProgram/data/wave2.txt");
	windFile.open("E:/projectProgram/data/wind.txt");
	curFile.open("E:/projectProgram/data/current.txt");
	taoFile.open("E:/projectProgram/data/tao.txt");
	thrustFile.open("E:/projectProgram/data/thrust.txt");
}

//�ر��ļ�
void ShipControl::closeFiles()
{
	paraFile.close();
	etaFile.close();
	nuFile.close();
	outFltFile.close();
	centerFile.close();
	optHeadFile.close();
	targetFile.close();
	wave1File.close();
	wave2File.close();
	windFile.close();
	curFile.close();
	taoFile.close();
	thrustFile.close();
}

//�û��ӿ�
void ShipControl::userInterFace()
{
	cout << "����������λ���Ʒ������" << endl << endl << endl;

	cout << "�������û���������" << endl;

	cout << "\t����(Ĭ��Ϊ" << SpeedWind << "m/s) = ";

	gets_s(SpeedWindStr, 20);
	valueStr = strtod(SpeedWindStr, &SpeedWindStrEnd);
	SpeedWind = (SpeedWindStrEnd == SpeedWindStr) ? SpeedWind : valueStr;

	cout << "\t����(Ĭ��Ϊ" << DirWind << "��) = ";

	gets_s(DirWindStr, 20);
	valueStr = strtod(DirWindStr, &DirWindStrEnd);
	DirWind = (DirWindStrEnd == DirWindStr) ? DirWind : valueStr;

	cout << "\t�˸�(Ĭ��Ϊ" << HeightWave << "m) = ";

	gets_s(HeightWaveStr, 20);
	valueStr = strtod(HeightWaveStr, &HeightWaveStrEnd);
	HeightWave = (HeightWaveStrEnd == HeightWaveStr) ? HeightWave : valueStr;

	cout << "\t����(Ĭ��Ϊ" << DirWave << "��) = ";

	gets_s(DirWaveStr, 20);
	valueStr = strtod(DirWaveStr, &DirWaveStrEnd);
	DirWave = (DirWaveStrEnd == DirWaveStr) ? DirWave : valueStr;

	cout << "\t����(Ĭ��Ϊ" << SpeedCurrent << "m/s) = ";

	gets_s(SpeedCurrentStr, 20);
	valueStr = strtod(SpeedCurrentStr, &SpeedCurrentStrEnd);
	SpeedCurrent = (SpeedCurrentStrEnd == SpeedCurrentStr) ? SpeedCurrent : valueStr;

	cout << "\t����(Ĭ��Ϊ" << DirCurrent << "��) = ";

	gets_s(DirCurrentStr, 20);
	valueStr = strtod(DirCurrentStr, &DirCurrentStrEnd);
	DirCurrent = (DirCurrentStrEnd == DirCurrentStr) ? DirCurrent : valueStr;

	cout << "\n�������ô����ڶ�����λ�����еĳ�ʼλ��������Ŀ��λ������" << endl;

	cout << "\t��ʼ��λ��(Ĭ��Ϊ" << xOrigin << "m) = ";

	gets_s(xOriginStr, 20);
	valueStr = strtod(xOriginStr, &xOriginStrEnd);
	xOrigin = (xOriginStrEnd == xOriginStr) ? xOrigin : valueStr;

	cout << "\t��ʼ��λ��(Ĭ��Ϊ" << yOrigin << "m) = ";

	gets_s(yOriginStr, 20);
	valueStr = strtod(yOriginStr, &yOriginStrEnd);
	yOrigin = (yOriginStrEnd == yOriginStr) ? yOrigin : valueStr;

	cout << "\t��ʼ����(Ĭ��Ϊ" << psiOrigin << "��) = ";

	gets_s(PsiOriginStr, 20);
	valueStr = strtod(PsiOriginStr, &PsiOriginStrEnd);
	psiOrigin = (PsiOriginStrEnd == PsiOriginStr) ? psiOrigin : valueStr;

	cout << endl;

	cout << "\tĿ�걱λ��(Ĭ��Ϊ" << xTarget << "m) = ";

	gets_s(xTargStr, 20);
	valueStr = strtod(xTargStr, &xTargStrEnd);
	xTarget = (xTargStrEnd == xTargStr) ? xTarget : valueStr;

	cout << "\tĿ�궫λ��(Ĭ��Ϊ" << yTarget << "m) = ";

	gets_s(yTargStr, 20);
	valueStr = strtod(yTargStr, &yTargStrEnd);
	yTarget = (yTargStrEnd == yTargStr) ? yTarget : valueStr;

	cout << "\tĿ������(Ĭ��Ϊ" << psiTarget << "��) = ";

	gets_s(PsiTargStr, 20);
	valueStr = strtod(PsiTargStr, &PsiTargStrEnd);
	psiTarget = (PsiTargStrEnd == PsiTargStr) ? psiTarget : valueStr;

	cout << "\n����ѡ������λ��������(1.���涯����λ��2.�������Ŷ�����λ)��" << endl;

	do 
	{
		cout << "\tĬ����������Ϊ 1.���涯����λ��\n\t�������������ʹ��룺 ";

		cin.get(dpFlagStr, 20);
		cin.get();

		dpFlag = ( 0 != atoi(dpFlagStr))? atoi(dpFlagStr) : 1;

		if (1 != dpFlag && 2 != dpFlag)
		{
			cout << "\t�������������ȷ�Ϻ��ٴ����룡" << endl;
		}
	} while (1 != dpFlag && 2 != dpFlag);

	cout << "\n����ѡ������λ����������(1.PID��������2.������ģ��Ԥ�������)��" << endl;

	do 
	{
		cout << "\tĬ�Ͽ���������Ϊ 1.PID��������\n\t��������������ʹ��룺 ";

		cin.get(ctlFlagStr, 20);
		cin.get();

		ctlFlag = ( 0 != atoi(ctlFlagStr))? atoi(ctlFlagStr) : 1;

		if (1 != ctlFlag && 2 != ctlFlag)
		{
			cout << "\t�������������ȷ�Ϻ��ٴ����룡" << endl;
		}
	} while (1 != ctlFlag && 2 != ctlFlag);

	//����DP����������
	switch (ctlFlag)
	{
	case 1:
		cout << "\n��������PID�������Ĳ���" << endl;

		cout << "\tKp(Ĭ��Ϊ" << kp << ") = ";

		gets_s(kpStr, 20);
		valueStr = strtod(kpStr, &kpStrEnd);
		kp = (kpStrEnd == kpStr) ? kp : valueStr;

		cout << "\n\tKi(Ĭ��Ϊ" << ki << ") = ";

		gets_s(kiStr, 20);
		valueStr = strtod(kiStr, &kiStrEnd);
		ki = (kiStrEnd == kiStr) ? ki : valueStr;

		cout << "n\tKd(Ĭ��Ϊ" << kd << ") = ";

		gets_s(kdStr, 20);
		valueStr = strtod(kdStr, &kdStrEnd);
		kd = (kdStrEnd == kdStr) ? kd : valueStr;

		break;
	case 2:
		cout << "\n��������NMPC�������Ĳ���" << endl;

		cout << "\tԤ������T(Ĭ��Ϊ" << Tpre << ") = ";

		gets_s(tStr, 20);
		valueStr = strtod(tStr, &tStrEnd);
		Tpre = (tStrEnd == tStr) ? Tpre : valueStr;

		cout << "\tȨֵw1(Ĭ��Ϊ" << w1 << ") = ";

		gets_s(w1Str, 20);
		valueStr = strtod(w1Str, &w1StrEnd);
		w1 = (w1StrEnd == w1Str) ? w1 : valueStr;

		cout << "\tȨֵw2(Ĭ��Ϊ" << w2 << ") = ";

		gets_s(w2Str, 20);
		valueStr = strtod(w2Str, &w2StrEnd);
		w2 = (w2StrEnd == w2Str) ? w2 : valueStr;

		cout << "\tȨֵw3(Ĭ��Ϊ" << w3 << ") = ";

		gets_s(w3Str, 20);
		valueStr = strtod(w3Str, &w3StrEnd);
		w3 = (w3StrEnd == w3Str) ? w3 : valueStr;

		break;
	default:
		break;
	}

	//���û�������������ƵĲ���
	if (2 == dpFlag)
	{
	cout << "\n����ѡ�񻷾����Ŷ�����λ���Ʋ���\n"
		<< "\t(1.WOPC��ZPC-W��Ϻ�Ļ������Ŷ�����λ���Ʋ���;\n"
		<< "\t2.WOPC���û������ƵĻ������Ŷ�����λ���Ʋ���;\n"
		<< "\t3.ZPC-W�������Ŷ�����λ���Ʋ���)��" << endl;

	do 
	{
		cout << "\tĬ�Ͽ��Ʋ���Ϊ 1.WOPC��ZPC-W��Ϻ�Ļ������Ŷ�����λ���Ʋ��ԡ�\n"
			<< "\t��������Ʋ��Դ��룺 ";

		cin.get(wopcFlagStr, 20);
		cin.get();

		wopcFlag = ( 0 != atoi(wopcFlagStr))? atoi(wopcFlagStr) : 1;

		if (1 != wopcFlag && 2 != wopcFlag && 3 != wopcFlag)
		{
			cout << "\t�������������ȷ�Ϻ��ٴ����룡" << endl;
		}
	} while (1 != wopcFlag && 2 != wopcFlag && 3 != wopcFlag);

		switch (wopcFlag)
		{
		case 1:
			break;
		case 2:
			break;
		case 3:
			cout << "\n��������ZPC-W������������������Ĳ���" << endl;

			cout << "\tKpZ(Ĭ��Ϊ" << kpZ << ") = ";

			gets_s(kpZStr, 20);
			valueStr = strtod(kpZStr, &kpZStrEnd);
			kpZ = (kpZStrEnd == kpZStr) ? kpZ : valueStr;

			cout << "\tKiZ(Ĭ��Ϊ" << kiZ << ") = ";

			gets_s(kiZStr, 20);
			valueStr = strtod(kiZStr, &kiZStrEnd);
			kiZ = (kiZStrEnd == kiZStr) ? kiZ : valueStr;

			cout << "\tKdZ(Ĭ��Ϊ" << kdZ << ") = ";

			gets_s(kdZStr, 20);
			valueStr = strtod(kdZStr, &kdZStrEnd);
			kdZ = (kdZStrEnd == kdZStr) ? kdZ : valueStr;

			break;
		default:
			break;
		}
	}


	cout << "\n���ڽ��м��㣬���Ե�..." << endl;

	paraFile << "��������������λ�����и�������Ϣ����\n";
	paraFile << "������ʼλ��������Ϊ�� (" << xOrigin << "m, " << yOrigin << "m, " << psiOrigin << "��).\n";
	paraFile << "����Ŀ��λ��������Ϊ�� (" << xTarget << "m, " << yTarget << "m, " << psiTarget << "��).\n";

	paraFile << "�ﻷ��������\n";
	paraFile << "����Ϊ�� " << SpeedWind << "m/s, " << "����Ϊ�� " << DirWind << "��.\n";
	paraFile << "�˸�Ϊ�� " << HeightWave << "m, " << "����Ϊ�� " << DirWave << "��.\n";
	paraFile << "����Ϊ�� " << SpeedCurrent << "m/s, " << "����Ϊ�� " << DirCurrent << "��.\n";

	paraFile << "�ﶯ����λ�������͡�\n";
	switch (dpFlag)
	{
	case 1:
		paraFile << "���涯����λ\n";
		break;
	case 2:
		paraFile << "�������Ŷ�����λ\n";
		break;
	default:
		break;
	}

	paraFile << "�ﶯ����λ���������͡�" << endl;
	switch (dpFlag)
	{
	case 1:
		paraFile << "\tPID������" << endl << endl;
		paraFile << "��PID������������\n";
		paraFile << "\tKp = " << kp << endl;
		paraFile << "\tKi = " << ki << endl;
		paraFile << "\tKd = " << kd << endl;
		paraFile << endl;
		break;
	case 2:
		paraFile << "\t������ģ��Ԥ�������" << endl << endl;
		paraFile << "�������ģ��Ԥ�������������\n";
		paraFile << "\tT = " << Tpre << endl;
		paraFile << "\tw1 = " << w1 << endl;
		paraFile << "\tw2 = " << w2 << endl;
		paraFile << "\tw3 = " << w3 << endl;
		paraFile << endl;
		break;
	default:
		break;
	}
	
	if (2 == dpFlag)
	{
		switch (wopcFlag)
		{
		case 1:
			paraFile << "��ZPC-W������������������Ĳ�����" << endl;
			paraFile << "\tKpZ = " << kpZ << endl;
			paraFile << "\tKiZ = " << kiZ << endl;
			paraFile << "\tKdZ = " << kdZ << endl;
			paraFile << endl;
			break;
		case 2:
			break;
		case 3:
			break;
		default:
			break;
		}

	}
	targetFile << xTarget << "\t" << yTarget << endl;

	setParameter();
}

//���ò���
void ShipControl::setParameter()
{

	//��ʼ��������������������Ĳ���
	optPsiCtrl.setStep(tStep*ctrlCyc);
	optPsiCtrl.setPID(kpZ, kiZ, kdZ);

	//��ʼ�������۲���
	envObs.setStep(tStep);
	envObs.setK(k1Env, k2Env, k3Env);

	//���ô���ģ�͵Ĳ���
	model.setStep(tStep);
	model.setInitEta(eta);

	//��ʼ��PID
	pid.initPID(kp, ki, kd);

	//����NMPC�е�������Ȩֵ
	nmpc.setT(Tpre);
	nmpc.setWeight(w1, w2, w3);
	nmpc.calM();


	filter.setStep(tStep);

	//��ʼ��������Ŀ��λ������̬
	Tool::initEtaTarget(etaTarget, xTarget, yTarget, psiTarget*angToRad);

	wopc.setStep(tStep*ctrlCyc);
	//���û������Ŷ�����λ�İ뾶
	wopc.setRadius(radius);

	//���û������Ŷ�����λ��Ŀ��λ��
	wopc.setPos(etaTarget);
}

//������������
void ShipControl::run()
{	
	//���ô����Ĳ���
	model.setData(data);
	model.calM();

	//��ʼ�����˵���Ϣ
	wave.setData(data);
	wave.setPara(HeightWave, DirWave);
	wave.calWave();

	//ѭ������ģ�ͼ���
	for (time = 0.0; time <= maxTime; time += tStep)
	{		
		//�õ�����������
		envObs.setNu(nu);
		envObs.setTao(thrust);
		envObs.cal();		
		envEst = envObs.force();

		//���û������Ŷ�����λ�ĵ�ǰλ���������ٶȽ��ٶ�
		wopc.setEta(eta);
		wopc.setNu(nu);

		//��ʵʱ������Ϣ
		wind.setHead(eta.psi);
		wind.setNu(nu);
		//������������
		wind.cal();

		windForce = wind.getWindTao();
		Tool::Force6ToArray(windForce, windArray);

		//����������
		wave.cal(eta, time);
		wave.getLoad(wave1Force, wave2Force);

		Tool::Force6ToArray(wave1Force, wave1Array);
		Tool::Force6ToArray(wave2Force, wave2Array);

		//��������
		cur.setPsi(eta.psi);
		cur.cal();
		curForce = cur.force();
		Tool::Force6ToArray(curForce, curArray);

		if (0 == ctrlCount)
		{
			//������λ����
			switch (ctlFlag)
			{
				//PID����
			case 1:
				pid.setTarget(etaTarget);
				pid.setEta(eta);
				pid.calculat();
				thrust = pid.getTao();
				break;
				//NMPC����
			case 2:
				nmpc.setTarget(etaTarget);
				nmpc.setEnv(envEst);
				nmpc.setEta(etaFlt);
				nmpc.setNu(nu);
				nmpc.cal();
				thrust = nmpc.Force();
				break;
			default:
				break;
			}

			//���������������
			if (2 == dpFlag)
			{
				switch (wopcFlag)
				{
				case 1:
					//wopc
					wopc.setThrust(thrust);
					wopc.calculat();
					etaTarget.n = wopc.getRTPosDes().first;
					etaTarget.e = wopc.getRTPosDes().second;
					etaTarget.psi = wopc.getPsiRTDes();
					optPsi = wopc.getPsiRTDes();
					break;
				case 2:
					break;
				case 3:
					//zpc-w
					optPsiCtrl.setPsi(optPsi);
					optPsiCtrl.setTao(thrust);
					optPsiCtrl.cal();
					optPsi = optPsiCtrl.OptPsi();
					break;
				default:
					break;
				}
			}

		}
		ctrlCount ++;
		if (ctrlCyc <= ctrlCount)
		{
			ctrlCount = 0;
		}

		//�������Ŷ�����λ��������
		if (2 == dpFlag)
		{
			optHeadFile << time << "\t" << optPsi << endl;
			if (1 == wopcFlag || 2 == wopcFlag)
			{
				centerFile << time << "\t" << etaTarget.n
					<< "\t" << etaTarget.e << endl;
			}
		}

		//�����ṹ��ת��Ϊ����
		Tool::Force6ToArray(thrust, thrustArray);

		//���ļ��зֱ��¼������
		taoFile << time << "\t";
		thrustFile << time << "\t";
		windFile << time << "\t";
		wave1File << time << "\t";
		wave2File << time << "\t";
		curFile << time << "\t";
		for (int i = 0; i < DOF6; ++ i)
		{
			//�������
			taoArray[i] = thrustArray[i] + wave1Array[i] + wave2Array[i] + windArray[i] + curArray[i];

			taoFile << taoArray[i] << "\t";
			thrustFile << thrustArray[i] << "\t";
			windFile << windArray[i] << "\t";
			wave1File << wave1Array[i] << "\t";
			wave2File << wave2Array[i] << "\t";
			curFile << curArray[i] << "\t";
		}
		taoFile << endl;
		thrustFile << endl;
		windFile << endl;
		wave1File << endl;
		wave2File << endl;
		curFile << endl;

		Tool::ArrayToForce6(taoArray, tao);

		//���������õ�����ģ����
		model.setForce(tao);

		//���㴬��ģ��
		model.cal();

		//�����õ�������λ����̬�Լ��ٶȽ��ٶ�
		eta = model.getEta();
		nu = model.getNu();		

		//�˲�
		filter.setTao(thrust);
		filter.setEta(eta);
		etaFlt = filter.cal();

		etaFile << time << "\t" << eta << endl;

		nuFile << time << "\t" << nu.u << "\t" << nu.v << "\t" << nu.w 
			<< "\t" << nu.p << "\t" << nu.q << "\t" << nu.r << endl;

		outFltFile << time << "\t" << etaFlt << endl;
	}

}

//����<<������
ostream& operator << (ostream &os, const Eta &eta)
{
	os << eta.n << "\t" << eta.e << "\t" << eta.d << "\t"
		<< eta.phi << "\t" << eta.theta << "\t" << eta.psi;
	return os;
}

