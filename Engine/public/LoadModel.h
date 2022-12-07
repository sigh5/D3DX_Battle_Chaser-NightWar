#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CLoadModel :public CComponent
{
public:
	enum LOAD_TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

	typedef struct tag_LoadModelDesc
	{
		_tchar* szProtoName = TEXT("");
	}LOAD_MODELDESC;


private:
	CLoadModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadModel(const CLoadModel& rhs);
	virtual ~CLoadModel() = default;

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	_matrix Get_PivotMatrix() const { return XMLoadFloat4x4(&m_PivotMatrix); }
	class CLoadBone* Get_BonePtr(const char* pBoneName);
	void Set_AnimIndex(_uint iAnimIndex) { m_iCurrentAnimIndex = iAnimIndex; }
public:
	virtual HRESULT Initialize_Prototype(LOAD_TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile);
	virtual HRESULT Initialize(void* pArg);

public:
	void Play_Animation(_double TimeDelta);
	HRESULT Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);
	HRESULT Render(CShader* pShader, _uint iMeshIndex, _uint iShaderIndex = 0, const char* pBoneConstantName = nullptr);

private:
	LOAD_TYPE							m_eType = TYPE_END;
	_uint								m_iNumMeshes = 0;
	vector<class CLoadMesh*>			m_Meshes;

	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>				m_Materials;
	
	_uint								m_iNumBones = 0;
	vector<class CLoadBone*>			m_Bones;

	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iNumAnimations = 0;
	vector<class CLoadAnimation*>		m_Animations;

	LOAD_MODELDESC						m_ModelDesc;
	_float4x4							m_PivotMatrix;
	char								m_szModelPath[MAX_PATH] = "";
public:
	HRESULT Ready_Bones(HANDLE hFile,class CBone* pParent);
	HRESULT Ready_MeshContainers(HANDLE hFile);
	HRESULT Ready_Materials(HANDLE hFile,const char* pModelFilePath);
	HRESULT Ready_Animation(HANDLE hFile);

public:
	static CLoadModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LOAD_TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix,HANDLE hFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END