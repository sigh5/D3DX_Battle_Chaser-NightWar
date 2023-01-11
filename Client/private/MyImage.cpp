#include "stdafx.h"
#include "..\public\MyImage.h"
#include "GameInstance.h"
#include "Client_Manager.h"

CMyImage::CMyImage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMyImage::CMyImage(const CMyImage & rhs)
	: CUI(rhs)
{
}

HRESULT CMyImage::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMyImage::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UIDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = (_float)g_iWinSizeX / 16;
	m_fSizeY = (_float)g_iWinSizeY / 16;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - m_fSizeX * 0.5f, -m_fY + m_fSizeY * 0.5f + 50.f, 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	m_bRenderActive = true;

	return S_OK;
}

HRESULT CMyImage::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;




	m_bLast_Initlize = true;
	return S_OK;
}

void CMyImage::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	Shake_Move(TimeDelta);
}

void CMyImage::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMyImage::Render()
{
	if (!m_bRenderActive)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	m_pShaderCom->Begin(1);		// UI 1번 알파블랜딩
	m_pVIBufferCom->Render();


	if (m_bRenderFont)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT(" VicTory !!"), _float2(m_fFontPosX, m_fFontPosY), 0.f, _float2(1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		RELEASE_INSTANCE(CGameInstance);
	}


	return S_OK;
}

void CMyImage::Change_Texture(_uint iLevel, const wstring & NewComPonentTag)
{
	Safe_Release(m_pTextureCom);
	Remove_component(TEXT("Com_Texture"));

	lstrcpy(m_UIDesc.m_pTextureTag, NewComPonentTag.c_str());

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(iLevel, m_UIDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		assert(! "CTurnCharcterUI Change_Texture");
}

void CMyImage::Set_RenderActive(_bool bRenderActive)
{
	__super::Set_RenderActive(bRenderActive);

}

void CMyImage::MoveTexture(_double TimeDelta)
{
	m_bRenderFont = true;
	m_fTextureMoveTimer += (_float)TimeDelta;

	if (m_fTextureMoveTimer >= 0.1f)
	{
		_uint iTextureNum 	=	m_pTextureCom->Get_SelectTextureIndex();
		
		if (iTextureNum >= m_iMaxTextureNum)
		{
			CClient_Manager::m_bCombatWin = true;
			return;
		}
		if (iTextureNum + 1 == 2)
			m_pTransformCom->Set_Scaled(_float3(89.f, 99.f, 1.f));

		if (iTextureNum + 1 == 4)
		{
			m_pTransformCom->Set_Scaled(_float3(126.f, 142.f, 1.f));
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-10.f, 50.f, 0.1f, 1.f));
		}
		if (iTextureNum + 1 == 5)
			m_pTransformCom->Set_Scaled(_float3(123.f, 138.f, 1.f));

		if(iTextureNum + 1 == 6)
			m_pTransformCom->Set_Scaled(_float3(123.f, 134.f, 1.f));

		if (iTextureNum + 1 == 7)
			m_pTransformCom->Set_Scaled(_float3(123.f, 130.f, 1.f));


		m_pTextureCom->Set_SelectTextureIndex(++iTextureNum);
		m_fTextureMoveTimer = 0.f;
	}


}

void CMyImage::ShakingControl(_float fCoolTime)
{
	if (m_iShakingOption == 1)
		return;
	
	m_bMove = true;
	XMStoreFloat4(&m_vOriginPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
}

void CMyImage::Shake_Move(_double TimeDelta)
{
	if (m_bMove)
	{
		if (m_fShakeTime > m_fCurShakeTime)
		{
			m_fCurShakeTime += (_float)TimeDelta;
			_float ShakefX = CClient_Manager::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;
			_float ShakefY = CClient_Manager::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;

			_matrix matLocal = XMMatrixTranslation(ShakefX, ShakefY, 0.f);

			_matrix matWorld = m_pTransformCom->Get_LocalMatrix();

			matWorld = matLocal*matWorld;

			matWorld = matLocal*matWorld;

			_float4x4	ShakingMat;
			XMStoreFloat4x4(&ShakingMat, matWorld);
			m_pTransformCom->Set_WorldMatrix(ShakingMat);
		}
		else
		{
			m_bMove = false;
			
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,XMLoadFloat4(&m_vOriginPos));
		}
	}
	


}

HRESULT CMyImage::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_UIDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMyImage::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_ORTH))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_pTextureCom->Get_SelectTextureIndex())))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMyImage * CMyImage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMyImage*		pInstance = new CMyImage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTurnCharcterUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMyImage::Clone(void * pArg)
{
	CMyImage*		pInstance = new CMyImage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTurnCharcterUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMyImage::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
