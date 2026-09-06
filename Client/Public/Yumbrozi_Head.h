#pragma once
#include "Yumbrozi_Parts.h"

class CYumbrozi_Head final : public CYumbrozi_Parts
{
private:
	CYumbrozi_Head(LPDIRECT3DDEVICE9 pGraphic_Device);
	CYumbrozi_Head(const CYumbrozi_Head& Prototype);
	virtual ~CYumbrozi_Head() = default;

public:
	virtual HRESULT Initialize_Prototype()				override; 
	virtual HRESULT Initialize(void* pArg)				override; 
	virtual void	Priority_Update(_float fTimeDelta)	override;
	virtual void	Update(_float fTimeDelta)			override;
	virtual void	Late_Update(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;
private:
	HRESULT Ready_Components()		override;
public:
	static CYumbrozi_Head* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject*           Clone(void* pArg)			override;
	virtual void           Free() override;
};

