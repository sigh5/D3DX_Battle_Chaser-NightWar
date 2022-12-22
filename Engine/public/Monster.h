#pragma once
#include "CombatActors.h"
#include "BaseDelegater.h"		// 애도 체력있어서 알려줘야함

BEGIN(Engine)

class ENGINE_DLL CMonster abstract :public CCombatActors
{
protected:
	CMonster(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual		_bool	IsCollMouse() = 0;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public: /*For.Imgui*/
	virtual _bool Piciking_GameObject() { return false; }

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END