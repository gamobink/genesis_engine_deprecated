/** \file defaultinputprocss.h
* \brief �ļ�����˵��
* Copyright (c) 2011,��������WebJet��ҵ��
* All rights reserved.
* Date        Ver    Who          Comment
* 2011:9:16   1.0    ���˲�   
*/
#ifndef __gyrostinputprocss_H__
#define __gyrostinputprocss_H__

#include "app/appframework/manager.h"
#include "app/apputil/mayacamerautil.h"


namespace App
{
	/*
	ϵͳ�ṩ�Ķ��û������Ĭ�ϴ�������������Ϣ�����������
	�����Ҫ�ô�������GyrostProcessInput(Manager�����뵽InputFeature�С����򣬿���ʹ���Լ������봦����
	*/
	class GyrostProcessInput: public App::Manager
	{
		__DeclareSubClass(GyrostProcessInput, App::Manager );
	public:
		GyrostProcessInput();
		~GyrostProcessInput();

		/// @App::Manager::OnActivate called when attached to game server
		virtual void OnActivate();

		/// @App::Manager::OnBeginFrame called before frame by the game server
		virtual void OnBeginFrame();
	public:
		AppUtil::MayaCameraUtil m_mayaCameraUtil;
	};



}







#endif // __gyrostinputprocss_H__
