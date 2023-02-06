#include "..\public\Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
	_vector		vLightEye = XMVectorSet(-10.f, 30.f, -10.f, 1.f);
	_vector		vLightAt = XMVectorSet(100.f, 0.f, 100.f, 0.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_matrix		LightViewMatrix;
	LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);

	XMStoreFloat4x4(&m_LightViewMatrix, LightViewMatrix);

	_matrix		LightProjMatrix;
	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), _float(1280) / _float(720), 0.2f, 300.f);

	XMStoreFloat4x4(&m_LightProjMatrix, LightProjMatrix);
}


LIGHTDESC * CLight_Manager::Get_LightDesc(wstring NameTag) 
{
	
	CLight* pLight = Find_Light(NameTag);

	if (nullptr == pLight)
		assert(! "CLight_Manager::Get_LightDesc");

	return pLight->Get_LightDesc();
}

void CLight_Manager::Set_LightView_Matrirx(wstring nameTag,_float4x4 fMatrix)
{
	/*CLight* pLight = Find_Light(nameTag);
	assert(nullptr != pLight && "CLight_Manager::Set_LightView_Matrirx");

	pLight->Set_LightView_Matrirx(fMatrix);*/

	m_LightViewMatrix = fMatrix;

}

void CLight_Manager::Set_LightProj_Matrirx(wstring nameTag, _float4x4 fMatrix)
{
	/*CLight* pLight = Find_Light(nameTag);
	assert(nullptr != pLight && "CLight_Manager::Set_LightProj_Matrirx");

	pLight->Set_LightProj_Matrirx(fMatrix);*/

	m_LightProjMatrix = fMatrix;
}

_float4x4 CLight_Manager::Get_Light_Matrix(const wstring nameTag) 
{
	/*CLight* pLight = Find_Light(nameTag);
	assert(nullptr != pLight && "CLight_Manager::Get_Light_Matrix");
	return pLight->Get_Light_Matrix();*/

	//_vector		vLightEye = XMVectorSet(-1.f, 10.f, -1.f, 1.f);
	//_vector		vLightAt = XMVectorSet(100.f, 0.f, 100.f, 0.f);
	//_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	//_matrix		LightViewMatrix;
	//LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);

	//;
	//XMStoreFloat4x4(&m_LightViewMatrix, LightViewMatrix);

	return m_LightViewMatrix;
}

_float4x4 CLight_Manager::Get_Light_ProjMatrix(wstring nameTag)
{
	/*CLight* pLight = Find_Light(nameTag);
	assert(nullptr != pLight && "CLight_Manager::Get_Light_ProjMatrix");
	return pLight->Get_Light_ProjMatrix();*/
	//_matrix		LightProjMatrix;
	//LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), _float(1280) / _float(720), 0.2f, 1000.f);

	//
	//XMStoreFloat4x4(&m_LightProjMatrix, LightProjMatrix);

	return m_LightProjMatrix;
	
}

_float4x4 CLight_Manager::Test1()
{
	return m_LightViewMatrix;
}

_float4x4 CLight_Manager::Test2()
{
	/*_matrix		LightProjMatrix;
	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), _float(1280) / _float(720), 0.2f, 300.f);

	XMStoreFloat4x4(&m_LightProjMatrix, LightProjMatrix);*/

	return m_LightProjMatrix;
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
