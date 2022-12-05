#pragma once
#pragma once

#include "VIBuffer.h"

BEGIN(Engine)


class ENGINE_DLL CVI_Buffer_Terrain final : public CVIBuffer
{
protected:
	CVI_Buffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVI_Buffer_Terrain(const CVI_Buffer_Terrain& rhs);
	virtual ~CVI_Buffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& terrainFilePath);
	virtual HRESULT Initialize(void* pArg);

	_float4				PickingTerrain(HWND hWnd, class CTransform * pCubeTransCom);
	virtual		void	Final_Update()override {}
	
	virtual  _bool		PickingBuffer(HWND hWnd, class CTransform * pCubeTransCom,_float4 &vPosition)override;
	

	_bool				PickingRetrunIndex(HWND hWnd, class CTransform * pCubeTransCom, _float4& fIndexs);
			

private:
	_uint		m_iNumVerticesX = 0;
	_uint		m_iNumVerticesZ = 0;
	_float4*	m_pVtx = nullptr;


public:
	static CVI_Buffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& terrainFilePath);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END