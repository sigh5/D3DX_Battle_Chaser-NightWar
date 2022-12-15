#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel :public CComponent
{
public:
	enum LOAD_TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

	typedef struct tag_LoadModelDesc
	{
		_tchar* szProtoName = TEXT("");
	}LOAD_MODELDESC;


private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	void	Set_NoRenderMeshName(const char* pBoneConstantName) {strcpy_s(m_szNoRenderMeshName, MAX_PATH, pBoneConstantName);}
	
public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	_matrix Get_PivotMatrix() const { return XMLoadFloat4x4(&m_PivotMatrix); }
	class CBone* Get_BonePtr(const char* pBoneName);
	
public:
	virtual HRESULT Initialize_Prototype(LOAD_TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile);
	virtual HRESULT Initialize(void* pArg);

public:
	void Play_Animation(_double TimeDelta,_bool IsCombat=false);


	HRESULT Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);
	HRESULT Render(CShader* pShader, _uint iMeshIndex, _uint iShaderIndex = 0, const char* pBoneConstantName = nullptr, const char* pNoRenderName = nullptr);
	
	_bool	 Get_Finished(_uint iAnimIndex);
	void	Set_Finished(_uint iAnimIndex,_bool bFinish);
	
	
public: /*For.Imgui*/
	virtual void Imgui_RenderProperty() override;


public: /*For.Animation*/
	void	Set_AnimIndex(_uint iAnimIndex);
	void	Set_AnimTickTime(_double TickTime);
	_uint	Get_AnimIndex() const { return m_iCurrentAnimIndex; }
	void	Set_AnimName(char* pAnimName);
	
	void	Set_Duration(_uint iAnimIndex,_double Ratio=1.f);
	void	Set_PlayTime(_uint iAnimIndex);
	void	InitChannel();
	_bool	Control_KeyFrame(_uint iAnimIndex,_uint KeyBegin, _uint KeyEnd);


public:
	void	Set_Lerp(_uint iprevIndex, _uint iNextIndex,_double TimeDelta, _bool bIsLerp);

private:
	LOAD_TYPE							m_eType = TYPE_END;
	_uint								m_iNumMeshes = 0;
	vector<class CMesh*>				m_Meshes;

	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>				m_Materials;

	_uint								m_iNumBones = 0;
	vector<class CBone*>				m_Bones;

	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;

	LOAD_MODELDESC						m_ModelDesc;
	_float4x4							m_PivotMatrix;

	
private: /*For.Imgui*/
	_bool								m_bAnimcontrol = false;
	int									m_iMeshIndex = -1;
	_double								m_iTickPerSecond = 0.0;
	char								m_imguiAnimName[MAX_PATH] = "";
	_uint								m_iNoRenderIndex = 999;

private:
	char								m_szModelPath[MAX_PATH] = "";
	char								m_szNoRenderMeshName[MAX_PATH] = "";
	
	class CBone*						m_pCameraBone = nullptr;


public:
	HRESULT Ready_Bones(HANDLE hFile, class CBone* pParent);
	HRESULT Ready_MeshContainers(HANDLE hFile, LOAD_TYPE eType);
	HRESULT Ready_Materials(HANDLE hFile, const char* pModelFilePath, LOAD_TYPE eType);
	HRESULT Ready_Animation(HANDLE hFile);
	HRESULT	Ready_CameraBone();


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LOAD_TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END