#pragma once
#include "HitBoxObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Point_Instancing;
class CShader;
class CBone;
class CTexture;
class CCollider;
END

BEGIN(Client)
class CTraile_Effect_Child;

class CTrail_Effect final : public CHitBoxObject
{
public:
	enum TRAIL_OPTION {
		GARRISON_NORMAL, GARRISON_SKILL1, GARRISON_ULTIMATE, BOSS_WHIP,TRAIL_OPTION_END
	};


	typedef struct  tag_Trail_Effect
	{
		CCollider* pColider;
		CGameObject* pGameObject;
		TRAIL_OPTION	eType;
		_int			iDevideFixNum;
		_int			iMaxCenterCount;
		_float			fSize;
	}tag_Trail_Effect_DESC;


public:
	CTrail_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Effect(const CTrail_Effect& rhs);
	virtual ~CTrail_Effect() = default;
public:
	const	_bool	Get_IsFinish()const { return m_bIsFinsishBuffer; }
	void			Set_Desc(tag_Trail_Effect_DESC& Desc,_int iTextureNum, _int iCurPointNum);
	void			Set_RenderStop() { m_bIsTrailStartCheck = false; }
	void			First_Edition();
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Last_Initialize()override;


public:
	void	Garrison_Normal_TrailTick();
	void	Garrison_Skill1_TrailTick();
	void	Garrison_Boss_TrailTick();
	
private:
	vector<class CTraile_Effect_Child*>	m_TrailEffectChild;


private:
	_bool								m_bIsFinsishBuffer = false;
	_bool								m_bIsTrailStartCheck = false;
	
	_float4x4							m_viewMatrix;

	tag_Trail_Effect_DESC				m_Desc;
	
	_float3								m_vOldCenter;
	_float3								m_vNewCenter;
	_int								m_iCenterCount = 0;
	_int								m_iTextureNum = 0;

	_vector								m_Trail_Pos[4];
	_vector								m_Collider_pos[100];
	
	_vector								m_Colider_Trail_Pos[100];
	_int								m_iTraillPos_Index = 0;
	_int								m_iDevide_Lerp_Num = 0;
	_int								m_CurPointNum = 30;


	_int								m_iCUrCOunt = 0;
	_uint								m_iShaderPass = 0;
	_float								m_fTrailTimer = 0.f;
	_vector								m_vFirstPos;
	_bool								m_bTrail_Maintain = false;
	_bool								m_bTraill_RealRender = false;
	_float								m_fRadius = 0.f;
	_bool								m_bIsTestBool = false;


	_bool								m_bIsImguiToolcheck = false;
	_vector								 vLook;
	_bool								m_bDeleteCheck = false;
	


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTrail_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END