#pragma once
#include "Client_Defines.h"
#include "Input_Manager_API.h"
#include "GameObject.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CCollider;
class CVIBuffer_Cube;
class CDimensionSwitcher;
struct FCollisionInfo;
NS_END;

NS_BEGIN(Client)
class CBlock;

class CTookee : public CGameObject
{
protected:
	CTookee(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTookee(const CTookee& Prototype);
	virtual ~CTookee() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;

public:
	virtual void   SetParent(CTookee* pTookee) {};

protected:
	CTookee*             m_pParent             = { nullptr };
	class CTopdee*       m_pTopdee             = { nullptr };
	class CToodee*       m_pToodee             = { nullptr };
	CTransform*          m_pTransformCom       = { nullptr };
	CTransform*          m_pTargetTransformCom = { nullptr };
	pair<_float, _float> m_pairCurPos = {};
	pair<_float, _float> m_pairPrePos = {};


public:
	_bool  IsConvert() const { return m_bIsConvert; }
	void   Set_Topdee(CTopdee* pTopdee) { m_pTopdee = pTopdee; }
	void   Set_Toodee(CToodee* pToodee) { m_pToodee = pToodee; }
	void   Move();
	void   Set_Position(_float3 vPos);

private:
	_bool     m_bIsConvert = false;
private:
	HRESULT Ready_Components();
	void OnCollision(FCollisionInfo  _fCollisionInfo);

public:
	static CTookee* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END
