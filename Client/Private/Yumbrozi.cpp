#include "Yumbrozi.h"
#include "Collider.h"
#include "GameInstance.h"
#include "Transform.h"

#include "Yumbrozi_Parts.h"
#include "Yumbrozi_GiantHand.h"
#include "Yumbrozi_Wave.h"
#include "Toodee.h"
#include"PSystem_Dust.h"
#include "ParticleManager.h"
#include "SpikeBlock.h"

CYumbrozi::CYumbrozi(LPDIRECT3DDEVICE9 pGraphic_Device) : CBoss(pGraphic_Device, EBOSSSTATE::BS_END, NUMPATTERNGROUP)
{
}

CYumbrozi::CYumbrozi(const CYumbrozi& Prototype)
	: CBoss(Prototype),
	m_vecLayerMasks(Prototype.m_vecLayerMasks)
{
}

HRESULT CYumbrozi::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	for (_uint i = 0; i < BP_END; i++) m_pivotTransformComs[i] = nullptr;
	for (_uint i = 0; i < BP_END; i++) m_pListBodyParts[i] = list<CYumbrozi_Parts*>();

	m_vecLayerMasks.clear();
	m_vecLayerMasks.push_back(wstring(L"FIX"));
	m_vecLayerMasks.push_back(wstring(L"SPIKE"));

	return S_OK;
}

HRESULT CYumbrozi::Initialize(void* pArg)
{
	for (_uint i = 0; i < BP_END; i++) m_pivotTransformComs[i] = nullptr;
	for (_uint i = 0; i < BP_END; i++) m_pListBodyParts[i] = list<CYumbrozi_Parts*>();

	if (FAILED(__super::Initialize(pArg))) return E_FAIL;

	m_pChaseTimer = m_pGameInstance->CreateTimerWithDuration(2.6f, false, nullptr, false);

	m_pHitTimer = m_pGameInstance->CreateTimerWithDuration(1.f, false, nullptr, false);

	m_pSpinTimer = m_pGameInstance->CreateTimerWithDuration(1.16f, false, [=]() {m_bIsDropable = true; }, false);

	m_pDropTimer = m_pGameInstance->CreateTimerWithDuration(0.26f, false, [=]() {m_bIsDropable = false; }, false);

	m_pDropWaitTimer = m_pGameInstance->CreateTimerWithDuration(1.3f, false, [=]() {m_bIsReChase = true; }, false);

	m_pReDropTimer = m_pGameInstance->CreateTimerWithDuration(0.26f, false, nullptr, false);

	m_pWaitTimer = m_pGameInstance->CreateTimerWithDuration(0.23f, false, nullptr, false);

	m_pJumpTimer = m_pGameInstance->CreateTimerWithDuration(1.f, false, nullptr, false);

	m_pIdleTimer = m_pGameInstance->CreateTimerWithDuration(2.f, false, [=]() {m_bIsWait = false; m_bIsReset = false; }, true);

	m_pTailTimer = m_pGameInstance->CreateTimerWithDuration(1.f, true, nullptr, true);

	//AnimTimer
	m_pAnimIdleTimer = m_pGameInstance->CreateTimerWithDuration(2.f, true, nullptr, true);
	m_pAnimScreamTimer = m_pGameInstance->CreateTimerWithDuration(2.f, false, nullptr, false);
	m_pAnimStandTimer = m_pGameInstance->CreateTimerWithDuration(1.3f, false, nullptr, false);
	m_pAnimGiantTimer = m_pGameInstance->CreateTimerWithDuration(3.3f, false, nullptr, false);

	//WaveTimer
	for (_uint i = 0; i < WP_END; i++)
	{
		if (i == WP_END - 1)
			m_pWaveTimer[i] = m_pGameInstance->CreateTimerWithDuration(0.2f, false, [=]() {m_bIsWave = false, m_fWaveOriginPos = {}; }, false);
		else
			m_pWaveTimer[i] = m_pGameInstance->CreateTimerWithDuration(0.2f, false, [=]() {m_pWaveTimer[i + 1]->Restart(); }, false);
	}

	//WaveCnt 
	for (_uint i = 0; i < WP_END; i++)
	{
		m_iWaveCnt[i] = i * 4;
	}

	m_iCurState = BS_IDLE;
	m_iJumpIdx = 0;
	m_iJumpCount = 0;
	CHKFAIL(SetIdleState())
		CHKFAIL(SetJumpState())
		CHKFAIL(Set_ChaseState())

		CHKFAIL(Set_ShockWaveState())
		CHKFAIL(Set_GiantHandState())
		CHKFAIL(Set_SpinDropState())


		CHKFAIL(SetHitState())
		CHKFAIL(SetDieState())

		CHKFAIL(Set_ScreamState())

		//for (_uint iState : arrPattern1Group) CHKFAIL(AddStateInPatternGrouup(iState, 0))

		m_vTargetPos = { 0,0,0 };
	m_iOriginX = 15;

	if (!m_pSystemDust)
	{
		m_pSystemDust = new CPSystem_Dust(m_pGraphic_Device);
		m_pSystemDust->Initialize();
		m_pSystemDust->SetSystem(12, .5f, 8.f, 7.f, .5f);
	}


	return S_OK;
}

void CYumbrozi::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CYumbrozi::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	UpdateBodyChain(fTimeDelta);
	UpdateTailChain(fTimeDelta);

	auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
	auto [curState, fRatio] = m_pGameInstance->Get_DimensionInfo();
	if (curState == EDimensionState::TWODIM) 
	{
		m_pTarget = m_pToodee;
	}
	else 
	{
		m_pTarget = m_pTopdee;
	} 
	
	m_pWaveMainTransform->Rotation(_float3{ 1.f, 0.f, 0.f }, D3DXToRadian((1 - fRatio) * 70));
	m_pWaveMainTransform->Scale(1.f, .9f * fRatio + .1f, 1.f);
	Wave();
}

void CYumbrozi::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_bIsCol = false;
}

HRESULT CYumbrozi::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))return E_FAIL;

	CCollider::FColliderDesc ColliderDesc{};
	ColliderDesc.bInitialActive = true;
	ColliderDesc.bIsTrigger = false;
	ColliderDesc.func = [=](FCollisionInfo _collisionInfo) {OnCollision(_collisionInfo); };
	ColliderDesc.iLayerLevelIndex = 0;
	ColliderDesc.pObject = this;
	ColliderDesc.pTransform = m_pTransformCom;
	ColliderDesc.strCollisionLayerTag = TEXT("Boss");
	FRectColliderInfo _collisionInfo;
	_collisionInfo.fHeight = 77.f / g_iCubicDot;
	_collisionInfo.fWidth = 72.f / g_iCubicDot;
	_collisionInfo.vCenter = _float2{ 0,0 };
	ColliderDesc.colliderInfo = _collisionInfo;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	CTransform::TRANSFORM_DESC  TransformDesc{ 0.f, 0.f };
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		(L"Com_Transform99"), reinterpret_cast<CComponent**>(&m_pWaveMainTransform), &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		(L"Com_Transform98"), reinterpret_cast<CComponent**>(&m_pBodyTargetTransform), &TransformDesc)))
		return E_FAIL;


	wstring _strCom = L"Com_Transform";
	for (_uint i = 0; i < BP_END; i++)
	{
		if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
			(_strCom + to_wstring(i)), reinterpret_cast<CComponent**>(&m_pivotTransformComs[i]), &TransformDesc)))
			return E_FAIL;
		CTransform::TRANSFORM_DESC _transformDesc{ 0.f, 0.f,m_pTransformCom };
		if (i != BP_BODY && i != BP_RULIMB && i != BP_LULIMB && i < BP_TAIL0)
			m_pivotTransformComs[i]->SetParent(&_transformDesc);
	}

	m_pivotTransformComs[BP_HEAD]->Set_State(STATE::POSITION, _float3{ 0.f, 1.f, -0.1f });
	m_pivotTransformComs[BP_BODY]->Set_State(STATE::POSITION, _float3{ 2.f, 1.5f, 0.3f });
	m_pivotTransformComs[BP_FACE]->Set_State(STATE::POSITION, _float3{ -0.7f, 0.8f, -0.3f });
	m_pivotTransformComs[BP_JAW]->Set_State(STATE::POSITION, _float3{ -0.5f, 0.6f, -0.25f });
	m_pivotTransformComs[BP_NOSE]->Set_State(STATE::POSITION, _float3{ -1.2f, 1.1f, -0.95f });
	m_pivotTransformComs[BP_LEYE]->Set_State(STATE::POSITION, _float3{ -1.7f, 1.4f, -0.4f });
	m_pivotTransformComs[BP_REYE]->Set_State(STATE::POSITION, _float3{ -0.3f, 1.3f, -0.65f });
	m_pivotTransformComs[BP_LBROW]->Set_State(STATE::POSITION, _float3{ -1.7f, 2.f, -0.45f });
	m_pivotTransformComs[BP_RBROW]->Set_State(STATE::POSITION, _float3{ -0.3f, 2.f, -0.75f });

	m_pivotTransformComs[BP_RHAND]->Set_State(STATE::POSITION, _float3{ 1.3f,-1.5f, -0.1f });
	m_pivotTransformComs[BP_LHAND]->Set_State(STATE::POSITION, _float3{ -2.f,-1.5f, 0.3f });
	m_pivotTransformComs[BP_RFOOT]->Set_State(STATE::POSITION, _float3{ 2.f,-1.f, 1.05f });
	m_pivotTransformComs[BP_LFOOT]->Set_State(STATE::POSITION, _float3{ -1.5f,-1.f, 1.05f });

	m_pivotTransformComs[BP_RSHOULDER]->Set_State(STATE::POSITION, _float3{ 2.8f, 1.4f, -0.05f });
	m_pivotTransformComs[BP_LSHOULDER]->Set_State(STATE::POSITION, _float3{ -2.4f,1.4f, -0.05f });

	m_pivotTransformComs[BP_RPELVIS]->Set_State(STATE::POSITION, _float3{ 1.7f, -0.7f, 1.f });
	m_pivotTransformComs[BP_LPELVIS]->Set_State(STATE::POSITION, _float3{ -2.f,-0.f, 1.f });


	for (_uint i = BP_HEAD; i < BP_END; i++)
	{
		auto _vPos = m_pivotTransformComs[i]->Get_State(STATE::POSITION);
		m_iPartsOriginY[i] = _vPos.y;
		m_iPartsOriginX[i] = _vPos.x;
		m_iFaceFlipX[i] = -1 * _vPos.x;
		m_iPartsOriginZ[i] = _vPos.z;
	}

	CTransform::TRANSFORM_DESC _transformDesc = { 0.f, 0.f, m_pivotTransformComs[BP_HEAD] };
	m_pBodyTargetTransform->SetParent(&_transformDesc);
	m_pBodyTargetTransform->Set_State(STATE::POSITION, _float3{ 2.f, 0.7f, 0.1f });

	//m_pivotTransformComs[BP_RSHOULDER]->Set_State(STATE::POSITION, _float3{ 3.5f,-1.8f, -0.04f });
	//m_pivotTransformComs[BP_LSHOULDER]->Set_State(STATE::POSITION, _float3{ -2.5f,-1.0f, -0.04f });



	m_pTransformCom->Set_State(STATE::POSITION, _float3{ 15.f, 8.f, ZOffset });

	return S_OK;
}

HRESULT CYumbrozi::Reset()
{
	__super::Reset();
	m_bIsReset = true;
	for (_uint i = 0; i < WAVECNT; i++) m_pWave[i]->Vanish();

	m_pTransformCom->Set_State(STATE::POSITION, _float3{ 15.f, 8.f, ZOffset });
	m_iOriginX = 15.f;
	m_iOriginY = 8.f;

	m_vTargetPos = {};
	m_iCurState = BS_IDLE;
	m_pIdleTimer->Restart();
	m_bHit = false;

	AdjustEyeScale(1.f);
	Set_OriginPartPos();
	ChangeChainOffset(_float3{ 2.f,0.7f,0.5f });

	SetTexureIndex(BP_LFOOT, 4);
	SetTexureIndex(BP_RFOOT, 4);
	SetTexureIndex(BP_LHAND, 4);
	SetTexureIndex(BP_RHAND, 4);

	m_iJumpIdx = 0;
	m_iCurSetNumber = 0;
	m_iJumpCount = 0;
	for (_uint i = 0; i < WP_END; i++) { m_pWaveTimer[i]->Reset(); }
	m_pJumpTimer->Reset();

	m_bIsWave = false;
	m_vecSpawnPoint.clear();
	if (!m_iRePeatCount)
		for (_uint i = 0; i < MAXGIANTHANDCNT; i++) m_pGiantHand[i]->Vanish();

	return S_OK;
}

HRESULT CYumbrozi::SetJumpState()
{
	HRESULT hr;

	hr = AddStateFunc(BS_JUMP, EKeyActionState::Enter, [=](_float _fTimeDelta)
		{
			m_vTargetPos = PatternJumpPos[m_iJumpIdx];
			m_iJumpCount++;
			if (m_iOriginX == m_vTargetPos.x)
			{
				m_iMoveX = 0.f;
			}
			else if (m_iOriginX > m_vTargetPos.x)
			{
				m_iMoveX = -20.f;
			}
			else if (m_iOriginX < m_vTargetPos.x)
			{
				m_iMoveX = 20.f;
			}
			m_iRePeatCount = m_iHP - 1; //2
			m_iOriginY = 7.f;
			m_iMoveY = 7.f;
			if (!m_iRePeatCount)
				PrintDebug("Cur T IDX ", m_iJumpIdx);
			if (!m_pJumpTimer->IsActive())
				m_pJumpTimer->Restart();

			ActiveAnim(BP_LHAND);
			ActiveAnim(BP_RHAND);
			ActiveAnim(BP_LFOOT);
			ActiveAnim(BP_RFOOT) ;


			m_pSystemDust->SetSystem(10 , .5f, 5.f, 1.f, 1.5f, 0.f, D3DX_PI / 2 - D3DX_PI / 36 * 8 , D3DX_PI / 2 + D3DX_PI / 36 * 6);
			m_pSystemDust->Trigger(m_pTransformCom->Get_State(STATE::POSITION) - _float3{ 0.f,0.5f * 1.35f - 0.2f,0.f });

			//ChangeChainOffset(_float3{ -0.1 , -2, 0.5f });

		});
	CHKFAIL(hr)


		hr = AddStateFunc(BS_JUMP, EKeyActionState::Stay, [=](_float _fTimeDelta)
			{
				if (m_pJumpTimer->IsActive() && !m_bIsReset)
				{
					auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
					auto _targetPos = m_vTargetPos;
					auto jumpRatio = m_pJumpTimer->GetElapsedRatio();

					if (jumpRatio > 0.98f && (m_vTargetPos == PatternJumpPos2 || m_vTargetPos == PatternJumpPos0))
					{
						if (m_vTargetPos == PatternJumpPos2)
						{
							FlipOriginFaceX();
							ChangeChainOffset(_float3{ -2.f,0.7f,0.5f });
							for (_uint i = BP_LEYE; i < BP_BODY; i++)
							{
								for (auto& _pPart : m_pListBodyParts[i])
								{
									if (i == BP_LBROW || i == BP_RBROW || i == BP_LEYE || i == BP_REYE || i == BP_RPELVIS || i == BP_LPELVIS
										|| i == BP_LSHOULDER || i == BP_RSHOULDER)
										continue;
									_pPart->Flip();
									m_bIsRight = true;
								}
							}
						}
						if (m_vTargetPos == PatternJumpPos0)
						{
							ChangeChainOffset(_float3{ 2.f,0.7f,0.5f });
							OriginFaceX();

							for (_uint i = BP_LEYE; i < BP_BODY; i++)
							{
								for (auto& _pPart : m_pListBodyParts[i])
								{
									if (i == BP_LBROW || i == BP_RBROW || i == BP_LEYE || i == BP_REYE || i == BP_RPELVIS || i == BP_LPELVIS
										|| i == BP_LSHOULDER || i == BP_RSHOULDER)
										continue;
									_pPart->Flip(false);
									m_bIsRight = false; 
								}
							}
						}

						m_pSystemDust->SetSystem(6, 1.f, 1.f, 1.f, 3.5f, 0.f, 0.f, D3DX_PI / 36 * 3);
						m_pSystemDust->Trigger(m_pTransformCom->Get_State(STATE::POSITION) - _float3{ 0.f,0.5f * 1.35f * 0.4f, 0.f });

						m_pSystemDust->SetSystem(4, 1.f, 1.f, 1.f, 3.5f, 0.f, D3DX_PI - D3DX_PI / 36 * 3, D3DX_PI);
						m_pSystemDust->Trigger(m_pTransformCom->Get_State(STATE::POSITION) - _float3{ 0.f,0.5f * 1.35f * 0.4f, 0.f });
					}

					if (m_pTarget == m_pTopdee && jumpRatio < 0.98f)
					{
						m_pColliderCom->SetActive(false);
					}
					else
						m_pColliderCom->SetActive(true);

					_float sT = m_pJumpTimer->GetElapsedTime();
					_float rT = 1.f;


					_vPos.x = m_iMoveX * (min((0.5f), ((1 / rT) * sT)));
					_vPos.x += m_iOriginX;

					_vPos.y = (4 * m_iMoveY * sT / rT) - (4 / (rT * rT)) * m_iMoveY * sT * sT;
					_vPos.y += m_iOriginY;
					m_pTransformCom->Set_State(STATE::POSITION, _vPos);
				}
			});
	CHKFAIL(hr)



		hr = AddStateFunc(BS_JUMP, EKeyActionState::Exit, [=](_float _fTimeDelta)
			{
				m_iOriginX = m_vTargetPos.x;
				OriginPosHead();
				OriginFaceY();
				OriginFaceZ();
				//AdjustLocation(BP_HEAD, _float3{ 0,0, -0.5f });

			});
	CHKFAIL(hr)


		hr = AddTransitionFunc(BS_JUMP, BS_SHOCKWAVE, [=]() { return IsArrived(); });
	CHKFAIL(hr)

		return S_OK;
}

HRESULT CYumbrozi::Set_ShockWaveState()
{
	HRESULT hr;
	hr = AddStateFunc(BS_SHOCKWAVE, EKeyActionState::Enter, [=](_float _fDeltaTime)
		{
			PrintDebug("SHOCKWAVE");

			auto  OriginPos = m_pTransformCom->Get_State(STATE::POSITION);
			OriginPos.x = OriginPos.x + 1;
			m_fWaveOriginPos = OriginPos;

			if (!m_pWaveTimer[WP_1]->IsActive() && !m_bIsWave)
				m_pWaveTimer[WP_1]->Restart();


			if (m_iRePeatCount <= m_iJumpCount && m_iRePeatCount)
			{
				m_iJumpIdx = (m_iJumpIdx + 1) % 4;
				m_iJumpCount = 0;
				m_iCurSetNumber++;
			}
			if (!m_iRePeatCount)
			{
				m_iJumpIdx = (m_iJumpIdx + 1) % 4;
			}
		});
	CHKFAIL(hr)

		hr = AddStateFunc(BS_SHOCKWAVE, EKeyActionState::Stay, [=](_float _fDeltaTime)
			{
				IdleAnim(_fDeltaTime);
			});
	CHKFAIL(hr)

		hr = AddTransitionFunc(BS_SHOCKWAVE, BS_JUMP, [=]()
			{
				if (!m_pWaveTimer[WP_4]->IsActive() && !m_bIsWave)
				{
					switch (m_iRePeatCount)
					{
					case 0:
						return false;
						break;
					case 1:
						if (m_iCurSetNumber == 6)
							return false;
						break;
					case 2:
						if (m_iCurSetNumber == 4)
							return false;
						break;
					}
					return true;
				}
				return false;

			});
	CHKFAIL(hr)
		hr = AddTransitionFunc(BS_SHOCKWAVE, BS_SCREAM, [=]()
			{
				if (!m_pWaveTimer[WP_4]->IsActive() && !m_bIsWave)
				{
					switch (m_iRePeatCount)
					{
					case 0:
						return true;
						break;
					case 1:
						if (m_iCurSetNumber == 6)
							return true;
						break;
					case 2:
						if (m_iCurSetNumber == 4)
							return true;
						break;
					}
				}
				return false;
			});

	return S_OK;
}

HRESULT CYumbrozi::Set_ChaseState()
{
	HRESULT hr;
	hr = AddStateFunc(BS_CHASE, EKeyActionState::Enter, [=](_float _fDeltaTime) {
		PrintDebug("CHASE");

		if (!m_pChaseTimer->IsActive())
			m_pChaseTimer->Restart();

		AdjustLocation(BP_RHAND, _float3{ 0.3 ,0.4, -0.5 });
		AdjustLocation(BP_RSHOULDER, _float3{ -3 ,-2, 0 });

		AdjustLocation(BP_RFOOT, _float3{ -0.7 ,-2, -1.9 });
		AdjustLocation(BP_RPELVIS, _float3{ -2.5 ,-2, -0.6 });

		AdjustLocation(BP_LHAND, _float3{ -0.7 ,0.5, -0.5});
		AdjustLocation(BP_LSHOULDER, _float3{ 1 ,-2.3, 0 });

		AdjustLocation(BP_LFOOT, _float3{ -0.8 ,-2.1, -1.5 });
		AdjustLocation(BP_LPELVIS, _float3{ 1 , -2.3,  0 });



		for (auto& _pPart : m_pListBodyParts[BP_LHAND])
		{
			_pPart->Flip();
		}
		for (auto& _pPart : m_pListBodyParts[BP_LFOOT])
		{
			_pPart->Flip();
		}
	
		ChangeChainOffset(_float3{ -0.5, -2.5, 0.3 });

		SetTexureIndex(BP_LFOOT, 0);
		SetTexureIndex(BP_RFOOT, 0);
		SetTexureIndex(BP_LHAND, 0);
		SetTexureIndex(BP_RHAND, 0);

		});
	CHKFAIL(hr)

		hr = AddStateFunc(BS_CHASE, EKeyActionState::Stay, [=](_float _fDeltaTime)
			{
				m_pColliderCom->SetActive(false);
				_float MoveSpeed = 10.f;
				auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
				_float3 _targetPos = static_cast<CTransform*>(m_pTarget->Get_Component(L"Com_Transform"))->Get_State(STATE::POSITION);
				if (m_pTarget == m_pToodee)
					_targetPos.y += 3;
				_targetPos.y += 4;
				_float3 vDir = _targetPos - _vPos;
				_float vDistance = D3DXVec3Length(&vDir);

				if (vDistance >= MoveThreshold)
				{
					_vPos += *D3DXVec3Normalize(&vDir, &vDir) * MoveSpeed * _fDeltaTime;
				}
				m_pTransformCom->Set_State(STATE::POSITION, _vPos);
			});
	CHKFAIL(hr)


		hr = AddTransitionFunc(BS_CHASE, BS_SPINDROP, [=]()
			{
				return !m_pChaseTimer->IsActive();
			});
	CHKFAIL(hr)
		return S_OK;
}

HRESULT CYumbrozi::Set_ScreamState()
{
	HRESULT hr;
	hr = AddStateFunc(BS_SCREAM, EKeyActionState::Enter, [=](_float _fDeltaTime) {
		Scream_Reset();
		m_pAnimScreamTimer->Restart();
		AdjustLocation(BP_JAW, { 0, -1.f, 0 });
		AdjustEyeScale(1.3f);
		});
	CHKFAIL(hr)

		hr = AddStateFunc(BS_SCREAM, EKeyActionState::Stay, [=](_float _fDeltaTime)
			{

				ScreamAnim(_fDeltaTime);
				_float _time = m_pAnimScreamTimer->GetElapsedRatio();

				auto _vPos = m_pivotTransformComs[BP_JAW]->Get_State(STATE::POSITION);
				_float y = sin(D3DX_PI * 2.0f * _time);
				_float maxY = m_iPartsOriginY[BP_JAW] - 1;
				_vPos.y += y * 0.01f;

				m_pivotTransformComs[BP_JAW]->Set_State(STATE::POSITION, _vPos);

			});
	CHKFAIL(hr)

		hr = AddTransitionFunc(BS_SCREAM, BS_GIANTHAND, [=]()
			{
				return m_bScreamDone;
			});
	CHKFAIL(hr)

		hr = AddStateFunc(BS_SCREAM, EKeyActionState::Exit, [=](_float _fDeltaTime)
			{
				AdjustLocation(BP_JAW, { 0, 1.f, 0 });
				AdjustEyeScale(1.f);
			});
	CHKFAIL(hr)
		return S_OK;
}

HRESULT CYumbrozi::Set_GiantHandState()
{
	HRESULT hr;
	hr = AddStateFunc(BS_GIANTHAND, EKeyActionState::Enter, [=](_float _fDeltaTime) {
		PrintDebug("GIANT HAND!!!!!");

		m_pAnimGiantTimer->Restart();
		m_pAnimStandTimer->Restart();
		LocationHead(0, 3, 0);
		AdjustLocation(BP_LSHOULDER, _float3{ 0,-1,0 });
		AdjustLocation(BP_RSHOULDER, _float3{ -1,-1,0 });


		ChangeChainOffset(_float3{ -0.5, -2, 0.5f });
		for (_uint i = 0; i < 3; i++)
		{
			auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
			if (_vPos.x != SpawnPos[i].x)
				m_vecSpawnPoint.push_back(SpawnPos[i]);
		}

		for (_uint i = 0; i < MAXGIANTHANDCNT; i++)
		{
			if (m_pGiantHand[i]->IsActive())break;
			m_pGiantHand[i]->Punch(m_vecSpawnPoint[i]);
		}

		if (!m_iRePeatCount)
		{
			++m_iCurSetNumber;
		}


		auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
		m_iStandOriginY = _vPos.y;
		m_iStandOriginX = _vPos.x;
		});

	CHKFAIL(hr)


		hr = AddStateFunc(BS_GIANTHAND, EKeyActionState::Stay, [=](_float _fDeltaTime)
			{
				IdleAnim(_fDeltaTime);
				for(_uint i = 0 ; i< MAXGIANTHANDCNT; i++)
				{
					if (m_pGiantHand[i]->Get_Crash()) 
					{
						PlaySquashSound();
						auto _vPos = m_vecSpawnPoint[i];
						_vPos.y = 7;
						PlaySquashParticle(_vPos);
						m_pGiantHand[i]->Set_Crash(false);
					}

				}

				if (m_pAnimStandTimer->IsActive())
				{
					auto _vPos = m_pivotTransformComs[BP_RHAND]->Get_State(STATE::POSITION);
					auto _ratio = m_pAnimStandTimer->GetElapsedRatio();
					auto _targetY = m_iStandOriginY + 17;
					auto _targetX = m_iStandOriginX;
					if (m_iStandOriginX == PatternJumpPos2.x)
						_vPos.x = _targetX;
					_vPos.y = _ratio * _targetY + (1 - _ratio) * _vPos.y;
					m_pivotTransformComs[BP_RHAND]->Set_State(STATE::POSITION, _vPos);

					auto _vPos2 = m_pivotTransformComs[BP_LHAND]->Get_State(STATE::POSITION);
					auto _ratio2 = m_pAnimStandTimer->GetElapsedRatio();
					auto _targetY2 = m_iStandOriginY + 17;
					auto _targetX2 = m_iStandOriginX - 6;
					_vPos2.y = _ratio2 * _targetY2 + (1 - _ratio2) * _vPos2.y;
					if (m_iStandOriginX == PatternJumpPos2.x)
					_vPos2.x = _targetX2;
					m_pivotTransformComs[BP_LHAND]->Set_State(STATE::POSITION, _vPos2);
					PrintDebug("LH_", _vPos2);
					PrintDebug("RH_", _vPos2);
				}
				if (m_pAnimGiantTimer->GetElapsedRatio() > 0.8f && !m_pAnimStandTimer->IsActive())
				{
					auto _vPos = m_pivotTransformComs[BP_RHAND]->Get_State(STATE::POSITION);
					auto _ratio = m_pAnimGiantTimer->GetElapsedRatio();
					auto _targetY = m_iPartsOriginX[BP_RHAND];
					auto _targetX = m_iPartsOriginY[BP_RHAND];
					_vPos.y = _ratio * _targetY + (1 - _ratio) * _vPos.y;
					_vPos.x = _targetX;
					m_pivotTransformComs[BP_RHAND]->Set_State(STATE::POSITION, _vPos);

					auto _vPos2 = m_pivotTransformComs[BP_LHAND]->Get_State(STATE::POSITION);
					auto _ratio2 = m_pAnimGiantTimer->GetElapsedRatio();
					auto _targetY2 = m_iPartsOriginX[BP_LHAND];
					auto _targetX2 = m_iPartsOriginY[BP_LHAND];
					_vPos2.y = _ratio2 * _targetY2 + (1 - _ratio2) * _vPos2.y;
					_vPos2.x = _targetX2;
					m_pivotTransformComs[BP_LHAND]->Set_State(STATE::POSITION, _vPos2);

					if (m_vTargetPos.x > 15)
					{
						ChangeChainOffset(_float3{ -2, 0.7, 0.5f });
						//OriginFaceY();
						//OriginFaceZ();
						Set_OriginPartPos();
					}
					else
					{
						ChangeChainOffset(_float3{ 2.f, 0.7f, 0.5f });
						Set_OriginPartPos();
					}

				}


			});
	CHKFAIL(hr)

		hr = AddTransitionFunc(BS_GIANTHAND, BS_IDLE, [=]()
			{
				if (!m_iRePeatCount && !m_pGiantHand[0]->IsActive())
				{
					return m_iCurSetNumber >= 4;
				}
				if (m_iRePeatCount)
					return !m_pGiantHand[0]->IsActive();
				return false;
			});
	CHKFAIL(hr)

		hr = AddTransitionFunc(BS_GIANTHAND, BS_JUMP, [=]()
			{
				if (!m_iRePeatCount && !m_pGiantHand[0]->IsActive())
				{
					return m_iCurSetNumber < 4;
				}
				else
					return false;
			});
	CHKFAIL(hr)

		hr = AddStateFunc(BS_GIANTHAND, EKeyActionState::Exit, [=](_float _fDeltaTime)
			{
				m_vecSpawnPoint.clear();
				if (!m_pWaitTimer->IsActive()) m_pWaitTimer->Restart();
				m_fChainMaxDistance = 2.5f;
				//if (m_bIsRight)
				//{
				//	/*OriginFaceY();
				//	OriginFaceZ();
				//	*/
				//}
				//else
					Set_OriginPartPos();

				auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
			});
	CHKFAIL(hr)
		return S_OK;
}

HRESULT CYumbrozi::Set_SpinDropState()
{
	HRESULT hr;
	hr = AddStateFunc(BS_SPINDROP, EKeyActionState::Enter, [=](_float _fDeltaTime) {
		PrintDebug("SPIN DROP!!!!!");
		if (!m_pSpinTimer->IsActive() && !m_pDropTimer->IsActive() && !m_pDropWaitTimer->IsActive()
			&& !m_pReDropTimer->IsActive())
			m_pSpinTimer->Restart();


		m_bHit = false;
		});
	CHKFAIL(hr);

	hr = AddStateFunc(BS_SPINDROP, EKeyActionState::Stay, [=](_float _fDeltaTime)
		{

			if (!m_pSpinTimer->IsActive() && m_bIsDropable && !m_pDropTimer->IsActive())
				m_pDropTimer->Restart();
			if (!m_bIsReChase && !m_pSpinTimer->IsActive() && !m_pDropTimer->IsActive() && !m_bIsDropable && !m_pDropWaitTimer->IsActive())
				m_pDropWaitTimer->Restart();

			_float2 _ContactPos1;
			_float2 _vRayDir1 = { 0.f, -1.f };
			auto _OriginPos1 = m_pTransformCom->Get_WorldState(STATE::POSITION);
			_float2 _vRayPos1 = _float2{ _OriginPos1.x , _OriginPos1.y + 5 };

			FRayCastHit _hit1;
			if (m_pGameInstance->RayCast(_vRayPos1, _vRayDir1, 100.f, &_hit1, m_vecLayerMasks)) {
				_ContactPos1 = _hit1.contactPoint;
			}

			if (m_pSpinTimer->IsActive() && !m_pDropTimer->IsActive())
			{
				auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
				static auto _targetY = _vPos.y + 2.f;
				auto fRatio = m_pSpinTimer->GetElapsedRatio();
				m_pTransformCom->Rotation(_float3{ 0.f, 1.f, 0.f }, 5 * 2 * D3DX_PI * fRatio);
				_vPos.y = fRatio * _targetY + (1 - fRatio) * _vPos.y;
				m_pTransformCom->Set_State(STATE::POSITION, _vPos);
			}
			if (!m_pSpinTimer->IsActive() && m_pDropTimer->IsActive())
			{
				

				auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
				_float3 _targetPos = static_cast<CTransform*>(m_pTarget->Get_Component(L"Com_Transform"))->Get_State(STATE::POSITION);
				_float _targetY = _targetPos.y;
				auto fRatio = m_pDropTimer->GetElapsedRatio();
				_vPos.y = fRatio * _targetY + (1 - fRatio) * _vPos.y;
				m_pTransformCom->Set_State(STATE::POSITION, _vPos);
				m_pColliderCom->SetActive(true);
				if (1 - fRatio <= 0.02) 
				{
					
					if (dynamic_cast<CBlock_Spike*>(_hit1.pObject))
					{
					}
					else 
					{
						_float3 _vpos = { _hit1.contactPoint.x, _hit1.contactPoint.y, -0.f };
						PlaySquashSound();
						PlaySquashParticle(_vpos);
					}
					
				}
			}
			if (m_pDropWaitTimer->IsActive() && m_pTarget == m_pToodee)
			{
				if (!m_pReDropTimer->IsActive()) 
				{
					m_pReDropTimer->Restart();
				}
					

				_float2 _ContactPos;
				_float2 _vRayDir = { 0.f, -1.f };
				auto _OriginPos = m_pTransformCom->Get_WorldState(STATE::POSITION);
				_float2 _vRayPos = _float2{ _OriginPos.x , _OriginPos.y + 5 };

				FRayCastHit _hit;
				if (m_pGameInstance->RayCast(_vRayPos, _vRayDir, 100.f, &_hit, m_vecLayerMasks)) {
					_ContactPos = _hit.contactPoint;
				}


				auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
				auto _targetY = _ContactPos.y + 2;
				auto fRatio = m_pReDropTimer->GetElapsedRatio();
				_vPos.y = fRatio * _targetY + (1 - fRatio) * _vPos.y;
				m_pTransformCom->Set_State(STATE::POSITION, _vPos);
				m_pColliderCom->SetActive(true);
			}

		});
	CHKFAIL(hr);


	hr = AddStateFunc(BS_SPINDROP, EKeyActionState::Exit, [=](_float _fDeltaTime)
		{
			SetTexureIndex(BP_LFOOT, 4);
			SetTexureIndex(BP_RFOOT, 4);
			SetTexureIndex(BP_LHAND, 4);
			SetTexureIndex(BP_RHAND, 4);

			Set_OriginPartPos();
			m_bIsReChase = false;
		});
	CHKFAIL(hr)

		hr = AddTransitionFunc(BS_SPINDROP, BS_HIT, [=]()
			{
				return  m_bHit;
			});
	CHKFAIL(hr)

		hr = AddTransitionFunc(BS_SPINDROP, BS_CHASE, [=]()
			{
				return m_bIsReChase && !m_pReDropTimer->IsActive();
			});
	CHKFAIL(hr)

		return S_OK;
}

HRESULT CYumbrozi::SetHitState()
{
	HRESULT hr;
	hr = AddStateFunc(BS_HIT, EKeyActionState::Enter, [=](_float _fDeltaTime) {
		HitBlink_Reset();
		m_pHitTimer->Restart();
		m_iHP--;
		m_iCurSetNumber = 0;
		m_iJumpCount = 0;
		});
	CHKFAIL(hr)

		hr = AddStateFunc(BS_HIT, EKeyActionState::Exit, [=](_float _fDeltaTime)
			{
				m_bHit = false;
				auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
				auto fRatio = m_pHitTimer->GetElapsedRatio();
				_vPos = fRatio * PatternJumpPos0 + (1 - fRatio) * _vPos;
				m_pTransformCom->Set_State(STATE::POSITION, _vPos);
			});
	CHKFAIL(hr)
		hr = AddTransitionFunc(BS_HIT, BS_IDLE, [=]() {return !m_pHitTimer->IsActive(); });
	CHKFAIL(hr)
		return S_OK;
}

HRESULT CYumbrozi::SetDieState()
{
	return S_OK;
}

HRESULT CYumbrozi::SetIdleState()
{
	HRESULT hr;
	hr = AddStateFunc(BS_IDLE, EKeyActionState::Enter, [=](_float _fDeltaTime)
		{
			m_iJumpIdx = 0;
		});
	CHKFAIL(hr)


		hr = AddStateFunc(BS_IDLE, EKeyActionState::Stay, [=](_float _fDeltaTime)
			{
				IdleAnim(_fDeltaTime);

			});
	CHKFAIL(hr)

		hr = AddStateFunc(BS_IDLE, EKeyActionState::Exit, [=](_float _fDeltaTime)
			{

			});
	CHKFAIL(hr)

		hr = AddTransitionFunc(BS_IDLE, BS_CHASE, [=]()
			{
				if (m_iCurSetNumber && !m_pWaitTimer->IsActive())
					return true;
				return false;
			});
	CHKFAIL(hr)

		hr = AddTransitionFunc(BS_IDLE, BS_JUMP, [=]()
			{
				return  !m_iCurSetNumber && !m_bIsWait && !m_bIsReset;
			});
	CHKFAIL(hr)

		return S_OK;
}


void CYumbrozi::HitBlink_Reset()
{
	for (_uint i = 0; i < BP_END; i++) for (auto& _pPart : m_pListBodyParts[i]) _pPart->Hit_Reset();
}

_bool CYumbrozi::IsArrived()
{
	_float3 _vDir = m_vTargetPos - m_pTransformCom->Get_State(STATE::POSITION);

	_float _fDist = D3DXVec3Length(&_vDir);

	bool _bIsArrived = false;
	if (_fDist < MoveThreshold) {
		m_pTransformCom->Set_State(STATE::POSITION, m_vTargetPos);
		_bIsArrived = true;
	}
	if (_bIsArrived && !m_pJumpTimer->IsActive())
		_bIsArrived = true;
	else
		_bIsArrived = false;
	return _bIsArrived;
}

void CYumbrozi::Wave()
{
	for (int i = 0; i < WP_END; i++)
	{
		;
		if (m_pWaveTimer[i]->IsActive())
		{
			if (i == 0)
				m_bIsWave = true;
			m_fWaveRadius = 3 + i;
			m_iResolution = m_iWaveCnt[i];
			WAVEPHASE eState = static_cast<WAVEPHASE>(i);
			WaveSpawn(eState);
		}
	}
}

void CYumbrozi::WaveSpawn(WAVEPHASE eWP)
{

	_float3 _targetPos;

	if (m_pWave && m_pWaveTimer[eWP]->IsActive() && !m_bIsReset)
	{
		for (_uint i = 0; i < m_iResolution; i++)
		{
			if (i > m_iResolution / 3 && i < (m_iResolution * 2) / 3)
				continue;
			m_pWaveMainTransform->Set_State(STATE::POSITION, m_fWaveOriginPos);
			_targetPos = _float3{ sin(D3DX_PI * 2.f * i / m_iResolution) * m_fWaveRadius, cos(D3DX_PI * 2.f * i / m_iResolution) * m_fWaveRadius, 0.f };
			_targetPos.x = round(_targetPos.x);
			_targetPos.y = round(_targetPos.y);

			for (_uint j = 0; j < WAVECNT; j++)
			{
				if (!m_pWave[j]->IsActive())
				{
					m_pWave[j]->StartWave(_targetPos);
					break;
				}
			}


		}
	}
}


CYumbrozi* CYumbrozi::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CYumbrozi* pInstance = new CYumbrozi(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CYumbrozi");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYumbrozi::Clone(void* pArg)
{
	CYumbrozi* pInstance = new CYumbrozi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CYumbrozi");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYumbrozi::Free()
{
	__super::Free();

	for (_uint i = 0; i < BP_END; i++) Safe_Release(m_pivotTransformComs[i]);
	for (_uint i = 0; i < BP_END; i++)
	{
		m_pListBodyParts[i].clear();
	}
	for (_uint i = 0; i < MAXGIANTHANDCNT; i++)   Safe_Release(m_pGiantHand[i]);
	for (_uint i = 0; i < WAVECNT; i++) Safe_Release(m_pWave[i]);
	Safe_Release(m_pWaveMainTransform);
	Safe_Release(m_pBodyTargetTransform);


}

void CYumbrozi::OnCollision(FCollisionInfo _fCollisionInfo)
{
	__super::OnCollision(_fCollisionInfo);
	m_bIsCol = true;

	if (_fCollisionInfo.strCollisionLayerTag == L"SPIKE")
	{
		PrintDebug("SPIKE!!");
		m_bHit = true;
	}
}

HRESULT CYumbrozi::AddBodyPart(CYumbrozi_Parts* _pBodyPart, EBOSSPART _eBossPart)
{
	if (_eBossPart >= BP_END)                                                       return E_FAIL;
	if (FAILED(_pBodyPart->SetParent(this)))                                        return E_FAIL;
	if (FAILED(_pBodyPart->SetParentTransform(m_pivotTransformComs[_eBossPart])))   return E_FAIL;
	m_pListBodyParts[_eBossPart].push_back(_pBodyPart);
	return S_OK;
}

HRESULT CYumbrozi::AddLimbPart()
{
	for (auto& _pPart : m_pListBodyParts[BP_RULIMB])
	{
		_pPart->SetParentTransform(m_pivotTransformComs[BP_RSHOULDER]);
		_pPart->SetHandTransform(m_pivotTransformComs[BP_RHAND]);
	}
	for (auto& _pPart : m_pListBodyParts[BP_LULIMB])
	{
		_pPart->SetParentTransform(m_pivotTransformComs[BP_LSHOULDER]);
		_pPart->SetHandTransform(m_pivotTransformComs[BP_LHAND]);
	}
	for (auto& _pPart : m_pListBodyParts[BP_RDLIMB])
	{
		_pPart->SetParentTransform(m_pivotTransformComs[BP_RPELVIS]);
		_pPart->SetHandTransform(m_pivotTransformComs[BP_RFOOT]);
	}
	for (auto& _pPart : m_pListBodyParts[BP_LDLIMB])
	{
		_pPart->SetParentTransform(m_pivotTransformComs[BP_LPELVIS]);
		_pPart->SetHandTransform(m_pivotTransformComs[BP_LFOOT]);
	}
	return S_OK;
}

HRESULT CYumbrozi::CreateResource(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{

	auto iPrototypeLevelIndex = ENUM_TO_UINT(LEVEL::BOSS_STAGE2);
	auto strPrototypeTag = TEXT("Prototype_GameObject_Yumbrozi_GiantHand");
	auto strPrototypeTag2 = TEXT("Prototype_GameObject_Yumbrozi_Wave");

	for (_uint i = 0; i < MAXGIANTHANDCNT; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag,
			iLayerLevelIndex, strLayerTag, nullptr, reinterpret_cast<CGameObject**>(&m_pGiantHand[i]))))
			return E_FAIL;
	}

	for (_uint i = 0; i < WAVECNT; i++)
	{

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag2,
			iLayerLevelIndex, strLayerTag, nullptr, reinterpret_cast<CGameObject**>(&m_pWave[i]))))
			return E_FAIL;
		m_pWave[i]->SetParent(this);
	}

	return S_OK;
}

HRESULT CYumbrozi::AdjustEyeScale(const _float& fRatio)
{
	_float _fScale = fRatio * 17.f * g_fDot + 0.01f;
	for (_uint i = BP_LEYE; i <= BP_REYE; i++)
	{
		for (auto& _pPart : m_pListBodyParts[i]) {
			static_cast<CTransform*>(_pPart->Get_Component(L"Com_Transform"))->Scale(_fScale, _fScale, _fScale);
		}
	}
	return S_OK;
}

HRESULT CYumbrozi::AdjustLocation(EBOSSPART _ePart, _float3 _Offset)
{
	auto _vPos = m_pivotTransformComs[_ePart]->Get_State(STATE::POSITION);
	_vPos.x += _Offset.x;
	_vPos.y += _Offset.y;
	_vPos.z += _Offset.z;
	m_pivotTransformComs[_ePart]->Set_State(STATE::POSITION, _vPos);

	return S_OK;
}

HRESULT CYumbrozi::PartsTurn(EBOSSPART _ePart, _float3 eAxis, _float RotSpeed)
{
	for (auto& _pPart : m_pListBodyParts[_ePart])
	{
		static_cast<CTransform*>(_pPart->Get_Component(L"Com_Transform"))->Turn(eAxis, RotSpeed);
	}


	return E_NOTIMPL;
}

void CYumbrozi::IdleAnim(_float fTimeDelta)
{
	_float HeadOffset = sin(D3DX_PI * 1 * m_pAnimIdleTimer->GetElapsedTime());
	_float BodyOffset = sin(D3DX_PI * 2 * m_pAnimIdleTimer->GetElapsedTime());
	PartsTurn(BP_HEAD, _float3{ 0,0, HeadOffset }, fTimeDelta * fTimeDelta);
	PartsTurn(BP_BODY, _float3{ 0,0, BodyOffset }, fTimeDelta * fTimeDelta);
}

void CYumbrozi::ScreamAnim(_float fTimeDelta)
{
	_float HeadOffset = sin(D3DX_PI * 4 * m_pAnimScreamTimer->GetElapsedTime());
	_float BodyOffset = sin(D3DX_PI * 3 * m_pAnimScreamTimer->GetElapsedTime());
	PartsTurn(BP_HEAD, _float3{ 0,0, HeadOffset }, fTimeDelta * fTimeDelta);
	PartsTurn(BP_BODY, _float3{ 0,0, BodyOffset }, fTimeDelta * fTimeDelta);
}

void CYumbrozi::OriginPosHead()
{
	m_pivotTransformComs[BP_HEAD]->Set_State(STATE::POSITION, _float3{ 0.f,  1.f,   -0.15f });
	RecoverPos(BP_LSHOULDER);
	RecoverPos(BP_RSHOULDER);
	RecoverPos(BP_LPELVIS);
	RecoverPos(BP_RPELVIS);
}

void CYumbrozi::OriginFaceY()
{
	for (_uint i = BP_LEYE; i < BP_BODY; i++)
	{
		auto _vPos = m_pivotTransformComs[i]->Get_State(STATE::POSITION);
		_vPos.y = m_iPartsOriginY[i];
		m_pivotTransformComs[i]->Set_State(STATE::POSITION, _vPos);
	}
}

void CYumbrozi::OriginFaceX()
{
	for (_uint i = BP_LEYE; i < BP_BODY; i++)
	{
		auto _vPos = m_pivotTransformComs[i]->Get_State(STATE::POSITION);
		_vPos.x = m_iPartsOriginX[i];
		m_pivotTransformComs[i]->Set_State(STATE::POSITION, _vPos);
	}
}

void CYumbrozi::OriginFaceZ()
{
	for (_uint i = BP_HEAD; i < BP_END; i++)
	{
		auto _vPos = m_pivotTransformComs[i]->Get_State(STATE::POSITION);
		_vPos.z = m_iPartsOriginZ[i];
		m_pivotTransformComs[i]->Set_State(STATE::POSITION, _vPos);
	}
}

void CYumbrozi::FlipOriginFaceX()
{
	for (_uint i = BP_LEYE; i < BP_BODY; i++)
	{

		auto _vPos = m_pivotTransformComs[i]->Get_State(STATE::POSITION);
		_vPos.x = m_iFaceFlipX[i];
		if (i == BP_NOSE)
			_vPos.z -= 0.1f;
		if (i == BP_LBROW || i == BP_RBROW || i == BP_LEYE || i == BP_REYE)
		{
			if (i == BP_LBROW)
				_vPos.x = m_iFaceFlipX[BP_RBROW];
			if (i == BP_RBROW)
				_vPos.x = m_iFaceFlipX[BP_LBROW];

			if (i == BP_LEYE)
				_vPos.x = m_iFaceFlipX[BP_REYE];
			if (i == BP_REYE)
				_vPos.x = m_iFaceFlipX[BP_LEYE];

		}
		m_pivotTransformComs[i]->Set_State(STATE::POSITION, _vPos);
	}
}

void CYumbrozi::Set_OriginPartPos()
{
	for (_uint i = BP_HEAD; i < BP_TAIL0; i++)
	{

		for (auto& _pPart : m_pListBodyParts[i])
		{
			auto partTransform = static_cast<CTransform*>(_pPart->Get_Component(L"Com_Transform"));
			if (i == BP_LBROW || i == BP_RBROW || i == BP_LEYE || i == BP_REYE)
				continue;
			_pPart->Flip(false);
		}
		auto part = static_cast<EBOSSPART>(i);
		RecoverPos(part);
	}
}

void CYumbrozi::LocationHead(_float x, _float y, _float z)
{
	AdjustLocation(BP_HEAD, _float3{ x, y ,z });
	AdjustLocation(BP_JAW, _float3{ x, y ,z });
	AdjustLocation(BP_FACE, _float3{ x, y ,z });
	AdjustLocation(BP_LEYE, _float3{ x, y ,z });
	AdjustLocation(BP_REYE, _float3{ x, y ,z });
	AdjustLocation(BP_LBROW, _float3{ x, y ,z });
	AdjustLocation(BP_RBROW, _float3{ x, y ,z });
	AdjustLocation(BP_NOSE, _float3{ x, y ,z });
	AdjustLocation(BP_RSHOULDER, _float3{ 0, y ,0 });
	AdjustLocation(BP_LSHOULDER, _float3{ 0, y ,0 });
	AdjustLocation(BP_LPELVIS, _float3{ 0, y ,0 });
	AdjustLocation(BP_RPELVIS, _float3{ 0, y ,0 });
}
void CYumbrozi::LocationFace(_float x, _float y, _float z)
{
	AdjustLocation(BP_JAW, _float3{ x, y ,z });
	AdjustLocation(BP_FACE, _float3{ x, y ,z });
	AdjustLocation(BP_LEYE, _float3{ x + 1, y ,z });
	AdjustLocation(BP_REYE, _float3{ x + 1, y ,z });
	AdjustLocation(BP_LBROW, _float3{ x + 1, y ,z });
	AdjustLocation(BP_RBROW, _float3{ x + 1, y ,z });
	AdjustLocation(BP_NOSE, _float3{ x + 1, y ,z });
}

void CYumbrozi::UpdateBodyChain(_float fTimeDelta)
{
	_float3 _vOriginPos, _vNxtPos, _vCurPos, _vLerpPos;

	_vOriginPos = m_pivotTransformComs[BP_HEAD]->Get_WorldState(STATE::POSITION);
	_vNxtPos = m_pBodyTargetTransform->Get_WorldState(STATE::POSITION);
	_vCurPos = m_pivotTransformComs[BP_BODY]->Get_State(STATE::POSITION);

	_double _ratio = pow((pow(10, -60)), static_cast<_double>(fTimeDelta));
	_vLerpPos = _vCurPos * (1 - _ratio) + _vNxtPos * (_ratio);
	auto _vDist = _vLerpPos - _vOriginPos;
	_float _fLength = min(D3DXVec3Length(&_vDist), m_fChainMaxDistance);

	D3DXVec3Normalize(&_vDist, &_vDist);
	_vDist *= _fLength;

	_vLerpPos = _vOriginPos + _vDist;

	m_pivotTransformComs[BP_BODY]->Set_State(STATE::POSITION, _vLerpPos);


}

void CYumbrozi::UpdateTailChain(_float fTimeDelta)
{

	auto _vTailDir = m_pivotTransformComs[BP_BODY]->Get_WorldState(STATE::POSITION) - m_pivotTransformComs[BP_HEAD]->Get_WorldState(STATE::POSITION);
	_vTailDir.z = 0;
	D3DXVec3Normalize(&_vTailDir, &_vTailDir);
	auto _vPos = m_pivotTransformComs[BP_BODY]->Get_WorldState(STATE::POSITION);
	if (_vTailDir.y < -cosf(D3DX_PI / 6)) {
		_vTailDir.y = -cosf(D3DX_PI / 6);

		if (m_bIsRight)	_vTailDir.x = -sinf(D3DX_PI / 6);
		else			_vTailDir.x = +sinf(D3DX_PI / 6);

	}
	_vPos += _vTailDir * 2.4f;
	m_pivotTransformComs[BP_TAIL0]->Set_State(STATE::POSITION, _vPos);

	_float3 _vOriginPos, _vNxtPos, _vCurPos, _vLerpPos;



	auto _fRatio = m_pTailTimer->GetElapsedRatio();
	_float _fAngle = D3DX_PI / 2 * sinf(_fRatio * D3DX_PI * 2.f);
	_fAngle += atan2(_vTailDir.y, _vTailDir.x);
	_float3 _vDir = _float3{ cosf(_fAngle), sinf(_fAngle), 0.f };
	_vOriginPos = m_pivotTransformComs[BP_TAIL0]->Get_WorldState(STATE::POSITION);

	for (_uint i = BP_TAIL1; i <= BP_TAIL16; i++)
	{
		_vNxtPos = _vOriginPos + _vDir;
		_vCurPos = m_pivotTransformComs[i]->Get_State(STATE::POSITION);

		_double _ratio = pow((pow(10, -40)), static_cast<_double>(fTimeDelta));
		_vLerpPos = _vCurPos * (1 - _ratio) + _vNxtPos * (_ratio);
		auto _vDist = _vLerpPos - _vOriginPos;
		//_float _fLength = min(D3DXVec3Length(&_vDist), m_fTailChainMaxDistance);
		_float _fLength = m_fTailChainMaxDistance;

		D3DXVec3Normalize(&_vDist, &_vDist);
		_vDist *= _fLength;

		_vLerpPos = _vOriginPos + _vDist;
		m_pivotTransformComs[i]->Set_State(STATE::POSITION, _vLerpPos);

		_vDir = _vDist + _float3{ 0.f, -0.1f, 0.f };
		_vOriginPos = m_pivotTransformComs[i]->Get_WorldState(STATE::POSITION);
	}


}

void CYumbrozi::FlipFace()
{
	for (_uint i = BP_LEYE; i < BP_BODY; i++)
	{
		auto _vPos = m_pivotTransformComs[i]->Get_State(STATE::POSITION);
		_vPos.x *= -1.f;
		m_pivotTransformComs[i]->Set_State(STATE::POSITION, _vPos);
	}
}

void CYumbrozi::ChangeChainOffset(_float3 Offset)
{
	m_pBodyTargetTransform->Set_State(STATE::POSITION, Offset);
}

void CYumbrozi::RotationParts(EBOSSPART ePart, _float3 Axis, _float angle)
{
	for (auto& _pPart : m_pListBodyParts[ePart])
	{
		static_cast<CTransform*>(_pPart->Get_Component(L"Com_Transform"))->Rotation(Axis, D3DXToRadian(angle));
	}

}

void CYumbrozi::RecoverPos(EBOSSPART ePart)
{
	m_pivotTransformComs[ePart]->Set_State(STATE::POSITION,
		_float3{ m_iPartsOriginX[ePart],m_iPartsOriginY[ePart],m_iPartsOriginZ[ePart] });
}

HRESULT CYumbrozi::SetTexureIndex(EBOSSPART ePart, _uint idx)
{
	for (auto& _pPart : m_pListBodyParts[ePart])
	{
		_pPart->SetTexture(idx);
		_pPart->IsAnimActive(false);
	}
	return S_OK;
}

HRESULT CYumbrozi::ActiveAnim(EBOSSPART ePart)
{
	for (auto& _pPart : m_pListBodyParts[ePart])
	{
		_pPart->IsAnimActive(true);
	}
	return S_OK;
}
