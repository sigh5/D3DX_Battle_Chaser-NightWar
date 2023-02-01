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
class CModel;
END

BEGIN(Client)
class CTraile_Effect_Child final : public CHitBoxObject
{

public:
	typedef struct tag_Trail_Child_Pos
	{
		_vector		m_Trail_Pos[4];
		_int		iTextureNum = 0;
		_float		fFixRatio = 0.1f;
		_int		ColorTexture0Num = 0;
		_int		ColorTexture1Num = 0;
	}Trail_Child_Pos_Desc;

public:
	CTraile_Effect_Child(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTraile_Effect_Child(const CTraile_Effect_Child& rhs);
	virtual ~CTraile_Effect_Child() = default;

public:

	void	Set_ChildDesc(Trail_Child_Pos_Desc& Desc);

	const	_bool	Get_IsFinish()const { return m_bIsFinsishBuffer; }
	void			Set_TextureNum(_int iTextureNum){
		m_iTextureNum = iTextureNum;
	}
	void			Set_RenderActive(_bool m_bRender) { m_bIsRenderTrue = m_bRender; }
	void			Set_UVXRatio(_float fRatio) {
		m_UVXRatioX = fRatio;
	}
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Last_Initialize()override;

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	CVIBuffer_Point_Instancing*			m_pVIBufferCom = nullptr;
	CTexture*							m_pTextureCom = nullptr;
	CTexture*							m_pColorTextureCom = nullptr;
	CTexture*							m_pColorTextureCom2 = nullptr;
private:
	Trail_Child_Pos_Desc				m_Desc;

	_bool								m_bIsFinsishBuffer = false;
	_bool								m_bIsRenderTrue = false;

	_int								m_iCenterCount = 0;
	_int								m_iTextureNum = 0;
	
	_int								m_iCUrCOunt = 0;
	_uint								m_iShaderPass = 0;
	_float								m_fTrailTimer = 0.f;

	_float								m_fRadius = 0.f;

	_float								m_UVXRatioX = 1.f;
	_float								m_UVFixRatio = 0.1f;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTraile_Effect_Child* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
