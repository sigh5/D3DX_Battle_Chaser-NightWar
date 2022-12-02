#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube final : public CVIBuffer
{
protected:
	CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cube(const CVIBuffer_Cube& rhs);
	virtual ~CVIBuffer_Cube() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual	void	Final_Update()override {}

	const _float4*		Get_vPos() { return m_vPos; }
	virtual _bool	PickingBuffer(HWND hWnd, class CTransform * pCubeTransCom, _float4 &vPosition)override;

public:
	static CVIBuffer_Cube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;


private:
	_float4		*m_vPos = nullptr;

};

END