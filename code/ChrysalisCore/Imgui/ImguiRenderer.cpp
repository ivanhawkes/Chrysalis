#include "StdAfx.h"
#include "ImguiRenderer.h"
#include "Imgui/imgui.h"
#include "CryRenderer/IRenderAuxGeom.h"
#include "CryEntitySystem/IEntitySystem.h"

using namespace Cry::Renderer::CustomPass;

static uint32 gPassCrc = CCrc32::Compute_CompileTime("ImguiPass");
static uint32 gTechniqueCrc = CCrc32::ComputeLowercase_CompileTime("Imgui");

static SInputElementDescription local_layout[] =
{
	{ "POSITION", 0, EInputElementFormat::FORMAT_R32G32_FLOAT,   0, 0, EInputSlotClassification::PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, EInputElementFormat::FORMAT_R32G32_FLOAT,   0, 8,  EInputSlotClassification::PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, EInputElementFormat::R8G8B8A8_UNORM,		 0, 16, EInputSlotClassification::PER_VERTEX_DATA, 0 },
	
};
static int gLayoutID = 0;

static auto gMvpConstant = std::make_pair("mvpMatrix", 0);

CImguiRenderer::CImguiRenderer(ICustomPassRenderer* pUIRenderer, Vec2i rtDimensions)
	: m_pUIRenderer(pUIRenderer)
	, m_renderDimensions(rtDimensions)
{
	m_pUIRenderer->CreateRendererInstance("ImGui", this, sizeof(Vec4) * 4);

	auto pShader = gEnv->pRenderer->EF_LoadShader("Imgui");
	if (!pShader)
		return;
	m_pImguiShader.Assign_NoAddRef(pShader);
}


bool CImguiRenderer::RT_Initalize(std::unique_ptr<ICustomRendererInstance> pInstance)
{
	m_pInstance = std::move(pInstance);

	CryLogAlways("Initializing imgui renderer");
	
	
	auto pShader = m_pImguiShader.get();
	auto params = pShader->GetPublicParams();

	gMvpConstant.second = m_pUIRenderer->RT_RegisterConstantName(gMvpConstant.first);

	TArray<SInputElementDescription> layoutView(local_layout, 3);
#ifdef PRE_5_5
	gLayoutID = m_pUIRenderer->RT_RegisterLayout(layoutView, pShader, gTechniqueCrc);
#else
	gLayoutID = m_pUIRenderer->RT_RegisterLayout(layoutView);
#endif

	//m_pMainRt = gEnv->pRenderer->EF_GetTextureByName("$SceneDiffuse");

	/*SRTCreationParams creationParams = {
		"Imgui_Rt",
		m_renderDimensions.x,
		m_renderDimensions.y,
		Col_Transparent,
		eTT_2D,
		FT_NOMIPS | FT_DONT_STREAM | FT_USAGE_RENDERTARGET,
		ETEX_Format::eTF_R8G8B8A8
	};

	auto pRT = m_pUIRenderer->CreateDynamicRenderTarget(creationParams);
	m_pDynRT = std::move(pRT);*/

	UpdateRenderTarget();

	Matrix44 mvp;
	mathMatrixOrthoOffCenterLH(&mvp, 0, (float)m_renderDimensions.x, (float)m_renderDimensions.y, 0, 0, 1);
	mvp = mvp.GetTransposed();

	memcpy(&m_mvpConstant, mvp.GetData(), sizeof(Matrix44));

	SPassCreationParams passParams;
	passParams.passName = "ImguiPass";
	UpdatePassParams(passParams);

	m_pInstance->CreateCustomPass(passParams, gPassCrc);

	m_bInitialized = true;
	return true;
}

void CImguiRenderer::RT_Shutdown()
{
	m_bInitialized = false;
	m_pRenderTarget.reset(nullptr);

	for (auto &bfrs : m_bufferHandles)
	{
		m_pUIRenderer->RT_FreeBuffer(bfrs.first);
		m_pUIRenderer->RT_FreeBuffer(bfrs.second);
		
	}

	m_pImguiShader.reset();
	m_pInstance.reset();
}

void CImguiRenderer::RT_Update(bool bLoadingThread /*= false*/)
{
	//Todo::Handle updates during load
}

void CImguiRenderer::RenderImgui()
{
	ImGui::Render();
	CryAutoCriticalSection lock(m_renderLock);
	//Copy the buffers to the render thread;
	auto pDrawData = ImGui::GetDrawData();
	m_rtDrawData.TotalIdxCount = pDrawData->TotalIdxCount;
	m_rtDrawData.TotalVtxCount = pDrawData->TotalVtxCount;
	m_rtDrawData.DisplayPos = pDrawData->DisplayPos;
	m_rtDrawData.DisplaySize = pDrawData->DisplaySize;
	m_rtDrawData.FramebufferScale = pDrawData->FramebufferScale;

	m_rtDrawLists.resize(pDrawData->CmdListsCount);
	for (int i = 0; i < pDrawData->CmdListsCount; ++i)
	{
		auto pDrawList = pDrawData->CmdLists[i];
		if (pDrawList)
		{
			auto pRTList = &m_rtDrawLists[i];

			pRTList->CmdBuffer = pDrawList->CmdBuffer;
			pRTList->IdxBuffer = pDrawList->IdxBuffer;
			pRTList->VtxBuffer = pDrawList->VtxBuffer;
			pRTList->Flags = pDrawList->Flags;
		}
	}

	//Just draw to screen via auxgeom for now
	//Use proper FulscreenStretch pass when implemented
	if (!m_pRenderTarget.get())
		return;
#ifdef PRE_5_5
	m_pUIRenderer->ExecuteRTCommand([id = m_pRenderTarget->GetTextureID()]()
	{
		int x, y, width, height;
		gEnv->pRenderer->GetViewport(&x, &y, &width, &height);

		float fWidth = (float)gEnv->pRenderer->GetOverlayWidth();// float)width;
		float fHeight = (float)gEnv->pRenderer->GetOverlayHeight(); //(float)height;

		fWidth /= (fWidth / 800);
		fHeight /= (fHeight / 600);

		gEnv->pRenderer->SetState(GS_BLSRC_SRCALPHA | GS_BLDST_ONEMINUSSRCALPHA | GS_NODEPTHTEST);
		gEnv->pRenderer->Push2dImage((float)x, (float)y, fWidth, fHeight, id, 0, 1.0f, 1.0f, 0);
	}, true
	);
#else
	//Just draw to screen via auxgeom for now
	//Use proper FulscreenStretch pass when implemented
	IRenderAuxImage::Draw2dImage(
		0,
		0,
		(float)m_renderDimensions.x, (float)m_renderDimensions.y,
		m_pRenderTarget->GetTextureID(), 0, 1.0f, 1.0f, 0);
#endif

	if (auto pEnt = gEnv->pEntitySystem->FindEntityByName("testingentity"))
	{
		if (auto pMat = pEnt->GetSlotMaterial(0))
			if (pMat->GetShaderItem().m_pShaderResources->GetTexture(0)->m_Sampler.m_pITex != m_pRenderTarget)
			{
				pMat->SetTexture(m_pRenderTarget->GetTextureID(), 0);
				m_pLastRT = m_pRenderTarget.get();
			}
	}
}


void CImguiRenderer::RT_Render()
{
	if (!m_bInitialized)
		return;

	CryAutoCriticalSection lock(m_renderLock);
	if (m_rtDrawLists.empty())
	{
		if (m_bClearOnEmpty)
		{
			m_pUIRenderer->RT_ClearRenderTarget(m_pRenderTarget.get(), Col_Transparent);
			m_bClearOnEmpty = false;
		}
		return;
	}
		
	bool rtUpdate = IsPassDataDirty();

	SPassUpdateScope scope;

	if (!rtUpdate)
		scope = std::move(m_pInstance->RT_BeginScopedPass(gPassCrc));
	else
	{
		SPassParams passParams;
		UpdatePassParams(passParams);
		scope = std::move(m_pInstance->RT_BeginScopedPass(gPassCrc,passParams));
	}

	AdjustRenderMeshes();
	for (int i = 0; i < m_rtDrawLists.size(); ++i)
	{
		DrawCommandList(m_rtDrawLists[i], i);
	}

	m_blendModeRT = m_blendMode;
	m_bClearOnEmpty = true;
}


void CImguiRenderer::UpdateRenderTarget()
{
	SRTCreationParams creationParams = {
		"Imgui_Rt",
		m_renderDimensions.x,
		m_renderDimensions.y,
		Col_Transparent,
		eTT_2D,
		FT_NOMIPS | FT_DONT_STREAM | FT_USAGE_RENDERTARGET,
		ETEX_Format::eTF_R8G8B8A8
	};

	m_pRenderTarget = m_pUIRenderer->CreateRenderTarget(creationParams);

	m_pUIRenderer->ExecuteRTCommand([this]() { m_pUIRenderer->RT_ClearRenderTarget(m_pRenderTarget, Col_Transparent); });
}

bool CImguiRenderer::IsPassDataDirty()
{
	m_renderDimensions.x = (int)(m_rtDrawData.DisplaySize.x * m_rtDrawData.FramebufferScale.x);
	m_renderDimensions.y = (int)(m_rtDrawData.DisplaySize.y * m_rtDrawData.FramebufferScale.y);

	bool bWidthChanged = m_pRenderTarget->GetWidth() != m_renderDimensions.x;
	bool bHeightChanged = m_pRenderTarget->GetHeight() != m_renderDimensions.y;

	if (bHeightChanged || bWidthChanged)
	{
		Matrix44 mvp;
		mathMatrixOrthoOffCenterLH(&mvp, 0, (float)m_renderDimensions.x, (float)m_renderDimensions.y, 0, 0, 1);
		mvp = mvp.GetTransposed();

		memcpy(m_mvpConstant.m_mvpMat, mvp.GetData(), sizeof(Matrix44));

		//m_pDynRT->Update(m_renderDimensions.x, m_renderDimensions.y);
		UpdateRenderTarget();	
	}

	if (m_mvpConstantBuffer == INVALID_BUFFER)
	{
		m_mvpConstantBuffer = m_pUIRenderer->RT_CreateConstantBuffer(sizeof(SMvpMat));
	}
		
	return (bConstantWasDirty = bWidthChanged || bHeightChanged);
}

void CImguiRenderer::UpdateBuffers(const SRTDrawList &list, int meshIDX)
{
	SBufferParams paramsVtx(list.VtxBuffer.size(), sizeof(ImDrawVert), EBufferBindType::Vertex, EBufferUsage::Dynamic);
	paramsVtx.pData = list.VtxBuffer.Data;
	m_bufferHandles[meshIDX].first = m_pUIRenderer->RT_CreateOrUpdateBuffer(paramsVtx, m_bufferHandles[meshIDX].first);

	SBufferParams paramsIdx(list.IdxBuffer.size(), sizeof(ImDrawIdx), EBufferBindType::Index, EBufferUsage::Dynamic);
	paramsIdx.pData = list.IdxBuffer.Data;
	m_bufferHandles[meshIDX].second = m_pUIRenderer->RT_CreateOrUpdateBuffer(paramsIdx, m_bufferHandles[meshIDX].second);
}

void CImguiRenderer::DrawRenderCommand(const ImDrawCmd &cmd, int meshIDX)
{
	SDrawParamsExternalBuffers drawParams;

	drawParams.inputBuffer = m_bufferHandles[meshIDX].first;
	drawParams.inputSize = cmd.ElemCount / 3;
	drawParams.inputLayout = gLayoutID;
	drawParams.inputStride = sizeof(ImDrawVert);
	drawParams.inputOffset = m_currentVtxOffset;

	drawParams.idxBuffer = m_bufferHandles[meshIDX].second;
	drawParams.idxSize = cmd.ElemCount;
	drawParams.idxOffset = m_currentIdxOffset;

	drawParams.drawType = EDrawTypes::TriangleList;

	SShaderParams shaderParams;
	shaderParams.pShader = m_pImguiShader;
	shaderParams.techniqueLCCRC = gTechniqueCrc;

	STextureParam texParam;
	texParam.pTexture = cmd.TextureId;

	SSamplerParam samplerParam;
	samplerParam.handle = 12; // EDefaultSamplerStates::BilinearCompare ;

	shaderParams.textures = TArray<STextureParam>(&texParam, 1);
	shaderParams.samplerStates = TArray<SSamplerParam>(&samplerParam, 1);

	/*SNamedConstantArray mvpConstant;
	mvpConstant.shaderClass = EShaderClass::Vertex;
	mvpConstant.constantIDX = gMvpConstant.second;
	mvpConstant.values = TArray<Vec4>(m_mvpConstant.m_mvpMat, 4);

	SNamedConstantsParams constantParams;
	constantParams.namedConstantArrays = TArray<SNamedConstantArray>(&mvpConstant, 1);*/


	SConstantBuffer constantBuffer;
	constantBuffer.externalBuffer = m_mvpConstantBuffer;
	constantBuffer.newData = (uint8*)&m_mvpConstant;
	constantBuffer.dataSize = sizeof(m_mvpConstant);
	constantBuffer.stages = ssAllWithoutCompute;
	constantBuffer.slot = EConstantSlot::PerPass;

	SInlineConstantParams constantParams;
	constantParams.buffers = TArray<SConstantBuffer>(&constantBuffer, 1);

	SPrimitiveParams primitiveParams(shaderParams, constantParams, drawParams);

	/*primitiveParams.stencilState =
		STENC_FUNC(STENCFUNC_ALWAYS) |
		STENCOP_FAIL(STENCOP_KEEP) |
		STENCOP_ZFAIL(STENCOP_KEEP) |
		STENCOP_PASS(STENCOP_KEEP);*/

	//primitiveParams.stencilRef = 1;
	//primitiveParams.renderStateFlags = GS_BLSRC_ONE | GS_BLDST_ONEMINUSSRCALPHA; 
	primitiveParams.renderStateFlags = m_blendModeRT;//GS_BLSRC_SRCALPHA | GS_BLDST_ONEMINUSSRCALPHA;
	//primitiveParams.renderStateFlags = GS_BLSRC_SRC1ALPHA	| GS_BLDST_ONEMINUSSRC1ALPHA | GS_BLALPHA_MIN | GS_DEPTHFUNC_LEQUAL;

	auto clipRect = cmd.ClipRect;
	primitiveParams.scissorRect = Vec4_tpl<ulong>((ulong)clipRect.x, (ulong)clipRect.y, (ulong)clipRect.z, (ulong)clipRect.w);

	m_pInstance->RT_ScopedAddPrimitive(primitiveParams);

}

void CImguiRenderer::AdjustRenderMeshes()
{

	int numNewMeshes = (int)m_rtDrawLists.size() - (int)m_bufferHandles.size();
	if (numNewMeshes > 0)
	{
		m_bufferHandles.resize(m_bufferHandles.size() + numNewMeshes, { INVALID_BUFFER,INVALID_BUFFER });
	}
	else if (numNewMeshes < 0)
	{
		//Free unnecessaty buffers
		int newLastIdx = (m_bufferHandles.size() - numNewMeshes) - 1;
		for (int i = newLastIdx; i < m_bufferHandles.size(); ++i)
		{
			m_pUIRenderer->RT_FreeBuffer(m_bufferHandles[i].first);
			m_pUIRenderer->RT_FreeBuffer(m_bufferHandles[i].second);
		}

		m_bufferHandles.resize(m_rtDrawLists.size());
	}
}

void CImguiRenderer::UpdatePassParams(SPassParams &params)
{
	params.viewPort = SRenderViewport(0, 0, m_renderDimensions.x, m_renderDimensions.y);
	params.pColorTarget = m_pRenderTarget.get();
	//passParams.pDepthsTarget = m_pDynStencilTarget->GetTexture();
	params.clearMask = BIT(2);// (BIT(1) | );
}


void CImguiRenderer::DrawCommandList(const SRTDrawList &list, int meshIDX)
{
	m_currentIdxOffset = 0;
	UpdateBuffers(list, meshIDX);

	for (auto &command : list.CmdBuffer)
	{
		DrawRenderCommand(command, meshIDX);
		m_currentIdxOffset += command.ElemCount;
	}
}



