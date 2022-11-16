#pragma once

#include "Shader.h"
#include "Renderer.h"
#include "VIBuffer_Rect.h"
#include "VI_Buffer_Terrain.h"
#include "Transform.h"
#include "Texture.h"
/* �پ��� ������Ʈ(����)���� ��� �ִ´�. -> �纻������Ʈ���� ����ؾ��� ��ü���� �����Ѵ�. */
/* �纻�� �����Ͽ� �����Ѵ�. */
BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void* pArg = nullptr);

private:
	_uint											m_iNumLevels = 0;

	/* �������� ���� ������Ʈ(�ؽ���, �޽ø� etc )���� �����ϰڴ�. */
	map<const wstring, class CComponent*>*			m_pPrototypes = nullptr;
	typedef map<const wstring, class CComponent*>	PROTOTYPES;

private:
	CComponent* Find_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag);


public:
	virtual void Free() override;
};

END