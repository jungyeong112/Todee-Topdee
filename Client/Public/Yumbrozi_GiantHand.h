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
class CYumbrozi_GiantHand final : public CGameObject
{
	enum GHPATTERNTIMER 
	{
		GH_FIRST,
		GH_WAIT,
		GH_PULL,
		GH_DROP,
		GH_CRAHSH,
		GH_VANISH,

		GH_END
	};


private:
	CYumbrozi_GiantHand(LPDIRECT3DDEVICE9 pGraphic_Device);
	CYumbrozi_GiantHand(const CYumbrozi_GiantHand& Prototype);
	virtual ~CYumbrozi_GiantHand() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Update(_float fTimeDelta)	override;
	virtual void	Update(_float fTimeDelta)			override;
	virtual void	Late_Update(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

	HRESULT Ready_Components();
	void    Move(_float fTimeDelta);
public:
	void OnCollision(FCollisionInfo  _fCollisionInfo);

	void SetActive(_bool _bIsActive) { m_bIsActive = _bIsActive; };
	bool IsActive() { return m_bIsActive; };
	void Punch(_float3 _vPos);
	void Vanish();
	_bool Get_Crash()const { return m_bIsCrash; }
	void Set_Crash(_bool isActive) { m_bIsCrash = isActive; }


public:
	static CYumbrozi_GiantHand* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg)			override;
	virtual void           Free()           override;

private:
	CTexture* m_pTextureCom         = { nullptr };
	CTexture* m_pArmTextureCom      = { nullptr };
	CTexture* m_pArm2TextureCom      = { nullptr };

	CTransform* m_pTransformCom     = { nullptr };
	CTransform* m_pArmTransformCom  = { nullptr };
	CTransform* m_pArm2TransformCom = { nullptr };

	CCollider* m_pColliderCom       = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom  = { nullptr };

private:
	_bool        m_bIsActive               = { false };
	_bool        m_bIsTimer                = { false };
	UniqueTimer  m_pGiantHandTimer[GH_END] = {nullptr};
	_float3      m_fStartPos               = {};
	_float3      m_fWaitPos                = {};
	_float3      m_fTargetPos              = {};
	_bool        m_bIsCrash                = { false };
};
NS_END
