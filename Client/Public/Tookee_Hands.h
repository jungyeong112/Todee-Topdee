#pragma once
#include "Tookee_Parts.h"


NS_BEGIN(Client)
class CTookee_Hands :public CTookee_Parts
{
private:
	CTookee_Hands(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTookee_Hands(const CTookee_Hands& Prototype);
	virtual ~CTookee_Hands() = default;


public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;

public:
	static CTookee_Hands* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	HRESULT Ready_Components() override;

public:
	// CTopdee_Parts을(를) 통해 상속됨
	void SetParent(CTopTookee* ppGameObject) override;

private:
	_int m_iDir = { 1 };
	_float3          m_fOriginPos = {};
	UniqueTimer  m_pUpdownTimer = { nullptr };
};
NS_END

