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
		// 이 컴포넌트에서 보여줄 데이터를 imgui로 작성한다.
	virtual void Imgui_RenderProperty() {}

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;


};

END