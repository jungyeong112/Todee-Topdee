#pragma once
#include "Topdee.h"
#include "Topdee_Parts.h"
NS_BEGIN(Client)
class CTopdee_Body: public CTopdee_Parts
{
private:
	CTopdee_Body(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTopdee_Body(const CTopdee_Body& Prototype);
	virtual ~CTopdee_Body() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;


public:
	static CTopdee_Body* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;

private:
	HRESULT	Ready_Components();

public:
	// CTopdee_Parts을(를) 통해 상속됨
	void SetParent(CTopdee* ppGameObject) override;
};
NS_END
