#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_SingleEffect final :public CVIBuffer
{
private:
	CVIBuffer_SingleEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_SingleEffect(const CVIBuffer_SingleEffect& rhs);
	virtual ~CVIBuffer_SingleEffect() = default;

public:
	virtual HRESULT Initialize_Prototype(_);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_double*				m_pSpeeds = nullptr;

public:
	static CVIBuffer_SingleEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
