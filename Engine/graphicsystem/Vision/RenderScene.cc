﻿/****************************************************************************
Copyright (c) 2011-2013,WebJet Business Division,CYOU

http://www.genesis-3d.com.cn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "stdneb.h"
#include "vis/visentity.h"
#include "vis/visserver.h"
#include "vis/vissystems/visquadtree.h"
#include "graphicsystem/Renderable/RenderObject.h"
#include "RenderScene.h"
#include "ViewPortWindow.h"
#include "GraphicSystem.h"
#include "Camera/Camera.h"

namespace Graphic
{

	static const int QuadTreeDepth = 7;
	extern const Math::float4 AmbientColorOfDefaultLight(0.198f, 0.198f, 0.198f, 1.0f);	// 50 50 50 255

	struct LightsSort : public std::binary_function<const Light*, const Light*, std::size_t>
	{
		bool operator() (const Light* lhs, const Light* rhs) const
		{
			if (lhs->GetLightType() > Light::eDirectionalLight && rhs->GetLightType() > Light::eDirectionalLight)//会衰减的光源还是按原来的顺序排。相对位置不做调整。
			{
				return false;
			}
			else
			{
				return lhs->GetLightType() < rhs->GetLightType();
			}
		}
	};

	struct CameraSort : public std::binary_function<const GPtr<Camera>&, const GPtr<Camera>&, std::size_t>
	{
		bool operator() (const GPtr<Camera>& lhs, const GPtr<Camera>& rhs) const
		{
			//ViewPortWindow* lhvp = (NULL == lhs->GetTargetWindow()) ? default_window : lhs->GetTargetWindow();
			//ViewPortWindow* rhvp = (NULL == rhs->GetTargetWindow()) ? default_window : rhs->GetTargetWindow();

			//if(default_camera && lhs == default_camera)
			//	return false;

			//if(default_camera && rhs == default_camera)
			//	return true;

			//if (lhvp != rhvp)
			//{
			//	return lhvp->GetType() < rhvp->GetType();
			//}
			//return lhs->GetCameraOrder() < rhs->GetCameraOrder();

			const Camera* lc = lhs.get_unsafe();
			const Camera* rc = rhs.get_unsafe();
			if (lc->GetRenderSort() != rc->GetRenderSort())
			{
				return lc->GetRenderSort() < rc->GetRenderSort();
			}
			
			if ( NULL != lc->GetRenderTargetSuite() || rc->GetRenderTargetSuite() != NULL )
			{
				return (uintptr_t)lc->GetRenderTargetSuite() < (uintptr_t)rc->GetRenderTargetSuite();
			}

			ViewPortWindow* lhvp = (NULL == lhs->GetTargetWindow()) ? default_window : lhs->GetTargetWindow();
			ViewPortWindow* rhvp = (NULL == rhs->GetTargetWindow()) ? default_window : rhs->GetTargetWindow();
			if (lhvp != rhvp)
			{
				return lhvp->GetType() < rhvp->GetType();
			}
			return lc->GetCameraOrder() < rc->GetCameraOrder();
		}
		ViewPortWindow* default_window;
		//GPtr<Camera>    default_camera;
		CameraSort(ViewPortWindow* _default)//,GPtr<Camera> _camera)
		{
			default_window = _default;
			//default_camera = _camera;
		}
	};


	RenderScene::Environment::Environment()
		:fogColor(0.5f,0.5f,0.5f,0.0f)
		,fogParam(0.0f,2000.f,0.0f,-1.0f)
		,ambientColor(AmbientColorOfDefaultLight)	
		,softShadowParam(0.3f,1.0f,0.f,0.f)
		,shadowStrength(0.f,0.f,0.f,1.f)

	{

	}


	RenderScene::RenderScene()
		:mEnvironment(NULL)
		,mDefaultCamera(NULL)
		,mRenderSort(RenderScene::SortScene)
	{

	}

	RenderScene::~RenderScene()
	{

	}

	void RenderScene::SetRenderSort(uint sort)
	{
		mRenderSort = sort;
		if(GraphicSystem::HasInstance())
		{
			GraphicSystem::Instance()->SortRenderScene();
		}
	}

	void RenderScene::Setup()
	{
		n_assert(!mVisServer.isvalid());
		mVisServer = Vis::VisServer::Create();
		GPtr<Vis::VisQuadtree> pQuadTree = Vis::VisQuadtree::Create();
		// @Todo 应该允许外部设置
		pQuadTree->SetQuadTreeSettings( QuadTreeDepth, Math::bbox( Math::point(0,0,0), Math::vector(1000,1000,1000 )) );
		mVisServer->AttachVisSystem( pQuadTree.upcast<Vis::VisSystemBase>() );
		mVisServer->Open();

		GraphicSystem::Instance()->AddRenderScene(this);
	}

	void RenderScene::ApplyEnvironment() const
	{
		Graphic::GlobalMaterialParam* globalMaterialParam = Graphic::Material::GetGlobalMaterialParams();
		if (mEnvironment && globalMaterialParam)
		{
			if (globalMaterialParam)
			{
				globalMaterialParam->SetVectorParam(Graphic::eGShaderVecAmbientColor, mEnvironment->ambientColor);
				globalMaterialParam->SetVectorParam(Graphic::eGShaderVecFogColor, mEnvironment->fogColor);
				globalMaterialParam->SetVectorParam(Graphic::eGShaderVecFogParam, mEnvironment->fogParam);
				globalMaterialParam->SetVectorParam(Graphic::eGShaderVecSoftShadowParam, mEnvironment->softShadowParam);
				//globalMaterialParam->SetVectorParam(Graphic::eGShaderVecShadowColor, mEnvironment->shadowStrength);
			}
		}
		else
		{
			if (globalMaterialParam)
			{
				globalMaterialParam->SetVectorParam(Graphic::eGShaderVecAmbientColor, Math::float4(0.0f, 0.0f, 0.0f, 0.0f));
				globalMaterialParam->SetVectorParam(Graphic::eGShaderVecFogColor, Math::float4(0.0f, 0.0f, 0.0f, 0.0f));
				globalMaterialParam->SetVectorParam(Graphic::eGShaderVecFogParam, Math::float4(0.0f, 0.0f, 0.0f, 0.0f));
				globalMaterialParam->SetVectorParam(Graphic::eGShaderVecSoftShadowParam, Math::float4(0.0f, 0.0f, 0.0f, 0.0f));
			}
		}

	}


	void RenderScene::_AddLight(Light* light)
	{
		n_assert(light);
		//[zhongdaohuan]光照类型排序，应该是先eSunLight，然后eDirectionalLight，然后其他会衰减的光源

		Light::LightType light_type = light->GetLightType();
		int count = mLights.Size();
		if (Light::eSunLight == light_type)
		{
			if (count > 0 && Light::eSunLight == mLights[0]->GetLightType())//eSunLight应该是唯一的。
			{
				mLights[0] = light;
			}
			else if (count > 0 && Light::eSunLight != mLights[0]->GetLightType())
			{
				mLights.Insert(0, light);
			}
			else
			{
				mLights.Append(light);
			}
		}
		else if (Light::eDirectionalLight == light_type)
		{
			if (count > 0) 
			{
				size_t insertPos = mLights.Size() - 1;

				for ( ;insertPos >= 0; --insertPos)
				{
					Light::LightType lightType = mLights[insertPos]->GetLightType();
					if ( lightType <= Light::eDirectionalLight )
					{
						break;
					}
				}

				mLights.Insert(insertPos+1,light);
			}
			else
			{
				mLights.Append(light);
			}
		}
		else	//会衰减的光源，先到先占位，不作排序
		{
			mLights.Append(light);
		}
	}
	void RenderScene::_RemoveLight(Light* light)
	{
		Lights::Iterator it = mLights.Find(light);
		if (NULL != it)
		{
			mLights.Erase(it);
		}
	}

	void RenderScene::_SortLights()
	{
		Util::CustomSortArray<Light*, LightsSort>(mLights);
	}

	Light* RenderScene::GetSunLight() const
	{
		if (mLights.Size())
		{
			if (Light::eSunLight == mLights[0]->GetLightType())
			{
				return mLights[0];
			}
		}
		return NULL;
	}

	void RenderScene::Destroy()
	{
		mCameraList.Clear();
		mDefaultCamera = NULL;

		if (mVisServer)
		{
			mVisServer->Close();
		}

		GraphicSystem::Instance()->RemoveRenderScene(this);
	}





	//------------------------------------------------------------------------
	void RenderScene::_AddRenderObject(RenderObject* obj)
	{
		n_assert(NULL != obj);
		n_assert(!obj->_GetVisEnt().isvalid());
		GPtr<Vis::VisEntity> pVisEnt = Vis::VisEntity::Create();
		obj->_SetVisEnt(pVisEnt, this);
		mVisServer->RegisterVisEntity(pVisEnt, 0);
	}

	void RenderScene::_RemoveRenderObject(RenderObject* obj)
	{
		n_assert(NULL != obj);
		n_assert(obj->_GetVisEnt().isvalid());
		mVisServer->UnregisterVisEntity( obj->_GetVisEnt(), 0 );
		obj->_SetVisEnt( NULL , NULL);	
	}

	//------------------------------------------------------------------------
	void RenderScene::_AddRenderObjectNotCull(RenderObject* obj)
	{
		n_assert(obj);
		n_assert(!obj->_GetVisEnt().isvalid());
		mNotCullRenderObjects.Append(obj);	
	}
	//------------------------------------------------------------------------
	void RenderScene::_RemoveRenderObjectNotCull(RenderObject* obj)
	{
		n_assert(obj);
		n_assert(!obj->_GetVisEnt().isvalid());
		IndexT i = mNotCullRenderObjects.FindIndex(obj); 
		if( InvalidIndex != i )
		{
			mNotCullRenderObjects.EraseIndex(i);
		}
	}

	//------------------------------------------------------------------------
	GPtr<Vis::VisQuery> RenderScene::Cull(const Camera& camera)
	{
		return Cull(camera.GetViewProjTransform(), camera.GetTransform().get_position());
	}

	//------------------------------------------------------------------------
	GPtr<Vis::VisQuery> RenderScene::Cull(const Math::matrix44& viewProj, const Math::float4& pos)
	{
		n_assert( mVisServer.isvalid() );
		GPtr<Vis::ObserverContext> observer = Vis::ObserverContext::Create();
		observer->Setup(viewProj, pos);
		return mVisServer->PerformVisQuery(observer, Util::Array<IndexT>() );
	}

	//------------------------------------------------------------------------
	// internal call
	void RenderScene::_UpdateVisEntity(const GPtr<Vis::VisEntity>& visEnt )
	{

		n_assert( visEnt.isvalid() );
		n_assert( mVisServer.isvalid() );
		mVisServer->UpdateVisEntity( visEnt );
	}
	//------------------------------------------------------------------------
	const bool RenderScene::HasLight() const
	{
		return !mLights.IsEmpty();
	}

	//------------------------------------------------------------------------
	void RenderScene::_AddCamera(Camera* camera)
	{
		n_assert(camera);
		if (InvalidIndex == mCameraList.FindIndex(camera))
		{
			mCameraList.Append(camera);
			_SortCameraList();
		}
	}

	//------------------------------------------------------------------------
	void RenderScene::_RemoveCamera(const Camera* camera)
	{
		for (int i = 0; i < mCameraList.Size(); ++i)
		{
			if (camera == mCameraList[i])
			{
				mCameraList.EraseIndex(i);
				return;
			}
		}
	}

	//------------------------------------------------------------------------
	void RenderScene::_SortCameraList()
	{
		CameraSort order(GraphicSystem::Instance()->GetMainViewPortWindow());//,mDefaultCamera);
		Util::CustomSortArray<GPtr<Camera>, CameraSort>(mCameraList, order);
	}

	//------------------------------------------------------------------------
	void  RenderScene::SetDefaultCamera(Camera* camera)
	{
		
		for (CameraList::Iterator it = mCameraList.Begin(); it != mCameraList.End(); ++it)
		{
			if (camera  == (*it))
			{
				mDefaultCamera = camera;
				_SortCameraList();
				return;
			}
		}
	}
}
