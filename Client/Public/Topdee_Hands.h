#pragma once
#include "Topdee_Parts.h"


NS_BEGIN(Client)
class CTopdee_Hands :public CTopdee_Parts
{
private:
	CTopdee_Hands(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTopdee_Hands(const CTopdee_Hands& Prototype);
	virtual ~CTopdee_Hands() = default;


public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;

public:
	static CTopdee_Hands* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	HRESULT Ready_Components() override;

public:
	// CTopdee_Parts을(를) 통해 상속됨
	void SetParent(CTopdee* ppGameObject) override;

private:
	_int m_iDir = { 1 };
	_float3          m_fOriginPos = {};
	UniqueTimer  m_pUpdownTimer = { nullptr };
};
NS_END

