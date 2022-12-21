#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instancing final : public CVIBuffer_Instancing
{
public:
	CVIBuffer_Rect_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_Instancing(const CVIBuffer_Rect_Instancing& rhs);
	virtual ~CVIBuffer_Rect_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg);
	virtual	HRESULT	Tick(_double TimeDelta);
	virtual HRESULT Render();

private:
	_double*		m_pSpeeds = nullptr;


public:
	static CVIBuffer_Rect_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END



