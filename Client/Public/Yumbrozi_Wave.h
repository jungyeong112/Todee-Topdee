#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CCollider;
class CVIBuffer_Rect;
struct FCollisionInfo;
NS_END
NS_BEGIN(Client)

class CYumbrozi_Wave final : public CGameObject
{

private:
	CYumbrozi_Wave(LPDIRECT3DDEVICE9 pGraphic_Device);
	CYumbrozi_Wave(const CYumbrozi_Wave& Prototype);
	virtual ~CYumbrozi_Wave() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Update(_float fTimeDelta)	override;
	virtual void	Update(_float fTimeDelta)			override;
	virtual void	Late_Update(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

	HRESULT Ready_Components();
	
public:
	void OnCollision(FCollisionInfo  _fCollisionInfo);

	void SetActive(_bool _bIsActive) { m_bIsActive = _bIsActive; };
	bool IsActive() { return m_bIsActive; };
	void StartWave(_float3 _vPos);
	void Vanish();
	void    SetParent(CGameObject* pGameObject);
	HRESULT SetParentTransform(CTransform* pTransformCom);
public:
	static CYumbrozi_Wave* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg)			override;
	virtual void           Free()           override;
private:

	void InheritPositon_Begin();
	void InheritPositon_End();

private:
	CTexture* m_pTextureCom = { nullptr };
	
	CTransform* m_pTransformCom       = { nullptr };
	CTransform* m_pPivotTransform     = { nullptr };

	CCollider* m_pColliderCom         = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom    = { nullptr };


private:
	_bool        m_bIsActive = { false };
	_float3      m_fStartPos = {};
	_float3      m_fWaitPos = {};
	_float3      m_fTargetPos = {};
	_float3      m_vOffset;
	_float       m_fColorRatio = { false };

};
NS_END
