#pragma once
#include "Client_Defines.h"
#include "Input_Manager_API.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTransform;
class CCollider;
class CDimensionSwitcher;
class CTexture;
class CVIBuffer_Rect;
class CVIBuffer_Cube;
struct FCollisionInfo;
NS_END


NS_BEGIN(Client)

//Must add Collider in children class
class CBoss abstract : public CGameObject
{
	constexpr static _float SCREAMDURATION = 2.f;
protected:
	CBoss(LPDIRECT3DDEVICE9 pGraphic_Device, _uint _iNumState, _uint _iNumStateGroup);
	CBoss(const CBoss& Prototype);
	virtual ~CBoss() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	void			Priority_Update(_float fTimeDelta)	override;
	void			Update(_float fTimeDelta)			override;
	void			Late_Update(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

protected:
	virtual void OnCollision(FCollisionInfo  _fCollisionInfo);

protected:
	CTransform*			m_pTransformCom = { nullptr };
	CCollider*			m_pColliderCom	= { nullptr };
	CDimensionSwitcher* m_pDimensionCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
private:
	CTexture*			m_pScreamTexture= { nullptr };
	CTransform* m_pScreamTransformCom = { nullptr };

public:
	virtual CGameObject*	Clone(void* pArg)	PURE;
	virtual void			Free()				override;
	_uint		GetHP();
	void		SetCamera(class CHybridCamera* _pCamera) { m_pCamera = _pCamera; }
protected:
	virtual HRESULT		Ready_Components();
	HRESULT		AddTransitionFunc		(_uint _prevState,		_uint _nextState,			function<bool(void)>  _func);
	HRESULT		AddForceTransitionFunc	(_uint _nextState, function<bool(void)>  _func);
	HRESULT		AddGroupTransitionFunc	(_uint _prevStateGroup, _uint _nextStateGroup,		function<bool(void)>  _func);
	HRESULT		AddStateFunc			(_uint _State, EKeyActionState _keyAction,			function<void(float)> _func);
	HRESULT		AddStateInPatternGrouup	(_uint _State, _uint _nextStateGroup);

	HRESULT		Scream_Render();
	void		Scream_Reset(_uint i = 1);
	void		PlaySquashParticle(_float3 _vPos);
	void		PlaySquashSound();
private:
	_int		TransitBossState();
private:
	const _uint	m_iNumState;
	const _uint	m_iNumStateGroup;
	vector<vector<list<function<void(float)>>>> m_vecFuncs;
	vector<vector<function<bool(void)>>>		m_vecTransitFuncs;
	vector<function<bool(void)>>				m_vecForceTransitFuncs;
	vector<vector<function<bool(void)>>>		m_vecGroupTransitFuncs;
	vector<vector<_uint>>						m_vecPatternGroups;


protected:
	_uint m_iHP = { 2 };

	_uint m_iCurState  = {};
	_uint m_iPrevState = {};
	_bool	m_bScreamDone = true;
	class CTopdee* m_pTopdee = { nullptr };
	class CToodee* m_pToodee = { nullptr };

	class CHybridCamera* m_pCamera = { nullptr };

protected:
	class CPSystem_SquashDust* m_pParticleSystem_SquashDust = { nullptr };
};

NS_END
