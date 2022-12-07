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
	void	Set_CurrentCameraTargetTag(const wstring& ChaseTargetTag) {m_ChaseTargetTag = ChaseTargetTag;}

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
	HRESULT Update_CameraLookPos();
private:
	list<pair<const wstring ,CGameObject*>>	m_TargetList;

private:
	_float					m_CameraDistanceY = 5.f;
	_float					m_CameraDistanceZ = 5.f;
	wstring					m_ChaseTargetTag = TEXT("");
	CGameObject*			m_pCurrentTarget = nullptr;
public:
	static CCamera_Static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END