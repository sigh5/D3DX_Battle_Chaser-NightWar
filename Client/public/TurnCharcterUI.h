#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Client_Manager.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CTurnCharcterUI final : public CUI
{
private:
	CTurnCharcterUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTurnCharcterUI(const CTurnCharcterUI& rhs);
	virtual ~CTurnCharcterUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Change_Texture(_uint iLevel, const wstring& NewComPonentTag);
public: /*Observer Pattern*/
	
	void	IsMove();
		
	void	Top_Move();
	void	Bottom_Move() ;
	void	Normal_Move();

	void	MoveControl(_uint iOption);

	_bool	isUITop(OUT CUI*& pUI);
	_bool	isUIBottom(OUT CUI*& pUI);
	_float	Get_PosY();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	_float4x4				m_ViewMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

	_bool					m_bIsBottom = false;
	_bool					m_bMove = false;
	_float4					m_vLimitPos;
	


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTurnCharcterUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	std::function<void()> m_Movefun;
	
};

END
