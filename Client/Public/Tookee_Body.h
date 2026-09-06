#pragma once
#include "TopTookee.h"
#include "Tookee_Parts.h"

NS_BEGIN(Client)
class CTookee_Body : public CTookee_Parts
{
private:
	CTookee_Body(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTookee_Body(const CTookee_Body& Prototype);
	virtual ~CTookee_Body() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;


public:
	static CTookee_Body* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;

private:
	HRESULT	Ready_Components();

public:
	// CTopdee_Parts을(를) 통해 상속됨
	void SetParent(CTopTookee* ppGameObject) override;
};
NS_END
