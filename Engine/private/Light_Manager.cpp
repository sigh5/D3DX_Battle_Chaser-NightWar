#include "..\public\Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}


LIGHTDESC * CLight_Manager::Get_LightDesc(wstring NameTag) 
{
	
	CLight* pLight = Find_Light(NameTag);

	if (nullptr == pLight)
		assert(! "CLight_Manager::Get_LightDesc");

	return pLight->Get_LightDesc();
}

//HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
//{
//	CLight*		pLight = CLight::Create(pDevice, pContext, LightDesc);
//	if (nullptr == pLight)
//		return E_FAIL;
//
//	m_Lights.push_back(pLight);
//
//	return S_OK;
//}

void CLight_Manager::Render_Light(CVIBuffer_Rect * pVIBuffer, CShader * pShader)
{
	/*for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight)
			pLight->Render(pVIBuffer, pShader);
	}*/

	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight.second)
			pLight.second->Render(pVIBuffer, pShader);
		if (nullptr == pLight.second)
			_bool b = false;
	}
}

void CLight_Manager::Clear()
{
	for (auto &iter : m_Lights)
		Safe_Release(iter.second);

	m_Lights.clear();
}

HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, wstring NameTag, const LIGHTDESC & LightDesc)
{
	if (nullptr != Find_Light(NameTag))
		assert(! "CLight_Manager::Add_Light");

	CLight*		pLight = CLight::Create(pDevice, pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.emplace(NameTag,pLight);

	return S_OK;
}

HRESULT CLight_Manager::Delete_Light(wstring NameTag)
{
	CLight*		pLight = Find_Light(NameTag);

	if (nullptr == pLight)
		assert(!"CLight_Manager::Delete_Light");

	Safe_Release(pLight);
	m_Lights.erase(NameTag);

	return S_OK;
}

CLight * CLight_Manager::Find_Light(wstring nameTag)
{
	auto Pair = find_if(m_Lights.begin(), m_Lights.end(), [&](auto &MyPair)->bool
	{
		if (MyPair.first == nameTag)
			return true;

		return false;
	});

	if (Pair != m_Lights.end())
		return Pair->second;

	return nullptr;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight.second);

	m_Lights.clear();
}
