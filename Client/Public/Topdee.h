#pragma once
#include "Client_Defines.h"
#include "Input_Manager_API.h"
#include "GameObject.h"
#include "Time_Manager_API.h"

#include "ClearPortal.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CCollider;
class CVIBuffer_Rect;
class CDimensionSwitcher;
struct FCollisionInfo;
NS_END


NS_BEGIN(Client)
class CBlock;
class CTopdee : public CGameObject
{
	inline static _float _vMoveSpeed = 5.f;
protected:
	CTopdee(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTopdee(const CTopdee& Prototype);
	virtual ~CTopdee() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;

	void Set_IsDead();

private:
	void OnCollision(FCollisionInfo  _fCollisionInfo);

protected:
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pDieTextureCom = { nullptr };
	CTexture* m_pTexturCom_OL = { nullptr };

	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pTransformAnimCom = { nullptr };
	CTransform* m_pTransformDieCom = { nullptr };

	CCollider*          m_pColliderCom = { nullptr };
	CVIBuffer_Rect*     m_pVIBufferCom = { nullptr };
	CDimensionSwitcher* m_pDimensionCom = { nullptr };

	

public:
	static  CTopdee* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
	void    Set_Position(_float3 v_Pos);
	_float  Get_Angle() const { return m_fAngle; }
	_int    Get_TextureIdx() const { return m_iCurTextureIdx; }
	_bool   Get_IsTelePorting() const { return m_bIsTelePorting; }
	void    Set_IsTelePorting(_bool isActive) { m_bIsTelePorting = isActive; }

protected:
	_int     m_iCurTextureIdx = {};
	_float   m_fRatio = {};
	_float   m_fAngle = {};
	_float   m_fPreAngle = {};
	_float   m_fOriginAngle = {};

	CBlock*  m_pHoldingObject = { nullptr };
	pair<int, int > m_pairLiftIndex = { 0,0 };   //상자를 들었던 위치 인덱스
	pair<int, int > m_pairPutIndex = { 0,0 };    //상자를  내려놓을 위치 인덱스
	UniqueKeyContext m_pKeyContext_Move;
	UniqueKeyContext m_pKeyContext_Rot;
	_bool     m_bIsDead = false;
	_bool     m_bIsClear = false;
	_bool     m_bIsLift = false;
	_bool     m_bIsConvert = false;
	_float    m_fAlphaRatio = {};
	_bool     m_bIsTelePorting = false;

private:
	void CreateKeyContext();
	HRESULT Ready_Components();

public:
	void  SetAngle();
	_bool HoldingLightningBlock();
private:
	_bool _bHoldingLightningBlock = {false};


public:
	_bool  Get_IsDead() const { return m_bIsDead;}
	_bool  Get_IsClear()const { return m_bIsClear;}
	void   Reset_TopDee(_float3 _vPos);
	_bool  Get_IsLift() const { return m_bIsLift; }
	_bool  Get_IsConvert() const { return m_bIsConvert; }
	_float Get_AlphaRaito() const { return m_fAlphaRatio;}

public:
	enum ETOPDEESTATE { IDLE, WALK,SIDE_WALK, LIFT, PUT, DIE, DISABLE,ABLE, STATE_END };
private:
	
	ETOPDEESTATE m_eCurState = {IDLE};
	ETOPDEESTATE m_ePreState;

private:
	void Topdee_MotionChange();

public:
	ETOPDEESTATE Get_State() { return m_eCurState; }

private:
	vector < tuple< CBlock*, float, int >> m_vecNearBlock = {};   //pBlock , 각도 , 넣은 순서 인덱스
	_uint        m_iPosX = {};
	_uint        m_iPosY = {};
	_float       m_fTime = {};

public:
	void Search_Table();
	void Lift_Block();
	void Put_Block();


//map Info
public:
	void SetMapInfo(vector<vector<class CBlock*>>* _pVecBlockPtrs) { m_pVecBlockPtrs = _pVecBlockPtrs; }
private:
	vector<vector<class CBlock*>>* m_pVecBlockPtrs = nullptr;

//Step Sound
private:
	UniqueTimer m_pFootStepTimer = { nullptr };
	void CreateFootStepTimer();

	_float3 _vDir = _float3( 0.f, 0.f, 0.f );
};
NS_END
