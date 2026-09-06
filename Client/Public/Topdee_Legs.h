#pragma once
#include "Topdee_Parts.h" 
NS_BEGIN(Client)
class CTopdee_Legs : public CTopdee_Parts
{
private:
	CTopdee_Legs(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTopdee_Legs(const CTopdee_Legs& Prototype);
	virtual ~CTopdee_Legs() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;

public:
	static CTopdee_Legs* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	HRESULT Ready_Components();

private:
	_float3          m_fOriginPos = {};
	_int             m_iDir = { 1 };
	_int             m_iAnimDir = { 1 };
	UniqueTimer      m_pUpdownTimer = { nullptr };

public:
	// CTopdee_Parts을(를) 통해 상속됨
	void SetParent(CTopdee* pGameObject) override;

};
NS_END
