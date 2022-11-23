#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCalculator : public CComponent
{
protected:
	CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCalculator(const CCalculator& rhs);
	virtual ~CCalculator() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public: /* imgui */
		// �� ������Ʈ���� ������ �����͸� imgui�� �ۼ��Ѵ�.
	virtual void Imgui_RenderProperty() {}

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;


};

END