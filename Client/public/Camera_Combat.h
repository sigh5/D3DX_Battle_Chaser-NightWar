#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)


END

BEGIN(Client)
class CCamera_Combat final: public CCamera
{
private:
	CCamera_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Combat(const CCamera_Combat& rhs);
	virtual ~CCamera_Combat() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Last_Initialize()override;
	virtual HRESULT Render() override;

	void	Camera_Shaking();
	void	Camera_ZoomIn_CurActor();
private:
	HRESULT SetUp_Components();

private:
	_float					m_CameraDistanceX = 0.f;
	_float					m_CameraEyeY = 0.f;
	_float					m_CameraEyeZ = 0.f;


public:
	static CCamera_Combat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};


END