#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBUffer_Sphere final :public CVIBuffer
{
private:
	explicit CVIBUffer_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBUffer_Sphere(const CVIBUffer_Sphere& rhs);
	virtual ~CVIBUffer_Sphere() =default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual		void	Final_Update()override {}
	virtual  _bool		PickingBuffer(HWND hWnd, class CTransform * pCubeTransCom, _float4 *vPosition)override;

public:
	static CVIBUffer_Sphere*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr)override;
	virtual void Free(void) override;

};

END