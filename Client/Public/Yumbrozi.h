#pragma once

#include "Boss.h"
#include "Time_Manager_API.h"

NS_BEGIN(Client)

class CYumbrozi_Parts;

class CYumbrozi : public CBoss
{
	const  static	_uint	MAXGIANTHANDCNT = 2;
	const  static   _uint   WAVECNT         = 300;

	const inline static _float	ZOffset = -1.f;
	const inline static _float  JumpYPos = 7.f;
	const inline static _float  JumpXOffset = 5.f;

	const inline static _float3 PatternJumpPos0 = _float3{ g_iMapSizeX - JumpXOffset ,	JumpYPos,   ZOffset }; // 오른쪽 지형
	const inline static _float3 PatternJumpPos1 = _float3{ g_iMapSizeX * 0.5f,	JumpYPos,   ZOffset }; // 중앙 지형
	const inline static _float3 PatternJumpPos2 = _float3{ JumpXOffset,		JumpYPos,   ZOffset }; // 왼쪽 지형


	const inline static _float3 PatternJumpPos[4] =
	{
		PatternJumpPos1,
		PatternJumpPos2,
		PatternJumpPos1,
		PatternJumpPos0
	};   //점프는 무조건 가운데 - 왼쪽 - 가운데 - 오른쪽 반복

	const inline static _float3 SpawnPos[3] =
	{
		PatternJumpPos0,
		PatternJumpPos1,
		PatternJumpPos2,
	};   // 저장용
	 
	const inline static _float	MoveThreshold = 1.f;

public:
	enum EBOSSSTATE
	{
		BS_IDLE,
		BS_SCREAM,
		BS_HIT,
		BS_DIE,

		BS_CHASE,
		BS_JUMP,

		BS_GIANTHAND,
		BS_SHOCKWAVE,
		BS_SPINDROP,

		BS_END
	};
	enum EBOSSPART
	{
		BP_HEAD,

		BP_LEYE,
		BP_REYE,

		BP_NOSE,

		BP_LBROW,
		BP_RBROW,

		BP_FACE,
		BP_JAW,

		BP_LHAND,
		BP_RHAND,

		BP_LFOOT,
		BP_RFOOT,

		BP_BODY,

		BP_LSHOULDER,
		BP_RSHOULDER,

		BP_LPELVIS,
		BP_RPELVIS,

		BP_LULIMB,
		BP_RULIMB,
		BP_LDLIMB,
		BP_RDLIMB,



		BP_TAIL0,
		BP_TAIL1,
		BP_TAIL2,
		BP_TAIL3,
		BP_TAIL4,
		BP_TAIL5,
		BP_TAIL6,
		BP_TAIL7,
		BP_TAIL8,
		BP_TAIL9,
		BP_TAIL10,
		BP_TAIL11,
		BP_TAIL12,
		BP_TAIL13,
		BP_TAIL14,
		BP_TAIL15,
		BP_TAIL16,


		BP_END
	};

	enum WAVEPHASE 
	{
		WP_1,
		WP_2,
		WP_3,
		WP_4,
		WP_5,
		WP_6,
		WP_7,
		WP_8,
		WP_9,
		WP_10,
		WP_11,
		WP_12,
		WP_13,
		WP_14,

		WP_END
	};
	inline static _uint NUMPATTERNGROUP = 2;
	//inline static _uint arrPattern1Group[] = { BS_JUMP , BS_SHOCKWAVE}; // BS_SCREAM, BS_GIANTHAND , BS_CHASE, BS_SPINDROP
	//inline static _uint arrPattern2Group[] = { BS_HIT, BS_DIE, BS_IDLE };

protected:

	CYumbrozi(LPDIRECT3DDEVICE9 pGraphic_Device);
	CYumbrozi(const CYumbrozi& Prototype);
	virtual ~CYumbrozi() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;

	virtual void	Priority_Update(_float fTimeDelta)	override;
	virtual void	Update(_float fTimeDelta)			override;
	virtual void	Late_Update(_float fTimeDelta)		override;
private:
	virtual HRESULT	Ready_Components()					override;
	virtual HRESULT Reset()                             override;

	HRESULT SetJumpState();
	HRESULT Set_ChaseState();

	HRESULT Set_ShockWaveState();
	HRESULT Set_ScreamState();
	HRESULT Set_GiantHandState();
	HRESULT Set_SpinDropState();

	HRESULT SetHitState();
	HRESULT SetDieState();
	HRESULT SetIdleState();

	

public:
	void HitBlink_Reset();
private: 
	_bool IsArrived();
	void  Wave();
	void  WaveSpawn(WAVEPHASE eWP);


private:
	CTransform*              m_pivotTransformComs[BP_END];
	CTransform*              m_pWaveMainTransform = { nullptr };
	CTransform*              m_pBodyTargetTransform = { nullptr };
	list<CYumbrozi_Parts*>   m_pListBodyParts[BP_END];

	_uint                    m_iJumpIdx = 0;
	_float3                  m_vTargetPos   = {};
	_uint                    m_iJumpCount   = {};
	_uint                    m_iRePeatCount = {};
	_uint                    m_iCurSetNumber= {};
	_float3                  m_vChaseDir    = {}; 

	_uint                    m_iOriginX = {};
	_uint                    m_iOriginY = {};
	_int                     m_iMoveX   = {};
	_int                     m_iMoveY   = {};

	_int                     m_iStandOriginY = {};
	_int                     m_iStandOriginX = {};

	vector<_float3>                     m_vecSpawnPoint = {};
	class  CYumbrozi_GiantHand*         m_pGiantHand[MAXGIANTHANDCNT]  =  { nullptr };
	class  CYumbrozi_Wave*              m_pWave [WAVECNT]              =  { nullptr };
	class CPSystem_Dust* m_pSystemDust = { nullptr };
	CGameObject* m_pTarget = { nullptr };
	vector<wstring>					m_vecLayerMasks;

public:
	static  CYumbrozi*   Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void		 Free()				override;
	void                 OnCollision(FCollisionInfo  _fCollisionInfo) override;

	HRESULT AddBodyPart(CYumbrozi_Parts* _pBodyPart, EBOSSPART _eBossPart);
	HRESULT AddLimbPart();

	HRESULT	CreateResource(_uint iLayerLevelIndex, const _wstring& strLayerTag);

	HRESULT AdjustEyeScale(const _float& fRatio);
	HRESULT AdjustLocation(EBOSSPART _ePart, _float3 _Offset);

	HRESULT PartsTurn(EBOSSPART _ePart, _float3 eAxis, _float RotSpeed);
	void    IdleAnim(_float fTimeDelta);
	void    ScreamAnim(_float fTimeDelta);

	void    OriginPosHead();
	void    OriginFaceY();
	void    OriginFaceX();
	void    OriginFaceZ();
	void    FlipOriginFaceX();
	void    Set_OriginPartPos();
	void    LocationHead(_float x,_float y, _float z);
	void    LocationFace(_float x, _float y, _float z);
	void	UpdateBodyChain(_float fTimeDelta);
	void	UpdateTailChain(_float fTimeDelta);
	void    FlipFace();
	void    ChangeChainOffset(_float3 Offset);
	void    RotationParts(EBOSSPART ePart,_float3 Axis,_float angle);

	void	RecoverPos(EBOSSPART ePart);
	HRESULT SetTexureIndex(EBOSSPART ePart, _uint idx);
	HRESULT ActiveAnim(EBOSSPART ePart);
private:

	//PatternTimer
	UniqueTimer         m_pChaseTimer             = { nullptr };
	UniqueTimer         m_pWaveTimer[WP_END]      = { nullptr };
	UniqueTimer         m_pHitTimer               = { nullptr };
	UniqueTimer         m_pSpinTimer              = { nullptr };
	UniqueTimer         m_pDropWaitTimer          = { nullptr };
	UniqueTimer         m_pDropTimer              = { nullptr };
	UniqueTimer         m_pReDropTimer            = { nullptr };
	UniqueTimer         m_pWaitTimer              = { nullptr };
	UniqueTimer         m_pJumpTimer              = { nullptr };
	UniqueTimer         m_pIdleTimer              = { nullptr };

	//AnimTimer 
	UniqueTimer         m_pAnimIdleTimer          = { nullptr };
	UniqueTimer         m_pAnimScreamTimer        = { nullptr };
	UniqueTimer         m_pAnimStandTimer         = { nullptr };
	UniqueTimer         m_pAnimGiantTimer         = { nullptr };

	_bool               m_bHit            = { false };
	_bool               m_bIsWave         = { false };
	_bool               m_bIsDropable     = { false };
	_bool               m_bIsReChase      = { false };
	_bool               m_bIsCol          = { false };
	_bool               m_bIsReset        = { false };
	_bool               m_bIsWait         = { true };

	_float              m_fWaveRadius     = {};
	_uint               m_iResolution     = {};
	_float3             m_fWaveOriginPos  = {};
	_float              m_fChainMaxDistance = {2.5f};
	_float              m_fTailChainMaxDistance = {.7f};
	_uint               m_iWaveCnt[WAVEPHASE::WP_END] = {};
	_float              m_iPartsOriginY[BP_END] = {};
	_float              m_iPartsOriginX[BP_END] = {};
	_float              m_iPartsOriginZ[BP_END] = {};
	_float              m_iFaceFlipX[BP_END] = {};
	_float             m_fTopdimOffset = { -2 };
	_float             m_fToodimOffset = { 0 };

	_bool				m_bIsRight = false;

	UniqueTimer			m_pTailTimer = { nullptr };
};
NS_END