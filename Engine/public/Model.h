#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final:public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint	Get_MeshNum()const { return m_iMeshNum; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg);

public:
	_bool			PicikingModel(HWND hWnd, class CTransform* pTransform);

public:
	HRESULT	Bind_Material(class CShader* pShader, _uint iMeshIndex, aiTextureType eType, const char* pConstantName);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);
public:
	const  aiScene*						m_pAIScene = nullptr;
	Assimp::Importer					m_Importer;
	TYPE								m_eType = TYPE_END;
	
	_uint								m_iMeshNum = 0;
	vector<class CMesh*>				m_Meshs;

	_uint								m_iNumMaterials;	// 모델의 메터리얼 숫자
	vector<MODELMATERIAL>				m_Materials;		// 

public:
	HRESULT		Ready_MeshContainers();
	HRESULT		Ready_Materials(const char* pModelFilePath);


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END