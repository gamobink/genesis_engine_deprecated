/** \file simplegamestatehandler.h
* \brief �ļ�����˵��
* Copyright (c) 2011,��������WebJet��ҵ��
* All rights reserved.
* Date        Ver    Author          Comment
* 2011:11:16   1.0    libin   
*/
#ifndef __simplegamestatehandler_H__
#define __simplegamestatehandler_H__

#include "appframework/statehandler.h"
#include "appframework/actor.h"
#include "math/ray.h"
#include "math/color.h"
#include "gyrostat.h"
#include "particles/particlesystem.h"

#include "guifeature/gui.h"

namespace Sample
{
	using namespace Particles;

	class SimpleStateHandler : public App::StateHandler
	{
		__DeclareClass(SimpleStateHandler);
	public:    

		/// constructor
		SimpleStateHandler();
		/// destructor
		virtual ~SimpleStateHandler();

		//static Func for Web 
		static void ChangeParticleUp();
		static void ChangeParticleDown();

		/// called when the state represented by this state handler is entered
		virtual void OnStateEnter(const Util::String& prevState);
		/// called when the state represented by this state handler is left
		virtual void OnStateLeave(const Util::String& nextState);
		/// called each frame as long as state is current, return new state
		virtual Util::String OnFrame();

		/// set level filename, required by setup mode LoadLevel
		void SetSceneName(const Util::String& n);
		/// get level name
		const Util::String& GetSceneName() const;

		//ʾ������ϵ
		void UpdateWorldCoord(void);

		// ʾ������ν���ʰȡ����
		void IntersectActor();

		// ʾ�������»����ߵ�����
		void UpdateLine(void);

		//������������
		void UpdateGyrostat();

		//check circle actor
		bool IsCircleSelected(const Ptr<App::Actor>& actor );


		// ʾ��������ʿ��
		void UpdateSoldier(void);

		// ʾ�����ҽӲ���
		void ProcessAttach();

		// ���Ƴ�ʰȡ����actor�İ�Χ��
		void CreateOrUpdateBoundActor(const Ptr<App::Actor>& actor );

		void CreateParticleFromTemplate();

		void CreateScene();

		//
		void SetKeyDown(int key);

		void ChangeEmitter();

		//
		void createParSys(Util::Array<Util::StringAtom>& outTexture);

		void LoadTemplate();

		void UpdateParticle();

		void InitParticleTexutre(Util::Array<Util::StringAtom>& outTexture);


	private:    
		Util::String mSceneName;

		App::Actor::FastId mMeshActorID;
		Util::String mCurMeshName;
		Util::String mCurShaderName;
		Util::String mCurTexName;

		bool mShowAll;	//	��ʾ����İ�Χ�У����ǰ������ӽڵ�İ�Χ��
		bool mShowParticle;

		Ptr<App::Scene> mScene;

		Ptr<App::Actor> mCamera;
		Ptr<App::Actor> mLight;
		Ptr<App::Actor> mLineActor;		//	������ʾ��
		Ptr<App::Actor> mSoldierActor;	//	���ƴ��
		Ptr<App::Actor> mSoldierActorChild;

		Ptr<App::Actor> mSelectActor;	//	��ǰѡ�е�actor
		Ptr<App::Actor> mIntersectBoundActor;	//	������ʾ��Χ�е�actor

		Ptr<App::Actor> mWorldCoordsActor;	//	�ο�����
		//mGyrostatActor //need attach a circleActors , acoordsActor, three taper,


		Ptr<App::Actor> mSelectOpActor;	//	��ǰѡ�е�������actor
		Gyrostat* mGyrostat;

		float2	mMouseScreenPos;//��һ�����λ��

		Ptr<App::Actor> ParticleAct;
		Util::Array<Util::StringAtom> outlist;
		Ptr<ParticleSystem> ParSystem;


		// ===================== UI test ======================
		void _initUI();
		bool _frameUI();
		MyGUI::VectorWidgetPtr _addLayout(char* name);
		void _emptyWindowClick(MyGUI::Widget* _sender);
		void _radioClick(MyGUI::Widget* _sender);
		void _checkClick(MyGUI::Widget* _sender);
		void _nextPtc(MyGUI::Widget* _sender);
		void _prevPtc(MyGUI::Widget* _sender);
		void _clearPtc(MyGUI::Widget* _sender);

		void _requestCreateWidgetItem(MyGUI::ItemBox* _sender, MyGUI::Widget* _item);
		void _notifyScrollChangePosition(MyGUI::ScrollBar* _sender, size_t _position);

		MyGUI::VectorWidgetPtr mEmptyWin;
		MyGUI::Button* mEmtyWinBtn;
		MyGUI::EditBox* mPressTxt;
		MyGUI::ScrollBar* mRed;
		MyGUI::ScrollBar* mGreen;
		MyGUI::ScrollBar* mBlue;
		MyGUI::Widget* mColor;
		MyGUI::EditBox* mColorText;

		Util::Array<MyGUI::Button*> mRadios;
	};

	//------------------------------------------------------------------------------
	/**
	*/
	inline void
	SimpleStateHandler::SetSceneName(const Util::String& n)
	{
		this->mSceneName = n;
	}

	//------------------------------------------------------------------------------
	/**
	*/
	inline const Util::String&
	SimpleStateHandler::GetSceneName() const
	{

		return this->mSceneName;
	}
}



#endif // __simplegamestatehandler_H__



