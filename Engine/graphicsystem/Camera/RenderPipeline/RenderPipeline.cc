/****************************************************************************
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
#include "profilesystem/ProfileSystem.h"
#include "RenderPipeline.h"
#include "GraphicSystem.h"
#include "graphicsystem/Renderable/RenderObject.h"
#include "graphicsystem/Camera/Camera.h"
#include "graphicsystem/Renderable/GraphicRenderer.h"

namespace Graphic
{

	const int RenderPipeline::s_lightSupport = MAX_LIGHT_SUPPORT;
	static const Math::float4 g_LightNone(0.0f, 0.0f, 0.0f, -1.0f);

	__ImplementClass(RenderPipeline,'REPI',Core::RefCounted)

		RenderPipeline::RenderPipeline()
	{
		//empty
	}

	RenderPipeline::~RenderPipeline()
	{
	}
	//------------------------------------------------------------------------

	void RenderPipeline::setTargetWindow(PipelineParamters& context)
	{
		GraphicRenderer::ResetCache();
		if (context.m_targetWindows)
		{
			GraphicSystem::Instance()->_SetRenderWindow(context.m_targetWindows);
		}
		else
		{
			const GPtr<RenderToTexture>& rtt = context.m_targetSuite->GetRenderToTexture();
			if (rtt.isvalid())
			{
				GraphicSystem::Instance()->SetRenderTarget(rtt, 0, RenderBase::RenderTarget::ClearAll);
			}
		}
	}
	//------------------------------------------------------------------------
	void RenderPipeline::setGlobalShaderParam(PipelineParamters& params)
	{
		const Camera* camera = params.m_camera;

		if(camera->UseViewport())
		{
			const Camera::Viewport& vp = camera->GetViewport();
			GraphicSystem::Instance()->SetViewport(vp);
		}

		GlobalMaterialParam* pGMP = Material::GetGlobalMaterialParams();

		if (params.m_targetWindows)
		{
			const RenderBase::DisplayMode& dm = params.m_targetWindows->GetDisplayMode();
			Material::GetGlobalMaterialParams()->SetVectorParam(eGShaderVecScreenSize,
				float4(float(dm.GetWidth()), float(dm.GetHeight()), 0.5f / (float)dm.GetWidth(), 0.5f / (float)dm.GetHeight())		
				);
		}
		else
		{
			int width = params.m_targetSuite->GetWidth();
			int height = params.m_targetSuite->GetHeight();
			Material::GetGlobalMaterialParams()->SetVectorParam(eGShaderVecScreenSize,
				float4((float)width, (float)height, 0.5f / (float)width, 0.5f / (float)height)
				);
		}
		
		camera->GetRenderScene()->ApplyEnvironment();


		//��ȫ�ֲ�������Ϊ��ǰҪ��Ⱦ�����RT����
		// TODO�� ����wireframe �ж�RT��

		if (camera->IsRenderDepthMap())
		{
			Material::GetGlobalMaterialParams()->SetTextureParam(eGShaderTexDepthMap, params.m_targetSuite->GetDepthTexture()->GetTextureHandle());
		}

#if __WIN32__ && RENDERDEVICE_D3D9
		if (camera->IsRenderLightLitMap())
		{
			Material::GetGlobalMaterialParams()->SetTextureParam(eGShaderTexLightLitMap, params.m_targetSuite->GetLightLitTexture()->GetTextureHandle());
		}
		else
		{
			Material::GetGlobalMaterialParams()->SetTextureParam(eGShaderTexLightLitMap, GraphicSystem::Instance()->GetDefaultWhiteRTT()->GetTextureHandle());
		}
#endif
		const Math::matrix44& view = camera->GetViewTransform();
		const Math::matrix44& proj = camera->GetProjTransform();
		Math::float4 camPos = camera->GetTransform().get_position();
		camPos.set_w(Camera::VT_persp == camera->GetViewType() ? 0.0f : 1.0f);

		const CameraSetting& cs = camera->GetCameraSetting();

		Math::float4 camParam = Math::float4(cs.GetZNear(), cs.GetZFar(), cs.GetFarWidth() * 0.5f, cs.GetFarHeight() * 0.5f);

		pGMP->SetMatrixParam(eGShaderMatV,view);
		pGMP->SetMatrixParam(eGShaderMatInverseV,Math::matrix44::inverse(view));
		pGMP->SetMatrixParam(eGShaderMatP,proj);
		pGMP->SetVectorParam(eGShaderVecCameraPos,camPos);
		pGMP->SetVectorParam(eGShaderVecDebugCameraColor,camera->GetDebugColor());
		pGMP->SetVectorParam(eGShaderVecCameraParam, camParam);
		pGMP->SetMatrixParam(eGShaderMatInvVP, Math::matrix44::inverse(camera->GetViewProjTransform()));
	}
	//------------------------------------------------------------------------
	void RenderPipeline::setCommonLights(PipelineParamters& params)
	{
		ActiveLightCollection& activeLights = params.m_activeLights.GetActiveLights();
		int attBegin = params.m_activeLights.GetAttLightBeginIndex();

		int max = (attBegin < s_lightSupport) ? attBegin : s_lightSupport;

		for (int i = 0; i < max; ++i)
		{
			setLight(activeLights[i].light, i);
		}

		if (max < s_lightSupport)
		{
			for(int i = max; i < s_lightSupport; ++i)
			{
				setEnableLight(i, false);
			}
		}
	}
	//------------------------------------------------------------------------
	void RenderPipeline::setLight(const GPtr<Light>& light, SizeT lightindex)
	{
		GlobalMaterialParam* pGMP = Material::GetGlobalMaterialParams();

		Math::float4 lightColor = light->GetLightColor() * light->GetLightIntensity();
		Math::float4 lightDir = light->GetTransform().get_zaxis();//��shader�У�Ϊ�˷�����㣬�������������ʵ�ʷ���ķ�����
		Math::float4 lightPos = light->GetLightPos();
		Math::float4 attenuation;



		switch (light->GetLightType())
		{
		case Light::eSunLight:
			{
				lightColor.set_w(3.0f);
				lightPos = lightDir * 10000.0f;
				lightPos.set_w(1);
			}
			break;
		case Light::eSpotLight:
			lightColor.set_w(0);
			lightDir.set_w(1);
			lightPos.set_w(light->GetFalloff());

			attenuation.set_y(light->GetCosHalfOuterAngle());
			attenuation.set_z(light->GetInvCosConeDifference());
			attenuation.set_w(light->GetExponent());


			// attenuation.x ������������洢Lightrange
			attenuation.set_x( light->GetLightRange() ) ;

			pGMP->SetVectorParam(static_cast<GlobalVectorParamIndex>(eGShaderVecLightAttenuation0 + lightindex * 4),attenuation);
			break;
		case Light::ePointLight:
			lightColor.set_w(0);
			lightDir.set_w(0);
			lightPos.set_w(light->GetFalloff());

			// attenuation.x ������������洢Lightrange
			attenuation.set_x( light->GetLightRange() ) ;

			pGMP->SetVectorParam(static_cast<GlobalVectorParamIndex>(eGShaderVecLightAttenuation0 + lightindex * 4),attenuation);
			break;
		case Light::eDirectionalLight:
			lightColor.set_w(1);
			lightPos = lightDir * 10000.0f;
			lightPos.set_w(1);
			break;
		default:
			n_error("Invalide Light Type!");
		}
		pGMP->SetVectorParam(static_cast<GlobalVectorParamIndex>(eGShaderVecLightDiffuse0 + lightindex * 4),lightColor);
		pGMP->SetVectorParam(static_cast<GlobalVectorParamIndex>(eGShaderVecLightPos0 + lightindex * 4),lightPos);
		pGMP->SetVectorParam(static_cast<GlobalVectorParamIndex>(eGShaderVecLightDir0 + lightindex * 4),lightDir);
	}

	//------------------------------------------------------------------------
	void RenderPipeline::setEnableLight(SizeT lightindex,bool bEnable)
	{
		GlobalMaterialParam* pGMP = Material::GetGlobalMaterialParams();
		pGMP->SetVectorParam_W(static_cast<GlobalVectorParamIndex>(eGShaderVecLightPos0 + lightindex * 4), bEnable? 1.0f : -1.0f );
	}

	bool RenderPipeline::empty(PipelineParamters& params, RenderData::Type type)
	{
		RenderDataIndexArray& indices = params.m_renderDatas.GetRenderDataIndices(type);
		return (indices.Count() == 0);
	}

	bool RenderPipeline::contain(PipelineParamters& params, RenderData::Type type)
	{
		RenderDataIndexArray& indices = params.m_renderDatas.GetRenderDataIndices(type);
		return (indices.Count() > 0);
	}

	void RenderPipeline::renderRenderableList(PipelineParamters& params, RenderData::Type type ,RenderPassType passType, const Material* customMat)
	{
		RenderDataArray& datas = params.m_renderDatas.GetRenderDatas();
		RenderDataIndexArray& indices = params.m_renderDatas.GetRenderDataIndices(type);
		RenderDataIndexArray::Iterator it = indices.Begin();
		RenderDataIndexArray::Iterator end = indices.End();
		while (it != end)
		{
			RenderData& renderData = datas[*it];
			GraphicRenderer::BeforeRender(renderData.renderable, passType, customMat);
			renderData.onwer->object->Render(renderData.renderable, passType, customMat);
			++it;
		}
	}

	void RenderPipeline::renderRenderableList(PipelineParamters& params, RenderData::Type type , RenderPassType passType, const Material* customMat, uint mark)
	{
		RenderDataArray& datas = params.m_renderDatas.GetRenderDatas();
		RenderDataIndexArray& indices = params.m_renderDatas.GetRenderDataIndices(type);
		RenderDataIndexArray::Iterator it = indices.Begin();
		RenderDataIndexArray::Iterator end = indices.End();
		while (it != end)
		{
			RenderData& renderData = datas[*it];
			Renderable* renderable = renderData.renderable;
			if (renderable->GetMark() & mark)
			{
				GraphicRenderer::BeforeRender(renderData.renderable, passType, customMat);
				renderData.onwer->object->Render(renderData.renderable, passType, customMat);
			}

			++it;
		}
	}

	void RenderPipeline::renderRenderableListWidthLight(PipelineParamters& params, RenderData::Type type , const Material* customMat)
	{
		RenderDataArray& datas = params.m_renderDatas.GetRenderDatas();
		RenderDataIndexArray& indices = params.m_renderDatas.GetRenderDataIndices(type);
		RenderDataIndexArray::Iterator it = indices.Begin();
		RenderDataIndexArray::Iterator end = indices.End();

		int attBegin = params.m_activeLights.GetAttLightBeginIndex();
		attBegin = attBegin > s_lightSupport ? s_lightSupport : attBegin;
		int supportCount = s_lightSupport - attBegin;//��Щ���廹�ܽ��ܶ���յ˥����Դ��

		while (it != end)
		{
			RenderData& renderData = datas[*it];
			bool bUsedForLightmap = false;

			const TempLightBlock& block = params.m_activeLights.FindActiveAttLights(renderData.onwer->object, supportCount, bUsedForLightmap);

			int commonLightIndex = 0;
			int index = 0;

			SizeT nAffectedLight = 0;

			while(commonLightIndex < attBegin)
			{
				//set common light
				setEnableLight(commonLightIndex, !bUsedForLightmap);
				commonLightIndex++;
				nAffectedLight++;
			}

			while(index < block.Count())
			{
				setLight(block[index]->light, attBegin + index);
				++index;
				nAffectedLight++;
			}
			while(index < supportCount)
			{
				setEnableLight(attBegin + index, false);
				++index;
			}

#if RENDERDEVICE_OPENGLES
		if (nAffectedLight > 1)
		{
			nAffectedLight = 1;
		}
		
#endif

			renderData.renderable->SetAffectedLightCount(nAffectedLight);
			GraphicRenderer::BeforeRender(renderData.renderable, eForward, customMat);
			renderData.onwer->object->Render(renderData.renderable, eForward, customMat);
			++it;
		}

	}

	//------------------------------------------------------------------------
	void RenderPipeline::renderDepthMap(PipelineParamters& params)
	{
		if(!params.m_camera->IsRenderDepthMap())
			return;
		GraphicRenderer::ResetCache();
		const Camera* camera = params.m_camera;
		const GPtr<RenderToTexture>& depthRT = params.m_targetSuite->GetDepthTexture();
		GraphicSystem::Instance()->SetRenderTarget(depthRT,0,RenderBase::RenderTarget::ClearAll);

		renderRenderableList(params, RenderData::Opaque, eDepth, NULL, Renderable::GenDepth);
		//alphablend���� ����Ҫ��Ⱦ���
		//renderRenderableList(params, RenderData::Alpha, eDepth, NULL);

	}

	void RenderPipeline::renderLightLitMap(PipelineParamters& params)
	{
		const Camera* camera = params.m_camera;
		if(!(camera->IsRenderLightLitMap() && camera->IsRenderDepthMap()) )
			return;
		GraphicRenderer::ResetCache();
		const Light* sunLight = camera->GetRenderScene()->GetSunLight();
		if (NULL == sunLight)
		{
			return;
		}
		if(!sunLight->IsEnableShadow())
			return;
		GlobalMaterialParam* pGMP = Material::GetGlobalMaterialParams();
		n_assert(NULL != pGMP);

		const Util::Array< GPtr<Camera> >& smcList = sunLight->GetShadowMapCameraList();

		float4 lightCameraNear(0.f,0.f,0.f,0.f);
		float4 lightCameraFar(0.f,0.f,0.f,0.f);
		int count = smcList.Size();
		if (count > 0)
		{
			pGMP->SetMatrixParam(eGShaderMatLightV,smcList[0]->GetViewTransform());
			for (SizeT i = 0; i < count; ++i)
			{
				const CameraSetting& cs = smcList[i]->GetCameraSetting();
				lightCameraNear[i] = cs.GetZNear();
				lightCameraFar[i] = cs.GetZFar();
				pGMP->SetMatrixParam(static_cast<GlobalMatrixParamIndex>(eGShaderMatLightP0 + i ),smcList[i]->GetProjTransform());
			}
		}

		pGMP->SetVectorParam(eGShaderVecLightCameraNear, lightCameraNear);
		pGMP->SetVectorParam(eGShaderVecLightCameraFar, lightCameraFar);

		const GPtr<RenderToTexture>& lightlitRT = params.m_targetSuite->GetLightLitTexture();

		ImageFiltrationSystem::Render(NULL, lightlitRT, camera->GetLightLitMaterial());
	}

	//------------------------------------------------------------------------
	void RenderPipeline::renderCustomized(PipelineParamters& params)
	{
		if(!params.m_camera->IsRenderCustom())
			return;
		GraphicRenderer::ResetCache();
		const GPtr<RenderToTexture>& rtt = params.m_targetSuite->GetRenderToTexture();

		GraphicSystem::Instance()->SetRenderTarget(rtt, 0, RenderBase::RenderTarget::ClearAll);


		const Camera* camera = params.m_camera;
		const Material* mat = params.m_camera->GetCustomMaterial().get_unsafe();

		RenderPassType spt;
		if (params.m_camera->GetCameraOrder() == eCO_Shadow)
		{
			spt = eShadow;
		}
		else
		{
			spt = eCustomized;
		}

		//���û�������Ҫ��Ⱦ��Щ������壬���߽��˴��Ĳ������ü���mask���������

		renderRenderableList(params, RenderData::Opaque, spt, mat);

		if ( eShadow != spt)
		{
			//alphablend���� ����Ҫ��Ⱦ���
			renderRenderableList(params, RenderData::Alpha, spt, mat);
		}
	}

	void RenderPipeline::renderSpecialPost(PipelineParamters& params)
	{

#ifdef __XXX_TEST__
		return;
#endif

		if (eCO_Main == params.m_camera->GetCameraOrder() && (contain(params, RenderData::ParticleDecal) 
			|| contain(params, RenderData::ParticlePost) || contain(params, RenderData::Glass) ))
		{
			const Camera* camera = params.m_camera;

			const GPtr<RenderToTexture>& rtt= params.m_targetSuite->GetSwapTexture();
			const GPtr<RenderToTexture>& mainTex = params.m_targetSuite->GetRenderToTexture();

			if ( mainTex.isvalid() && rtt.isvalid() )
			{
				RenderData::Type type[3] = { RenderData::Glass, RenderData::ParticleDecal, RenderData::ParticlePost};

				for (int index = 0; index < sizeof(type)/sizeof(RenderData::Type); index++)
				{
					// �Ƚ�mianbuffer�����ݸ��Ƶ�swapbuffer
					ImageFiltrationSystem::Render(&(mainTex->GetTextureHandle()), rtt, NULL, 0, RenderBase::RenderTarget::ClearNone);
					GraphicSystem::Instance()->SetRenderTarget(mainTex->GetTargetHandle(),0,RenderBase::RenderTarget::ClearNone);

					GlobalMaterialParam* pGMP = Material::GetGlobalMaterialParams();
					pGMP->SetTextureParam(eGShaderTexSwapBuffer, rtt->GetTextureHandle());
					//draw Special effect
					renderRenderableList(params, type[index], eForward, NULL);
				}
			}
		}
	}

	void RenderPipeline::renderCustomObjs(PipelineParamters& params)
	{
		GraphicRenderer::ResetCache();
		renderRenderableList(params, RenderData::Custom, eForward, NULL);
	}

	void RenderPipeline::renderScreenObjs(PipelineParamters& params)
	{
		//render 2d mesh

		RenderDataIndexArray& indices = params.m_renderDatas.GetRenderDataIndices(RenderData::Screen);

		GraphicRenderer::ResetCache();
		const GPtr<RenderToTexture>& rtt = params.m_targetSuite->GetRenderToTexture();

		GraphicSystem::Instance()->SetRenderTarget(rtt, 0, RenderBase::RenderTarget::ClearDepth);

		matrix44 view = matrix44::identity();
		ViewPortWindow* window = params.m_camera->GetTargetWindow();
		if (NULL == window)
		{
			window = GraphicSystem::Instance()->GetMainViewPortWindow();
		}
		const RenderBase::DisplayMode& dm = window->GetDisplayMode();

		float width = (float)dm.GetWidth();
		float height = (float)dm.GetHeight();

		//z_scale: ������(max, -max)ѹ����(-0.5f,0.5f)��maxΪwidth,height����֮����
		float z_scale = (width > height) ? (-0.5f / width) : (-0.5f / height);

		view.mx[0][0] = 2.0f / width;
		view.mx[1][1] = 2.0f / height;
		view.mx[2][2] = z_scale;
		view.mx[0][3] = -1.0f;
		view.mx[1][3] = 1.0f;
		view.mx[2][3] = 0.5f;//������(-0.5f,0.5f)�Ƶ�(0,1)��
		GlobalMaterialParam* pGMP = Material::GetGlobalMaterialParams();
		pGMP->SetMatrixParam(eGShaderMatV, view);//matrix44::identity());// 

		renderRenderableList(params, RenderData::Screen, eForward, NULL);
	}
	//--------------------------------------------------------------------------------
	void RenderPipeline::renderMain(PipelineParamters& params)
	{
		GraphicSystem::Instance()->SetWireFrameMode(false);

		const GPtr<Camera>& camera = params.m_camera;
		const GPtr<RenderToTexture>& mainBuffer = params.m_targetSuite->GetRenderToTexture();
		if (!mainBuffer.isvalid())
		{
			return;
		}
		GraphicRenderer::ResetCache();
		RenderToTexture* source = mainBuffer.get();
		ImageFilterManager& postEffects = camera->GetPostEffectFilters();
		int count = postEffects.Size();
		if (count > 0)
		{
			RenderToTexture* destination = params.m_targetSuite->GetSwapTexture().get();
			int index = 0;
			PROFILER_ADDDTICKBEGIN(postTime);
			while(index < count)
			{
				if(postEffects.GetFilter(index)->RenderFilter(source, destination))
				{
					RenderToTexture* temp = destination;
					destination = source;
					source = temp;
				}	
				++index;
			}
			PROFILER_ADDDTICKEND(postTime);
		}

		if (source == params.m_targetSuite->GetSwapTexture().get())
		{
			PROFILER_ADDDTICKBEGIN(postTime);
			ImageFiltrationSystem::Render(&source->GetTextureHandle(), mainBuffer.get(), NULL, 0);//SwapTextureû����Ȼ��壬����Ҫ��copy��mainBuffer�
			PROFILER_ADDDTICKEND(postTime);
			source = mainBuffer.get();
		}

		if (contain(params,RenderData::Screen))
		{
			renderScreenObjs(params);
		}			

		if (params.m_targetWindows == NULL)
		{
			GraphicSystem::Instance()->SetRenderTarget(RenderBase::RenderTargetHandle(), 0, RenderBase::RenderTarget::ClearNone);
		}
		//else
		//{
		//	GraphicSystem::Instance()->SetRenderTarget(params.m_targetWindows->GetBackBuffer(), 0, RenderBase::RenderTarget::ClearNone);
		//	ImageFiltrationSystem::Render(&source->GetTextureHandle(), 
		//		params.m_targetWindows->GetBackBuffer().get(), NULL, NULL, 0);
		//}
	}
} 
