#pragma once

#include "HitBoxObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CModel;
class CShader;
class CBone;
END

BEGIN(Client)
class CSkill_Object final: public CHitBoxObject
{
public:
	enum Skill_DIR {
		Skill_DIR_straight, Skill_DIR_LISING, Skill_DIR_DOWN, Skill_DIR_ScaleUP_DOWN, Skill_DIR_END
	};

	typedef struct tagSkill_OBJ_Desc
	{
		CTransform*			ParentTransform;
		_float4x4			PivotMatrix;			//생성위치를 알기위해서
		CBone*				pSocket;				//생성위치를 알기위해서
		_tchar				pModelTag[MAX_PATH];	//모델 태그
		CTransform*			pTraget;			// 스킬이 가는 위치
		_float4				vPosition;
		_float				vAngle;		// 스킬의 크기
		
		WeaponType			eType;
		Skill_DIR			eDir;

	}SKILL_OBJ_DESC;


private:
	CSkill_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Object(const CSkill_Object& rhs);
	virtual ~CSkill_Object() = default;

public:
	virtual	 WeaponType		Get_Type()const override { return m_SkillDesc.eType; }
	virtual	 CCollider*		Get_Colider() override { return m_pColliderCom; }

public:
	const	_bool			Get_IsTargetHit()const {return m_IsHitTarget;}

	void					Set_Shoot(_bool bShoot) { m_bShoot = bShoot; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Last_Initialize()override;

private:
	CShader*				m_pShaderCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;


private:
	void					Rising_Move_Start(_double TimeDelta);
	void					ScaleUP_Down_Move_Start(_double TimeDelta);


private:
	SKILL_OBJ_DESC			m_SkillDesc;
	_bool					m_IsHitTarget = false;
	_bool					m_bShoot = false;
	_float4x4				m_SocketMatrix;
	_matrix					m_OriginMatrix;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CSkill_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};


END