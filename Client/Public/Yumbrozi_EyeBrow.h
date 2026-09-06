#pragma once
#include "Yumbrozi_Parts.h"
class CYumbrozi_EyeBrow final : public CYumbrozi_Parts
{
private:
	CYumbrozi_EyeBrow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CYumbrozi_EyeBrow(const CYumbrozi_EyeBrow& Prototype);
	virtual ~CYumbrozi_EyeBrow() = default;
public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Update(_float fTimeDelta)	override;
	virtual void	Update(_float fTimeDelta)			override;
	virtual void	Late_Update(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;
public:
	HRESULT Ready_Components()		override;
public:
	static CYumbrozi_EyeBrow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg)			override;
	virtual void			Free()						override;

private:
	_uint GetTextureIdx();
private:
	_uint m_iCurTextureIdx = {};
};

