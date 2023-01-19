#pragma once
#include "Canvas.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class CStatus;
END

BEGIN(Client)
class CInventory final: public CCanvas
{
	enum InventoryOwner {INVEN_KNOLAN =18 , INVEN_GARRISON =17 , INVEN_CALIBRETTO =16, INVEN_END=0};
	enum InventoryOwnerWeapon { IV_KNOLAN_W = 10, IV_GARRISON_W = 9, IV_CALIBRETTO_W = 11, IV_END = 0 };
	enum InventoryITemTexutureNum	{ IVT_MP = 7, IVT_HP = 8 ,IVT_BOOK = 12, IVT_END};

private:
	CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventory(const CInventory& rhs);
	virtual ~CInventory() = default;

public:
	void	Set_Knolan(CGameObject* pPlayer);
	void	Set_Garrison(CGameObject* pPlayer);
	void	Set_Calibretto(CGameObject* pPlayer);



public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public:
	virtual void	Set_RenderActive(_bool bTrue)override;
	
	void			Clear_InventoryImage();


private:
	void		   Owner_KNOLAN_Set();
	void		   Owner_GARRISON_Set();
	void		   Owner_CALIBRETTO_Set();


public:
	void		   Owner_KNOLAN_Tick(_double TimeDelta);
	void		   Owner_GARRISON_Tick(_double TimeDelta);
	void		   Owner_CALIBRETTO_Tick(_double TimeDelta);

public:
	void		   Owner_KNOLAN_LateTick(_double TimeDelta);
	void		   Owner_GARRISON_LateTick(_double TimeDelta);
	void		   Owner_CALIBRETTO_LateTick(_double TimeDelta);


private:
	void			ICon_Button_Click();
	void			Cur_BigImageCast();



private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	map<wstring, CUI*>			m_mapIconButton;
	CStatus*					m_pKnolan_Status = nullptr;
	CStatus*					m_pGarrison_Status = nullptr;
	CStatus*					m_pCalibretto_Status = nullptr;

	vector<CUI*>				m_Knolan_InvenImage;
	vector<CUI*>				m_Garrison_InvenImage;
	vector<CUI*>				m_Calibretto_InvenImage;
private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix;
	_bool					m_bLoseRender = false;
	InventoryOwner			m_iImageOption = INVEN_KNOLAN;	//18 Gully 17 Garrison 16 Calibretto
	InventoryOwnerWeapon	m_iWeaponOption = IV_KNOLAN_W;

	


	

public:
	static CInventory* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;


};
END
