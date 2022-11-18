#pragma once
#include "GameObject.h"
#include "BaseDelegater.h"

BEGIN(Engine)

class ENGINE_DLL CPlayer abstract: public CGameObject
{
public:
	typedef struct tag_PlayerDesc :GAMEOBJECTDESC
	{

	}PLAYERDESC;

protected:
	CPlayer(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;


public: /* imgui */
	virtual void Imgui_RenderComponentProperties() {}
	virtual void Imgui_RenderProperty()override {}

protected:
	PLAYERDESC				m_PlayerDesc;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;


};

END