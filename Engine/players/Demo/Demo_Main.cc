/** \file demo.cc
* \brief: make demo for the boss
* Copyright (c) 2011,��������WebJet��ҵ��
* All rights reserved.
* Date        Ver    Who          Comment
* 2011/10/13   1.0    SunHao   
*/


#include "stdneb.h"

#include "system/appentry.h"
#include "Demo_GameApplication.h"

ImplementNebulaApplication()

/**
Demo ��Ҫ�����ڷ���Ĵ�ĳ�������� �����в�������
ProjectPath  ָ��Ҫ�򿪵�·��  ������
Scene        ָ��Ҫ�򿪵ĳ���  ���û�У�Ĭ�ϴ���һ���ճ���

ʵ����
��show���̵�show.scene. ����������в������� 
ProjectPath="E:\WebJet_CodeReview\trunk\test_project\ProjRes\show" Scene="asset:scene/show.scene"
E:\WebJet_CodeReview\trunk\test_project\ �滻�ɱ��صĹ���·��
*/

//------------------------------------------------------------------------------
/**
*/
void
NebulaMain(const Util::CommandLineArgs& args)
{
	Demo::DemoGameApplication app;
	app.SetCompanyName("Changyou");
	app.SetAppTitle("Demo");
	app.SetCmdLineArgs(args);

	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
}
