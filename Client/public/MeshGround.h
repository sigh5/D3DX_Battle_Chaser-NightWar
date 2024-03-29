#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CBone;
class CTexture;
class CModel;
END

BEGIN(Client)
class CMeshGround  final : public CGameObject
{
public:
	typedef struct tag_GroundModel
	{
		_tchar	Name[MAX_PATH] = TEXT("");
		_tchar  TextureName[MAX_PATH] = TEXT("");
		_int	iTextureIndex;
		_uint	iShaderPass;
	}Ground_ModelDesc;


private:
	CMeshGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshGround(const CMeshGround& rhs);
	virtual ~CMeshGround() = default;

public:
	void	Set_MeshGroundRender_Active(_bool bRenderActive) { m_bRenderActive = bRenderActive; }

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
	CModel*								m_pModelCom = nullptr;
	CTexture*							m_pTextureCom = nullptr;


private:
	Ground_ModelDesc					m_GroundModelDesc;
	_bool								m_bRenderActive = true;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();


private:
	_int								m_iPlayOnFrameCnt = 0;
	_int								m_iTextureNum = 0;
public:
	static CMeshGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END