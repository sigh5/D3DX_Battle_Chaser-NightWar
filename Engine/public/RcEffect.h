#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcEffect final: public CVIBuffer
{
public:
	typedef	struct tag_RcEffectDesc
	{
		_float2		_Uv_0;
		_float2		_Uv_1;
		_float2		_Uv_2;
		_float2		_Uv_3;
	}RC_EFFECT_DESC;



protected:
	CRcEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRcEffect(const CRcEffect& rhs);
	virtual ~CRcEffect() = default;

public:
	void Set_Texture(class CTexture* pTexture);
	void Set_TextureOption(_uint _framecnt, _uint _texturecntw, _uint _texturecnth);

	_bool Check_TextureCnt(_uint _widthtexturecnt, _uint heighttexturecnt);
	void Check_Alpha(_bool _TrueIsAlphaOn);

	const _uint& Get_WidthTextureCnt(void);
	const _uint& Get_HeightTextureCnt(void);


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);



public:
	_matrix					 m_matWorld;
	_matrix					 m_ViewMatrix;
	_matrix					 m_PrjMatrix;
	_float4					 m_WorldCamPos;
	_float4					 m_WorldLightPos{ 100.f, 0.f, 100.f, 1.f };

private:
	class CTexture*		m_pTexture = nullptr;
	_bool m_bIsAlpha = false;
	_uint m_iPlayOnFrameCnt = 0;
	_uint m_iWidthTextureCnt = 0;
	_uint m_iHeightTextureCnt = 0;

	_uint m_iFrameCnt = 0;
	_uint m_iTextureCnt_W = 0;
	_uint m_iTextureCnt_H = 0;

	_float m_fUSize = 0.f;
	_float m_fVSize = 0.f;

	RC_EFFECT_DESC			m_EffectDesc;


public:
	static CRcEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END