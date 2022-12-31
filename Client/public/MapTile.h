#pragma once

#include "Client_Defines.h"
#include "Environment_Object.h"
#include <set>

BEGIN(Engine)
class CShader;
class CRenderer;
class CVI_Buffer_Terrain;
class CTexture;
class CNavigation;
END

BEGIN(Client)
class CMapTile  final : public CEnvironment_Object
{
private:
	CMapTile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapTile(const CMapTile& rhs);
	virtual ~CMapTile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private: /* for_ FilterMap*/
	HRESULT	Ready_FilterBuffer();
	HRESULT	Ready_BufferLock_UnLock();

private: /* for_ FilterMap*/
	_ulong*						m_pPixel = nullptr;
	ID3D11Texture2D*			pTexture2D = nullptr;
	D3D11_TEXTURE2D_DESC		TextureDesc;
	std::set<_ulong>			m_FilterIndexSet;

private: /* for_ Brush*/
	_float4						m_vBrushPos;
	

	void						Test_Instancing();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVI_Buffer_Terrain*		m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom[TYPE_END] = { nullptr, };
	CNavigation*			m_pNavigationCom = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMapTile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};


END