#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)


END

BEGIN(Client)
class CCamera_Combat final: public CCamera
{
public:
	enum	CameraTarget {
		CameraTarget_CurActor, CameraTarget_Hiter, CameraTarget_recover, CameraTarget_END};

private:
	CCamera_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Combat(const CCamera_Combat& rhs);
	virtual ~CCamera_Combat() = default;

public:
	void	Set_ZoomMoveOption(_int iZoomMove) {
		m_bZoomMoveOption = (CameraTarget)(iZoomMove);
		m_LeprFov = 0.f;
	}

	void	Set_CameraShaking_Active(_bool bCameraShaking) { m_bCameraShakingActive = bCameraShaking; }
	void	Ready_CameraShaking(_float fShakeTime, _float fMagnitude)
	{
		m_fCurShakeTime = 0.f;
		m_fShakeTime = fShakeTime;
		m_fMagnitude = fMagnitude;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Last_Initialize()override;
	virtual HRESULT Render() override;


	void	Camera_ZoomIn_CurActor(_double TimeDelta);
	void	Camera_ZoomIn_HitActor(_double TimeDelta);
	void	Camera_ZoomOut(_double TimeDelta);




private:
	HRESULT SetUp_Components();
	void	Camera_Shaking(_double TimeDelta);

private:
	_float					m_CameraDistanceX = 0.f;
	_float					m_CameraEyeY = 0.f;
	_float					m_CameraEyeZ = 0.f;

	_bool					m_bCusorFix = false;
	_float					m_fRatioTimer = 0.f;

	CameraTarget			m_bZoomMoveOption = CameraTarget_END;			//0 CurActor 1 Hiter 2: recover
	_float					m_fZoomInFov = 57.f;
	_float					m_fFixFov = 60.f;
	_float					m_LeprFov = 0.f;


	_bool					m_bZoomHiter = false;
	_float					m_fZoomActor_to_Hiter_Timer = 0.f;
	
	_float					m_fZoomHiter_to_Actor_Timer = 0.f;
	_bool					m_bZoomFinish = false;
	_float4					m_vOriginPos;

	// shake
	_float m_fShakeTime=0.f;
	_float m_fCurShakeTime=0.f;
	_float m_fMagnitude=0.f;
	// shake
	_float4x4				m_WorldMat;


	_bool					m_bCameraShakingActive = false;

public:
	static CCamera_Combat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};


END