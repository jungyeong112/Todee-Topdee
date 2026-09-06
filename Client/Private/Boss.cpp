#include "Boss.h"
#include "Collider.h"
#include "GameInstance.h"
#include "Texture.h"

#include "Toodee.h"
#include "Topdee.h"

#include "HybridCamera.h"

#include "PSystem_SquashDust.h"
#include "ParticleManager.h"

CBoss::CBoss(LPDIRECT3DDEVICE9 pGraphic_Device, _uint _iNumState, _uint _iNumStateGroup)
	: CGameObject(pGraphic_Device), m_iNumState(_iNumState), m_iNumStateGroup(_iNumStateGroup)
{
}

CBoss::CBoss(const CBoss& Prototype)
	: CGameObject(Prototype), 
	m_iNumState				(Prototype.m_iNumState),
	m_iNumStateGroup		(Prototype.m_iNumStateGroup),
	m_vecFuncs				(Prototype.m_vecFuncs),
	m_vecTransitFuncs		(Prototype.m_vecTransitFuncs),
	m_vecForceTransitFuncs	(Prototype.m_vecForceTransitFuncs),
	m_vecGroupTransitFuncs	(Prototype.m_vecGroupTransitFuncs),
	m_iHP					(Prototype.m_iHP),
	m_iCurState				(Prototype.m_iCurState),
	m_iPrevState			(Prototype.m_iPrevState),
	m_vecPatternGroups		(Prototype.m_vecPatternGroups)
{

}


HRESULT CBoss::Initialize_Prototype()
{
	for (_uint i = 0; i < m_iNumState; i++) 
		m_vecFuncs.push_back(vector<list<function<void(float)>>>(3, list<function<void(float)>>()));
	for (_uint i = 0; i < m_iNumState; i++) 
		m_vecTransitFuncs.push_back(vector<function<bool(void)>>(m_iNumState, nullptr));
	for (_uint i = 0; i < m_iNumStateGroup; i++)
		m_vecGroupTransitFuncs.push_back(vector<function<bool(void)>>(m_iNumStateGroup, nullptr));
	for (_uint i = 0; i < m_iNumState; i++)
		m_vecForceTransitFuncs.push_back(nullptr);
	m_vecPatternGroups = vector<vector<_uint>>(m_iNumStateGroup, vector<_uint>());

	return S_OK;
}

HRESULT CBoss::Initialize(void* pArg)
{
	if (pArg) {
		pair<CTopdee*, CToodee*> _pPlayers = *static_cast<pair<CTopdee*, CToodee*>*>(pArg);

		m_pTopdee = _pPlayers.first;
		m_pToodee = _pPlayers.second;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pScreamTexture->SetAnimationDuration(SCREAMDURATION);
	m_pScreamTransformCom->Scale(960 * g_fDot, 960 * g_fDot, 1.f);


	if (!m_pParticleSystem_SquashDust)
	{
		m_pParticleSystem_SquashDust = new CPSystem_SquashDust(m_pGraphic_Device);
		m_pParticleSystem_SquashDust->Initialize();
		m_pParticleSystem_SquashDust->SetSystem(12, 2.f, 4.f, 4.f, 0.5, 1.f);
	}

	return S_OK;
}

void CBoss::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsKeyState('S', EKeyActionState::Enter))
		PlaySquashParticle(_float3(15.f, 8.f, -1.f));
}

void CBoss::Update(_float fTimeDelta)
{
	
	_int _iNxtState = TransitBossState();
	if (_iNxtState == -1) {
		for (auto& _func : m_vecFuncs[m_iCurState][ENUM_TO_UINT(EKeyActionState::Stay)]) _func(fTimeDelta);
	}
	else {
		for (auto& _func : m_vecFuncs[m_iCurState][ENUM_TO_UINT(EKeyActionState::Exit)]) _func(fTimeDelta);

		m_iPrevState = m_iCurState;
		m_iCurState = _iNxtState;

		for (auto& _func : m_vecFuncs[m_iCurState][ENUM_TO_UINT(EKeyActionState::Enter)]) _func(fTimeDelta);
	}
}

void CBoss::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
}

HRESULT CBoss::Render()
{
	CHKFAIL(Scream_Render());

	return S_OK;
}

void CBoss::OnCollision(FCollisionInfo _fCollisionInfo)
{
	if (_fCollisionInfo.strCollisionLayerTag == L"Player" ||
		_fCollisionInfo.strCollisionLayerTag == L"Topdee"
		) {
		//PrintDebug("Boss Contact Player");
	}
}

void CBoss::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pScreamTransformCom);
	Safe_Release(m_pDimensionCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pScreamTexture);
}

HRESULT CBoss::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc = { 30.f, 3.14f, nullptr };

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_ScreamTransform"), reinterpret_cast<CComponent**>(&m_pScreamTransformCom), &TransformDesc)))
		return E_FAIL;

	//Com_Dimension
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Dimension"),
		TEXT("Com_Dimension"), reinterpret_cast<CComponent**>(&m_pDimensionCom))))
		return E_FAIL;

	//Com_Texture
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STATIC), TEXT("Prototype_Component_Texture_Scream"),
		TEXT("Com_Texture_Scream"), reinterpret_cast<CComponent**>(&m_pScreamTexture))))
		return E_FAIL;

	//Com_VIBuffer
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::AddTransitionFunc(_uint _prevState, _uint _nextState, function<bool(void)> _func)
{
	if (_prevState >= m_iNumState)					return E_FAIL;
	if (_nextState >= m_iNumState)					return E_FAIL;

	if (m_vecTransitFuncs[_prevState][_nextState])	return E_FAIL;
	m_vecTransitFuncs[_prevState][_nextState] = _func;

	return S_OK;
}

HRESULT CBoss::AddForceTransitionFunc(_uint _nextState, function<bool(void)> _func)
{
	if (_nextState >= m_iNumState)					return E_FAIL;

	if (m_vecForceTransitFuncs[_nextState])	
		return E_FAIL;
	m_vecForceTransitFuncs[_nextState] = _func;

	return S_OK;
}

HRESULT CBoss::AddGroupTransitionFunc(_uint _prevStateGroup, _uint _nextStateGroup, function<bool(void)> _func)
{
	if (_prevStateGroup >= m_iNumStateGroup)		return E_FAIL;
	if (_nextStateGroup >= m_iNumStateGroup)			return E_FAIL;

	if (m_vecGroupTransitFuncs[_prevStateGroup][_nextStateGroup])	return E_FAIL;
	m_vecGroupTransitFuncs[_prevStateGroup][_nextStateGroup] = _func;

	return S_OK;
}

HRESULT CBoss::AddStateFunc(_uint _State, EKeyActionState _keyAction, function<void(float)> _func)
{
	if (_State >= m_iNumState)					return E_FAIL;
	if (_keyAction == EKeyActionState::None)	return E_FAIL;
	m_vecFuncs[_State][ENUM_TO_UINT(_keyAction)].push_back(_func);

	return S_OK;
}

HRESULT CBoss::AddStateInPatternGrouup(_uint _State, _uint _StateGroup)
{
	if (_State		>= m_iNumState)					return E_FAIL;
	if (_StateGroup >= m_iNumStateGroup)			return E_FAIL;

	m_vecPatternGroups[_StateGroup].push_back(_State);

	return S_OK;
}

_int CBoss::TransitBossState()
{
	for (_uint i = 0; i < m_iNumState; i++)
		if (m_vecForceTransitFuncs[i] && m_vecForceTransitFuncs[i]()) return i;

	_bool bInGroup = false;
	_uint curGroup;
	for (_uint i = 0; i < m_iNumStateGroup && !bInGroup; i++) {
		for(auto _pState : m_vecPatternGroups[i]) {
			if (_pState == m_iCurState) {
				bInGroup = true;
				curGroup = i;
				break;
			}
		}
	}


	for (_uint i = 0; i < m_iNumStateGroup && bInGroup; i++) 
		if (m_vecGroupTransitFuncs[curGroup][i] && m_vecGroupTransitFuncs[curGroup][i]()) { return m_vecPatternGroups[i].front(); }
	
		

	for (_uint i = 0; i < m_iNumState; i++)
		if (m_vecTransitFuncs[m_iCurState][i] && m_vecTransitFuncs[m_iCurState][i]()) return i;

	return -1;
}

HRESULT CBoss::Scream_Render()
{
	if (m_bScreamDone) return S_OK;

	CRenderStateGaurd	RSGuard(m_pGraphic_Device);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTEXOPCAPS_SELECTARG2);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DXCOLOR(1.f, 1.f, 1.f, 1.f - m_pScreamTexture->GetRatio()));
	
	if (FAILED(m_pScreamTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pScreamTexture->Bind_AnimTexture()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (m_pScreamTexture->IsEnd()) { m_bScreamDone = true; }

	return S_OK;
}

void CBoss::Scream_Reset(_uint i)
{
	m_bScreamDone = false;
	m_pScreamTexture->Bind_Texture(0);
	m_pScreamTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION));
	m_pCamera -> ShakeRound(.2f, .8f, 5);
	switch (i) {
	case 1:
		m_pGameInstance->PlaySoundW(L"Boss_Boss1_ScreamSnd.wav", CHANNELID::SOUND_BOSS, BOSSSOUND);
		break;
	case 2:
		m_pGameInstance->PlaySoundW(L"Boss_Boss2_ScreamSnd.wav", CHANNELID::SOUND_BOSS, BOSSSOUND);
		break;
	case 3:
		m_pGameInstance->PlaySoundW(L"Boss_Boss3_ScreamSnd.wav", CHANNELID::SOUND_BOSS, BOSSSOUND);
		break;
	case 4:
		m_pGameInstance->PlaySoundW(L"Boss_Boss4_ScreamSnd.wav", CHANNELID::SOUND_BOSS, BOSSSOUND);
		break;
	}
	
}

void CBoss::PlaySquashParticle(_float3 _vPos)
{
	m_pParticleSystem_SquashDust->Trigger(_vPos);
}

void CBoss::PlaySquashSound()
{
	m_pCamera->ShakeUpDown(.7f, 1.f, 4.f, 4.f);
	m_pGameInstance->PlaySoundW(L"Boss_SquashSnd.wav", CHANNELID::SOUND_BOSS, BOSSSOUND);
}

_uint CBoss::GetHP()
{
	return m_iHP;
}
