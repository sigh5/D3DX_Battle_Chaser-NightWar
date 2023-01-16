#pragma once

#include "VIBuffer_Instancing.h"


/* 인스턴싱 주의 사항 알파 블렌딩이랑 인스턴싱이랑 같이 쓰지말자 */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instancing final : public CVIBuffer_Instancing
{
public:
	typedef struct VIBUffer_Point_TextureDesc
	{
		_float					m_iTextureMax_Width_Cnt = 0.f;		// 계속 증가해야됌
		_float					m_iTextureMax_Height_Cnt = 0.f;	// 계속 증가해야됌
		_int					m_iFrameCnt = 0;					// 시간 조절
	}VIBUffer_Point_TextureDesc;

private:
	CVIBuffer_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& rhs);
	virtual ~CVIBuffer_Point_Instancing() = default;

public:
	void			Set_FrameCnt(_int iFrameCnt) { m_Desc.m_iFrameCnt = iFrameCnt; }
	void			Set_TextureMax_Width_Cnt(_float iMaxWidth_Cnt) { m_Desc.m_iTextureMax_Width_Cnt = iMaxWidth_Cnt; }
	void			Set_TextureMax_Height_Cnt(_float iMaxHeight_Cnt) { m_Desc.m_iTextureMax_Height_Cnt = iMaxHeight_Cnt; }

	void			Set_WidthTextureCnt(_float fCurWidthCnt) { m_iWidthTextureCnt = fCurWidthCnt; }
	void			Set_HeightTextureCnt(_float fCurHeightCnt) { m_iHeightTextureCnt = fCurHeightCnt; }

	const			VIBUffer_Point_TextureDesc& Get_Point_TextureDesc()const { return m_Desc; }
	void			Set_Point_Desc(VIBUffer_Point_TextureDesc& Desc)
	{
		memcpy(&m_Desc, &Desc, sizeof(m_Desc));
	}

	void			Set_Point_Instancing_Scale(_float3 vScale);
	void			Set_Point_Instancing_Num(_int iInstancingNum);
	void			Set_Point_Instancing_MainTain();

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(_double TimeDelta) override;
	virtual HRESULT Render();


	HRESULT Tick_Shaking(_double TimeDelta,_float fShakex);

	/*For.Imgui*/
	virtual		void	Imgui_RenderProperty()override;


	void				Move_Up_Position(_float4 vPos);
	void				Move_Up_Tick(_double TimeDelta, _float fSpeed);
	_bool				UV_Move_Tick(_double TimeDelta);
	HRESULT				Set_UV_RawValue(class CShader* pShader);

private:
	_double*				m_pSpeeds = nullptr;
	_uint					m_iPlayOnFrameCnt = 0;		//속도 조절 m_iPlayOnFrameCnt ==5 이면  5/60 틱에 한번 도는거임 

	_float					m_iWidthTextureCnt = 0.f;		// 계속 증가해야됌
	_float					m_iHeightTextureCnt = 0.f;	// 계속 증가해야됌
	_bool					m_bIsMaintain = false;



	VIBUffer_Point_TextureDesc m_Desc;
	
	/*For.Imgui*/
	int						iPlayOnFrameCnt		= 	0;
	float					iTextureMax_Width_Cnt	= 	0;
	float					iTextureMax_Height_Cnt	= 	0;
	

public:
	static CVIBuffer_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END