#pragma once
#include "Client_Defines.h"
#include "Tookee_Parts.h"
#include "Time_Manager_API.h"

NS_BEGIN(Client)

class CTookee_SpinningHead :public CTookee_Parts
{
private:
	CTookee_SpinningHead(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTookee_SpinningHead(const CTookee_SpinningHead& Prototype);
	virtual ~CTookee_SpinningHead() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;

private:
	HRESULT Ready_Components() override;

public:
	static CTookee_SpinningHead* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	// CTopdee_Parts을(를) 통해 상속됨
	void SetParent(CTopTookee* ppGameObject) override;
private:
	_float           m_fAngle = {};
	_float           m_fPreAngle = {};
	_float           m_fRatio = {};
	_float3          m_fOriginPos = {};
	_int             m_iDir = { 1 };
	UniqueTimer      m_pUpdownTimer = { nullptr };
};

NS_END