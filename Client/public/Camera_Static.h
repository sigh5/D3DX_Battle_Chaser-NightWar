#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CPlayer;

END

BEGIN(Client)

class CCamera_Static : public CCamera
{
private:
	CCamera_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Static(const CCamera_Static& rhs);
	virtual ~CCamera_Static() = default;

public:
	const _float	Get_CameraZ()const { return m_CameraEyeZ; }
	void			Set_CameraZ(_float fCamZ) { m_CameraEyeZ = fCamZ; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Last_Initialize()override;
	virtual HRESULT Render() override;

	virtual void	Set_CameraActive(_bool bCameraActive)override;
	



private:
	HRESULT SetUp_Components();
	HRESULT Update_CameraLookPos(_double TimeDelta);

	void	NormalCameraActive();
	void	Lerp_CameraActive(_float LerpTime,CPlayer* pCurCaptin);

private:
	CPlayer*				m_pCurTaget = nullptr;


private:
	_float					m_CameraDistanceX = 0.f;
	_float					m_CameraEyeY = 0.f;
	_float					m_CameraEyeZ = 0.f;

	
					

	_float					m_fLerpTimer = 0.0f;
	_float					m_fLerpTime = 0.3f;
	_bool					m_bLerp = false;


public:
	static CCamera_Static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END