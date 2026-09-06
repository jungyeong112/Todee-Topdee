#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
class CCollider;
NS_END

NS_BEGIN(Client)
class CTopdee;
class CBlock abstract : public CGameObject
{
public:
	enum EBLOCKFLAG {
		BF_FIX,
		BF_MOVE,
		BF_PUSH,
		BF_LIFT,
		BF_SPIKE,
		BT_END
	};
private:
	inline static _wstring arrColLayer[BT_END] = { L"FIX", L"KINETIC", L"KINETIC", L"KINETIC", L"SPIKE"};
protected:
	CBlock(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBlock(const CBlock& Prototype);
	virtual ~CBlock() = default;

public:
	virtual HRESULT Initialize_Prototype()	override;
	virtual HRESULT Initialize(void* pArg)	override;
	virtual void Priority_Update(_float fTimeDelta)	override;
	virtual void Update(_float fTimeDelta)			override;
	virtual void Late_Update(_float fTimeDelta)		override;
	virtual HRESULT Render()						override;

public:
	EBLOCKFLAG GetBlockFlag() { return m_iBlockFlag; }

protected:
	CTexture*		m_pTextureCom	= { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom	= { nullptr };
	CCollider*		m_pColliderCom	= { nullptr };

protected:
	virtual HRESULT	Ready_Components();
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;

	void   SetParent(CTopdee* pParent);
	void   SetPosition(_float fx, _float fy, _float fz);

	virtual PickingInfo* Picking(RAY tRay);

	HRESULT Reset() override;
protected:
	_uint		m_iTextureIdx = {};
	EBLOCKFLAG	m_iBlockFlag  = {};
	CTopdee*    m_pParent = { nullptr };

	pair<_uint, _uint> m_pairInitPos;


public:
	void SetMapInfo(vector<vector<class CBlock*>>* _pVecBlockPtrs) { m_pVecBlockPtrs = _pVecBlockPtrs; }
//private:
protected:
	vector<vector<class CBlock*>>* m_pVecBlockPtrs = nullptr;
	pair<_float, _float>          m_pairCurDir = {};
	pair<_uint, _uint>            m_pairCurPos = {};
	CBlock*                       m_pBlock = nullptr;
	vector<pair<_float, _float>>  m_vecRootDir = {};
	_bool                         m_bIsMoveable = false;
	_bool						  m_bIsHoleFall = false;
public:
	_bool Search_Block(_float fx,_float fy);
	void  Push_Block(_float fx, _float fy);
	void  Set_Pos(_float fX, _float fY);
	void  MoveUpdate(_float fx, _float fy);
	_bool DiffKenetic();

	void HoleFall();


};

NS_END