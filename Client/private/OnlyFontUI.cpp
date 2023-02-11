#include "stdafx.h"
#include "..\public\OnlyFontUI.h"
#include "GameInstance.h"

COnlyFontUI::COnlyFontUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

COnlyFontUI::COnlyFontUI(const COnlyFontUI & rhs)
	: CUI(rhs)
{
}

void COnlyFontUI::Set_FontOption(_int iOption)
{
	m_iOption = iOption;
	m_StrCurNum = 0;
	if (m_iOption == 0)
	{
		m_strText[0] = TEXT("�ȳ��ϼ��� ���谡�Ե�!!");
		m_strText[1] = TEXT("�� ���� ���� �ö󰡼� �������� ���� ������ ã������");
		m_strText[2] = TEXT("���͵��� ó���ϸ鼭 �ö���ø� �˴ϴ�!");
		m_StrMaxNum = 2;
	}
	else if (m_iOption == 1)
	{
		m_strText[0] = TEXT("���͵��� ó���ϴ��� ����߾��");
		m_strText[1] = TEXT("ü�°� ������ ȸ�������ٰԿ�");
		m_strText[2] = TEXT("���� �ܻ��� �и� �������� ó�����ּ���");
		m_strText[3] = TEXT("�������� ���ʿ� �ֽ��ϴ�");
		m_strText[4] = TEXT("����ϼ���!!");

		m_StrMaxNum = 4;
	}

	else if (m_iOption == 2)
	{
		m_strText[0] = TEXT("���ʿ� �ִ� �������͸� ��������");
		m_strText[1] = TEXT("ü�°� ������ ȸ�������ٰԿ�");
		m_strText[2] = TEXT("�� ���丮 ¥�� �����");
		m_strText[3] = TEXT("���� ���ʹ� ���ʿ� �ֽ��ϴ�!");
		m_strText[4] = TEXT("������ ��������� Ű���忡�� UŰ�� �����ּ���");
		m_StrMaxNum = 4;
	}

}

HRESULT COnlyFontUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT COnlyFontUI::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT COnlyFontUI::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
	m_bLast_Initlize = true;

	return S_OK;
}

void COnlyFontUI::Tick(_double TimeDelta)
{
	Last_Initialize();

	__super::Tick(TimeDelta);


	if (CGameInstance::GetInstance()->Key_Down(DIK_Q))
	{
		if (m_StrCurNum < m_StrMaxNum)
		{
			++m_StrCurNum;
		}
		else
		{
			m_StrCurNum = m_StrMaxNum;
		}
	}



}

void COnlyFontUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);




	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_INVENTORY, this);
}

HRESULT COnlyFontUI::Render()
{
	if (!m_bRenderActive)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Render_Font(TEXT("Font_Comic"), m_strText[m_StrCurNum].c_str(), _float2(m_fFontPosX, m_fFontPosY), 0.f, _float2(0.8f, 0.8f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT COnlyFontUI::SetUp_Components()
{
	/* For.Com_Renderer */
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
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT COnlyFontUI::SetUp_ShaderResources()
{
	return S_OK;
}

COnlyFontUI * COnlyFontUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	COnlyFontUI*		pInstance = new COnlyFontUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : COnlyFontUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * COnlyFontUI::Clone(void * pArg)
{
	COnlyFontUI*		pInstance = new COnlyFontUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : COnlyFontUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COnlyFontUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
