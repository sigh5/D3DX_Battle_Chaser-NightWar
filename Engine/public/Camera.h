#pragma once

#include "GameObject.h"

/* �������� �־� �ʼ������� �ʿ��� ����� �佺���̽� ��ȯ�� ������ȯ. */

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECTDESC
	{
		_float4		vEye, vAt, vUp;
	}CAMERADESC;

protected:
	CCamera(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

	/* ����� ���ε��Ͽ� �˾Ƽ�(������ɷ���������������) ������ ��ȯ�� ���ǵ��� ó��. dx11:x*/
	/* �ʿ��ϸ� ������ ��ɱ�����. */
	//m_pDevice->SetTransform();
	//m_pDevice->GetTransform();

private:
	class CPipeLine*			m_pPipeLine = nullptr;
	CAMERADESC					m_CameraDesc;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END