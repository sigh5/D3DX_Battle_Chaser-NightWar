#pragma once

#include "Shader.h"
#include "Renderer.h"
#include "VIBuffer_Rect.h"
#include "VI_Buffer_Terrain.h"
#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "Model.h"
#include "FSMComponent.h"
/* 다양한 컴포넌트(원형)들을 담고 있는다. -> 사본컴포넌트들은 사용해야할 객체들이 보유한다. */
/* 사본을 생성하여 리턴한다. */
BEGIN(Engine)

enum COMPONENT_TYPE { TYPE_TEXTURE, TYPE_MODEL, TYPE_END };

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)

public:

private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void* pArg = nullptr);



public: /*for_Imgui*/
	void		Imgui_ComponentViewer(_uint iLevel, OUT wstring& TextureTag, COMPONENT_TYPE eType);
	void		Remove_ProtoComponent(_uint iLevel, const wstring& pComponentName);


private:
	void		Imgui_TextureViewer(_uint iLevel,OUT wstring& TextureTag);
	void		Imgui_ModelViewer(_uint iLevel, OUT wstring& Model_NoAnimTag);


private:
	_uint											m_iNumLevels = 0;

	/* 레벨별로 원형 컴포넌트(텍스쳐, 메시모델 etc )들을 보관하겠다. */
	map<const wstring, class CComponent*>*			m_pPrototypes = nullptr;
	typedef map<const wstring, class CComponent*>	PROTOTYPES;

private:
	CComponent* Find_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag);


public:
	virtual void Free() override;
};

END