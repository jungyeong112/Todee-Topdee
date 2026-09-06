#pragma once

#include "Boss_Part.h"

NS_BEGIN(Client)
class CBoss;

class CYumbrozi_Parts : public CBoss_Part
{
protected:
	CYumbrozi_Parts(LPDIRECT3DDEVICE9 pGraphic_Device);
	CYumbrozi_Parts(const CYumbrozi_Parts& Prototype);
	virtual ~CYumbrozi_Parts() = default;

public:
	virtual HRESULT SetParent(CBoss* ppGameObject) override;
	HRESULT SetParentTransform(CTransform* pTransformCom);
	HRESULT SetHandTransform(CTransform* pTransformCom);
	void Flip(_bool _bFlip = true);
	void IsAnimActive(_bool bIsActive) { m_bIsAnim = bIsActive;}
	void SetTexture(_uint _idx) { m_iTextureIdx = _idx; }
protected:
	void InheritPositon_Begin();
	void InheritPositon_End();
	void InheritPositon_Begin_Limb();
	void InheritPositon_End_Limb();
protected:
	class CTransform* m_pParentTransform = { nullptr };
	class CTransform* m_pBaseTransform = { nullptr };
	class CTransform* m_pHandTransform = { nullptr };

	_float3 m_vOffset;
	bool	m_bFlip = false;
	_uint   m_iTextureIdx;
	_float3 m_vOriginScale = {};
	_bool   m_bIsAnim = { false };
};
NS_END