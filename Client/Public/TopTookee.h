#pragma once
#include "Client_Defines.h"
#include "Input_Manager_API.h"
#include "Tookee.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CCollider;
class CVIBuffer_Cube;
class CDimensionSwitcher;
struct FCollisionInfo;
NS_END


NS_BEGIN(Client)
class CBlock;
class CTopTookee : public CTookee
{
protected:
	CTopTookee(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTopTookee(const CTopTookee& Prototype);
	virtual ~CTopTookee() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	void Priority_Update(_float fTimeDelta)override;
	void Update(_float fTimeDelta)override;
	void Late_Update(_float fTimeDelta)override;
	HRESULT Render()override;
	void  SetParent(CTookee* pTookee) override;
private:
	void OnCollision(FCollisionInfo  _fCollisionInfo);

protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };


	CCollider* m_pColliderCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	CDimensionSwitcher* m_pDimensionCom = { nullptr };

public:
	static CTopTookee* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
	void   Set_Position(_float3 v_Pos);
	_float  Get_Angle() const { return m_fAngle; }
	_int    Get_TextureIdx() const { return m_iCurTextureIdx; }
protected:
	_int     m_iCurTextureIdx = {};
	_float   m_fRatio = {};
	_float   m_fAngle = {};
	_float   m_fPreAngle = {};
	_float   m_fOriginAngle = {};

	UniqueKeyContext m_pKeyContext_Move;
	UniqueKeyContext m_pKeyContext_Rot;
	_bool     m_bIsDead = false;
	_bool     m_bIsAble = false;
	_float    m_fAlphaRatio = {};

private:
	void CreateKeyContext();
	HRESULT Ready_Components(CTransform* pTransform);

public:
	void  SetAngle();
	_bool Get_Able() const { return m_bIsAble; }
	HRESULT Set_Position();
public:
	_bool  Get_IsDead() const { return m_bIsDead; }
	void   Reset_TopTookee(_float3 _vPos);

public:
	enum ETOPDEESTATE { IDLE, WALK, SIDE_WALK, LIFT, PUT, DIE, DISABLE, ABLE, STATE_END };
private:

	ETOPDEESTATE m_eCurState = { IDLE };
	ETOPDEESTATE m_ePreState;


private:
	void TopTookee_MotionChange();

public:
	ETOPDEESTATE Get_State() { return m_eCurState; }
	void Set_TooTookee(class CTooTooKee* pObj) { m_pTooTookee = pObj; }

private:
	vector < tuple< CBlock*, float, int >> m_vecNearBlock = {};   //pBlock , 각도 , 넣은 순서 인덱스
	_uint        m_iPosX = {};
	_uint        m_iPosY = {};
	class CTooTooKee* m_pTooTookee = { nullptr };

	//map Info
public:
	void SetMapInfo(vector<vector<class CBlock*>>* _pVecBlockPtrs) { m_pVecBlockPtrs = _pVecBlockPtrs; }
private:
	vector<vector<class CBlock*>>* m_pVecBlockPtrs = nullptr;
};
NS_END
