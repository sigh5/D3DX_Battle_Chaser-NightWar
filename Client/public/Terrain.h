#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include <set>
BEGIN(Engine)
class CShader;
class CRenderer;
class CVI_Buffer_Terrain;
class CTexture;
END

BEGIN(Client)

typedef struct tag_Create_object
{
	_tchar* szLayerName = TEXT("");
	_tchar* szProtoName = TEXT("");
	_tchar* szTextureName = TEXT("");
	_tchar* szObjectName = TEXT("");
	_tchar* szModelName = TEXT("");
	_uint	iShaderPass = 0;

}Create_OBJECTDESC;


class CTerrain final : public CGameObject
{
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	
public:	/*For_Imgui*/
	_float4			Get_Position()const ;
	void			Set_MapObject(Create_OBJECTDESC& pArg);
	virtual _bool	Piciking_GameObject()override;
	void			Create_Object();
	void			Set_CreateObject(_bool bCreateObject) {m_bCreateObject = bCreateObject;}


private: /* for_ FilterMap*/
	HRESULT	Ready_FilterBuffer();
	HRESULT	Ready_BufferLock_UnLock();

	_ulong*		m_pPixel= nullptr;
	ID3D11Texture2D*			pTexture2D = nullptr;
	D3D11_TEXTURE2D_DESC		TextureDesc;

	std::set<_ulong>   temp;

	

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVI_Buffer_Terrain*		m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom[TYPE_END] = { nullptr, };

	_bool					m_bCreateObject = false;
	_bool					m_bCreateCheck = false;


	Create_OBJECTDESC		m_ObjData;
	_uint					m_iObjNameNumber = 0;

	vector<_tchar*>			m_NameVector;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	
public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};


END

