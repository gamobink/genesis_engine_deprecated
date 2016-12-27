/** \file: GameBuilding.h
* \brief: building logic
* Copyright (c) 2011,WebJet Enterprise Department.ChangYou
* All rights reserved.
* Date        Ver    Who          Comment
* 2011/12/07   1.0    SunHao   
*/

#ifndef __GAMEBUILDING_H__
#define __GAMEBUILDING_H__

#include "core/refcounted.h"
#include "appframework/app_fwd_decl.h"
#include "appframework/actor.h"
#include "DemoFwdDecl.h"

namespace App
{
	class RenderComponent;
}

namespace Demo
{
	class Estates : public Core::RefCounted
	{
		__DeclareSubClass( Demo::Estates, Core::RefCounted )
	public:
		Estates()
			: m_pBuilding()
			, m_iType( 0 )
			, m_RollRadian( 0 )
			, m_Pos()
		{}

		~Estates();

		void Init( App::TActorPtr pBuilding, TEstateType iType, const Math::float3& pos, Math::scalar radian );
		void Rotate( Math::scalar radian );
		
		// - set method
		void SetRadian( Math::scalar radian )	{ m_RollRadian =radian; }

		// - get method
		App::Actor::FastId GetId( void )		{ return m_pBuilding->GetFastId();}
		App::TActorPtr GetBuilding( void )		{ return m_pBuilding; }
		Math::scalar GetRadian( void )			{ return m_RollRadian; }
		TEstateType GetType( void )				{ return m_iType; }
		Math::float3 GetPos( void )				{ return m_Pos; }
	private:
		App::TActorPtr m_pBuilding;
		TEstateType m_iType;
		Math::scalar m_RollRadian;
		Math::float3 m_Pos;
	};

	class LandAgent
	{
	public:
		LandAgent()
			: m_Buildings()
		{}
		~LandAgent()
		{}

		TEstatesPtr Find( App::Actor::FastId iFastId );
		TEstatesPtr Find( IndexT iIndex );

		bool Add( TEstatesPtr pBuilding );
		bool Add( App::TActorPtr pBuilding, TEstateType iType, const Math::float3& pos, Math::scalar radian  );
		void Remove( App::Actor::FastId iFastId );
		
		SizeT Size( void );
		void Clear( void );
	private:

		Util::Dictionary<App::Actor::FastId, TEstatesPtr> m_Buildings;
	};

	class BuildingAnimationManager: public Core::RefCounted
	{
		__DeclareClass(BuildingAnimationManager )
	public:
		BuildingAnimationManager();
		virtual ~BuildingAnimationManager();

		/// ��ʼ�������ø������ò���
		void Open();

		/// �ر�
		void Close();
		
		/// ����һ��building����Ӹ�building�л�ȡ���Խ��н������ɶ�����Actor
		void BeignAnimtion( App::TActorPtr pBlock );

		/// ������ʱ�䡣���ж����仯
		void OnFrame( float32 fTime );

	protected:

		float32 GetMaxHouseHeight( const Util::String& housename , float32 offSet ) const; 

		void AddAnimationInfo( const App::TActorPtr& actor, float32 offSet );

	protected:
		struct AnimInfo
		{
			App::TActorPtr m_house;
			Ptr<App::RenderComponent> m_render;
			float32 m_curHeight;	//	�����ĵ�ǰ�߶�
			float32 m_maxHeight;	//	���������߶�
		};
		typedef Util::Array<AnimInfo> AnimInfos;

		AnimInfos m_AnimInfos;	//	��ǰ�Ķ�����Ϣ

		Util::Dictionary<Util::String, float> m_heightLimit;	//	��¼�����ĸ߶�����

		Util::String m_animationShader;
		Util::StringAtom m_animationShaderParamName;

	};
}

#endif // __GAMEBUILDING_H__