#include "stdafx.h"
#include "..\public\Inventory.h"
#include "GameInstance.h"
#include "Inventory_Button.h"
#include "MyImage.h"
#include "Status.h"

CInventory::CInventory(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCanvas(pDevice, pContext)
{
}

CInventory::CInventory(const CInventory & rhs)
	: CCanvas(rhs)
{
}

void CInventory::Set_Knolan(CGameObject * pPlayer)
{
	CStatus* pStatus = static_cast<CStatus*>(pPlayer->Get_Component(TEXT("Com_StatusDungeon")));
	
	assert(nullptr != pStatus && " CInventory::Set_Knolan");
	m_pKnolan_Status = pStatus;
}

void CInventory::Set_Garrison(CGameObject * pPlayer)
{
	CStatus* pStatus = static_cast<CStatus*>(pPlayer->Get_Component(TEXT("Com_StatusDungeon")));
	assert(nullptr != pStatus && " CInventory::Set_Garrison");
	m_pGarrison_Status = pStatus;
}

void CInventory::Set_Calibretto(CGameObject * pPlayer)
{
	CStatus* pStatus = static_cast<CStatus*>(pPlayer->Get_Component(TEXT("Com_StatusDungeon")));
	assert(nullptr != pStatus && " CInventory::Set_Calibretto");
	m_pCalibretto_Status = pStatus;
}

HRESULT CInventory::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CInventory::Initialize(void * pArg)
{
	CCanvas::CANVASDESC		CanvasDesc;
	ZeroMemory(&CanvasDesc, sizeof(CanvasDesc));

	lstrcpy(CanvasDesc.m_pTextureTag, TEXT("Prototype_Component_Texture_Inventory"));

	if (FAILED(__super::Initialize(&CanvasDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = (_float)g_iWinSizeX / 11;
	m_fSizeY = (_float)g_iWinSizeY * 1.1f;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - m_fSizeX * 0.5f - (585.f), -m_fY + m_fSizeY * 0.5f, 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CInventory::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
#ifdef NOMODLES
	
	CInventory::Set_RenderActive(false);
#else

	Set_RenderActive(false);
	for (auto& pChild : m_ChildrenVec)
	{
		if (!lstrcmp(pChild->Get_ObjectName(), TEXT("Inventory_Button_Icon_0")))
		{
			m_mapIconButton.emplace(TEXT("Gully"), pChild);
		}
		else if (!lstrcmp(pChild->Get_ObjectName(), TEXT("Inventory_Button_Icon_1")))
		{
			m_mapIconButton.emplace(TEXT("Garrison"), pChild);
		}
		else if (!lstrcmp(pChild->Get_ObjectName(), TEXT("Inventory_Button_Icon_2")))
		{
			m_mapIconButton.emplace(TEXT("Calibretto"), pChild);
		}
		else
			continue;
	}
#endif
	
	

	
	m_bLast_Initlize = true;

	return S_OK;
}

void CInventory::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);
#ifdef NOMODLES

#else
	ICon_Button_Click();
	Cur_BigImageCast();
	
	Owner_KNOLAN_Tick(TimeDelta);
	Owner_GARRISON_Tick(TimeDelta);
	Owner_CALIBRETTO_Tick(TimeDelta);
#endif
}

void CInventory::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
#ifdef NOMODLES

#else
	Owner_KNOLAN_LateTick( TimeDelta);
	Owner_GARRISON_LateTick( TimeDelta);
	Owner_CALIBRETTO_LateTick( TimeDelta);
#endif
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInventory::Render()
{
	if (!m_bRenderActive)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Render();

	return S_OK;
}


void CInventory::Set_RenderActive(_bool bTrue)
{
	__super::Set_RenderActive(bTrue);

	for (auto& pUI : m_ChildrenVec)
		pUI->Set_RenderActive(bTrue);

	if (bTrue== true)
	{
		Owner_KNOLAN_Set();
		Owner_CALIBRETTO_Set();
		Owner_GARRISON_Set();
		
	}

}

void CInventory::Clear_InventoryImage()
{
	for (auto& pVec : m_Knolan_InvenImage)
		Safe_Release(pVec);
	m_Knolan_InvenImage.clear();
	
	for (auto& pVec : m_Garrison_InvenImage)
		Safe_Release(pVec);
	m_Garrison_InvenImage.clear();

	for (auto& pVec : m_Calibretto_InvenImage)
		Safe_Release(pVec);
	m_Calibretto_InvenImage.clear();

}

void CInventory::Owner_KNOLAN_Set()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObjcet = nullptr;

	UIDESC InvenImageDesc;
	ZeroMemory(&InvenImageDesc, sizeof(UIDESC));
	lstrcpy(InvenImageDesc.m_pTextureTag , TEXT("Prototype_Component_Texture_Inventory"));


	map<CStatus::ITEMID, _int> ITemMap;
	
	if (m_iImageOption == INVEN_KNOLAN)
	{
		for (auto& pVec : m_Knolan_InvenImage)
			Safe_Release(pVec);
		m_Knolan_InvenImage.clear();
		m_Knolan_InvenImage.reserve(5);
		ITemMap = m_pKnolan_Status->Get_ITemMap();
		
	}
	else if (m_iImageOption == INVEN_GARRISON)
	{
		for (auto& pVec : m_Garrison_InvenImage)
			Safe_Release(pVec);
		m_Garrison_InvenImage.clear();
		m_Garrison_InvenImage.reserve(5);
		ITemMap = m_pGarrison_Status->Get_ITemMap();
	}
	else if (m_iImageOption == INVEN_CALIBRETTO)
	{
		for (auto& pVec : m_Calibretto_InvenImage)
			Safe_Release(pVec);
		m_Calibretto_InvenImage.clear();
		m_Calibretto_InvenImage.reserve(5);
		ITemMap = m_pCalibretto_Status->Get_ITemMap();
	}
	else
	{
		_bool b = false;
		RELEASE_INSTANCE(CGameInstance);
		return;

	}

	_int iTextureNum = 0;
	_int iIndex = 0;
	_float xPos = -150.f;
	_float yPos = -210.f;
	_float fFontPosx = 495.f;
	_float fFontPosy = 585.f;
	
	for (auto& pItem : ITemMap)
	{
		if (pItem.first == CStatus::ITEM_HP_POTION)
		{
			iTextureNum = IVT_HP;
		}
		else if (pItem.first == CStatus::ITEM_MP_POSION)
		{
			iTextureNum = IVT_MP; 
		}
		else if (pItem.first == CStatus::ITEM_ULTIMATE_BOOK)
		{
			iTextureNum = IVT_BOOK;
		}

		pGameObjcet = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_MyImage"),&InvenImageDesc);
		
		static_cast<CMyImage*>(pGameObjcet)->Set_CurTextureNum(iTextureNum);
		static_cast<CMyImage*>(pGameObjcet)->Renderer_State_Inventory();
		static_cast<CMyImage*>(pGameObjcet)->Set_InventoryNumStr(to_wstring(pItem.second));
		static_cast<CMyImage*>(pGameObjcet)->Set_InventoryFontPos(_float2(fFontPosx, fFontPosy));
		wstring Temp = to_wstring(_int(pItem.first));
		pGameObjcet->Set_ObjectName(Temp.c_str());
		pGameObjcet->Get_Transform()->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(xPos, yPos, 0.1f, 1.f));
		pGameObjcet->Get_Transform()->Set_Scaled(_float3(70.f, 70.f, 1.f));		
		
		++iIndex;
		xPos += 80.f;
		fFontPosx += 80.f;
		if (iIndex >= 5)
		{
			yPos -= 80.f;
			fFontPosy += 80.f;
			xPos = -150.f;
			fFontPosx = 495.f;
		}

		if (m_iImageOption == INVEN_KNOLAN)
			m_Knolan_InvenImage.push_back(static_cast<CUI*>(pGameObjcet));
		else if(m_iImageOption == INVEN_GARRISON)
			m_Garrison_InvenImage.push_back(static_cast<CUI*>(pGameObjcet));
		else if (m_iImageOption == INVEN_CALIBRETTO)
			m_Calibretto_InvenImage.push_back(static_cast<CUI*>(pGameObjcet));
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CInventory::Owner_GARRISON_Set()
{
}

void CInventory::Owner_CALIBRETTO_Set()
{
}

void CInventory::Owner_KNOLAN_Tick(_double TimeDelta)
{
	if (m_iImageOption != INVEN_KNOLAN)
		return;

	for (auto& pChild : m_Knolan_InvenImage)
	{
		pChild->Set_RenderActive(true);
		pChild->Tick(TimeDelta);
	}

}

void CInventory::Owner_GARRISON_Tick(_double TimeDelta)
{
	if (m_iImageOption != INVEN_GARRISON)
		return;

	for (auto& pChild : m_Garrison_InvenImage)
	{
		pChild->Set_RenderActive(true);
		pChild->Tick(TimeDelta);
	}

}

void CInventory::Owner_CALIBRETTO_Tick(_double TimeDelta)
{
	if (m_iImageOption != INVEN_CALIBRETTO)
		return;
	for (auto& pChild : m_Calibretto_InvenImage)
	{
		pChild->Set_RenderActive(true);
		pChild->Tick(TimeDelta);
	}

}

void CInventory::Owner_KNOLAN_LateTick(_double TimeDelta)
{
	if (m_iImageOption != INVEN_KNOLAN)
		return;

	for (auto& pChild : m_Knolan_InvenImage)
	{
		pChild->Late_Tick(TimeDelta);
	}
}

void CInventory::Owner_GARRISON_LateTick(_double TimeDelta)
{
	if (m_iImageOption != INVEN_GARRISON)
		return;
	for (auto& pChild : m_Garrison_InvenImage)
	{
		pChild->Late_Tick(TimeDelta);
	}
}

void CInventory::Owner_CALIBRETTO_LateTick(_double TimeDelta)
{
	if (m_iImageOption != INVEN_CALIBRETTO)
		return;
	for (auto& pChild : m_Calibretto_InvenImage)
	{
		pChild->Late_Tick(TimeDelta);
	}
}

void CInventory::ICon_Button_Click()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
	{
		for (auto& pIConButton : m_mapIconButton)
		{
			if (static_cast<CInventory_Button*>(pIConButton.second)->Click_This_Button())
			{
				if (pIConButton.first == TEXT("Gully"))
				{
					m_iImageOption = INVEN_KNOLAN;
					m_iWeaponOption = IV_KNOLAN_W;
					Owner_KNOLAN_Set();

				}
				else if (pIConButton.first == TEXT("Garrison"))
				{
					m_iImageOption = INVEN_GARRISON;
					m_iWeaponOption = IV_GARRISON_W;
					Owner_KNOLAN_Set();
				
				}
				else if (pIConButton.first == TEXT("Calibretto"))
				{
					m_iImageOption = INVEN_CALIBRETTO;
					m_iWeaponOption = IV_CALIBRETTO_W;
					Owner_KNOLAN_Set();
				}
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CInventory::Cur_BigImageCast()
{
	for (auto pChild : m_ChildrenVec)
	{
		if (!lstrcmp(pChild->Get_ObjectName(),TEXT("Texture_Inventory_Big")))
		{
			static_cast<CMyImage*>(pChild)->Set_CurTextureNum(_uint(m_iImageOption));
		}
		if (!lstrcmp(pChild->Get_ObjectName(), TEXT("Inventory_CurWeaponImage")))
		{
			static_cast<CMyImage*>(pChild)->Set_CurTextureNum(_uint(m_iWeaponOption));
		}
	}

}

HRESULT CInventory::SetUp_Components()
{
	/*For.Com_Renderer */
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_CanvasDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventory::SetUp_ShaderResources()
{
	if(nullptr == m_pShaderCom)
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

CInventory * CInventory::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInventory*		pInstance = new CInventory(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInventory");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CInventory::Clone(void * pArg)
{
	CInventory*		pInstance = new CInventory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInventory");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInventory::Free()
{
	__super::Free();

	for (auto& pVec : m_Knolan_InvenImage)
		Safe_Release(pVec);
	m_Knolan_InvenImage.clear();

	for (auto& pVec : m_Garrison_InvenImage)
		Safe_Release(pVec);
	m_Garrison_InvenImage.clear();

	for (auto& pVec : m_Calibretto_InvenImage)
		Safe_Release(pVec);
	m_Calibretto_InvenImage.clear();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);


}
