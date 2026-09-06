#pragma once
#include "Yumbrozi_Parts.h"

class CYumbrozi_Body final : public CYumbrozi_Parts
{
private:
	CYumbrozi_Body(LPDIRECT3DDEVICE9 pGraphic_Device);
	CYumbrozi_Body(const CYumbrozi_Body& Prototype);
	virtual ~CYumbrozi_Body() = default;

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
	static CYumbrozi_Body* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg)			override;
	virtual void           Free() override;
};

