#pragma once
#include "Client_Defines.h"
#include "Tookee.h"
#include "Input_Manager_API.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CCollider;
class CDimensionSwitcher;
struct FCollisionInfo;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CTooTooKee :public CTookee
{
	static inline _uint		MAXTRANSPARANCEY	= 100;
	static inline _float3	TOOTOOKEEPOS		= { 0.f, 0.f, 0.f };

private:
	CTooTooKee(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTooTooKee(const CTooTooKee& Prototype);
	virtual ~CTooTooKee() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;
    void  SetParent(CTookee* pTookee) override;

	static _float3 GetPos() { return TOOTOOKEEPOS; }
private:

	CTexture* m_pCurTextureCom = { nullptr };

	CTexture* m_pTextureIDLE = { nullptr };
	CTexture* m_pTextureRUN = { nullptr };
	CTexture* m_pTextureJUMP = { nullptr };
	CTexture* m_pTextureEDGE = { nullptr };
	CTexture* m_pTextureDIE = { nullptr };


	CTransform* m_pTransformCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CDimensionSwitcher* m_pDimensionCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CTooTooKee* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
private:
	void OnCollision(FCollisionInfo _fCollisionInfo);
	void Jump(_float fTimeDelta);
	void Move(float _fTimeDelta);
	void Set_Scale();
	void Squash();
	void Change_JumpTexIdx();
	void ChangeLR();
	bool Check_OnAir(bool _bIsCol);

	HRESULT Render_Shadow();

	float Lerp(float a, float b, float t)
	{
		return a + (b - a) * t;
	}

	void Topdim_Overlapped(FCollisionInfo _fCollisionInfo);

public:
	bool Get_IsDead() { return m_bIsDead; }
	bool Get_IsClear() { return m_bIsClear; }
	void Reset_Toodee(_float3 vPos);
	void Set_IsDead() {
		m_eCurState = DIE;
		m_bIsDeadAnim = true;
		m_pCurTextureCom = m_pTextureDIE;
		m_pDeadAnimTimer->SetActive(true);
	}
	bool Get_IsAble() { return m_bIsAble; }
	void Add_SpeedX(_float _fSpeed) { m_fSpeedX = _fSpeed; }
	void Add_SpeedY(_float _fSpeed) { m_fSpeedY = _fSpeed; }
	_float Get_PreSpeedY() { return m_fPreSpeedY; }
	void Set_IsGround(_bool _IsGround) { m_bIsGround = _IsGround; }
	void Set_IsTeleporting(_bool _IsTeleporting) { m_bIsTeleporting = _IsTeleporting; }
private:
	void CreateKeyContext();

	UniqueKeyContext m_pKeyContext;
	UniqueTimer m_pJumpLimitTimer = { nullptr };
	UniqueTimer m_pDeadAnimTimer = { nullptr };

	_uint            m_iCurTextureIdx = {};
	_float m_fGravity = -64.f;
	_float m_fSpeedY = 14.f;
	_float m_fSpeedX = 0.f;
	_float m_fPreSpeedX = 0.f;
	_float m_fPreSpeedY = 0.f;

	_bool m_bIsJumping = false;
	_bool m_bWasOnGround = false;
	_bool m_bJumpLimit = true;

	_bool m_bIsCurRight = true;
	_bool m_bIsPreRight = true;
	_bool m_bLeftInput = false;
	_bool m_bRightInput = false;

	_bool m_bIsCollide = false;
	_bool m_bIsGround = true;

	_bool m_bIsTeleporting = false;

	_bool m_bIsAble = true;
	_bool m_bIsDeadAnim = false;
	_bool m_bIsDead = false;
	_bool m_bIsClear = false;

	_float time = 0.f;
	_float m_fTransparency = {};

	_uint m_iDieTextureIdx = {};

	enum ETOODEESTATE { IDLE, RUN, JUMP, FALL, EDGE, DIE, CLEAR, DISABLE, ENTER, TDST_END };
	ETOODEESTATE m_eCurState = IDLE;
	ETOODEESTATE m_ePreState = IDLE;
	void Tookee_MotionChange(ETOODEESTATE _eCurState);

	_float m_fToodeeSize = 3.125f;

	enum ERenderDir { LEFT, RIGHT, UP, DOWN, END };

	//map Info
public:
	void SetMapInfo(vector<vector<class CBlock*>>* _pVecBlockPtrs) { m_pVecBlockPtrs = _pVecBlockPtrs; }
private:
	vector<vector<class CBlock*>>* m_pVecBlockPtrs = nullptr;

};

NS_END