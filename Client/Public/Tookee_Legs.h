#pragma once
#include "Tookee_Parts.h" 
NS_BEGIN(Client)
class CTookee_Legs : public CTookee_Parts
{
private:
	CTookee_Legs(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTookee_Legs(const CTookee_Legs& Prototype);
	virtual ~CTookee_Legs() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;

public:
	static CTookee_Legs* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
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
	void SetParent(CTopTookee* pGameObject) override;

};
NS_END
