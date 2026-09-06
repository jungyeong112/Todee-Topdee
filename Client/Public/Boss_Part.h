#pragma once
#include "Client_Defines.h" 
#include "GameObject.h"
#include "Topdee.h"
#include "Time_Manager_API.h"


NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CDimensionSwitcher;
NS_END

NS_BEGIN(Client)
class CBoss;

//Add Texture Only in Children Class
class CBoss_Part abstract :public CGameObject
{
	constexpr static _float HITBLINKFREQ	= 6.f;
	constexpr static _uint	HITBLINKCNT		= 14;

protected:
	CBoss_Part(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_Part(const CBoss_Part& Prototype);
	virtual ~CBoss_Part() = default;
public:
	virtual HRESULT Initialize_Prototype()				override; /* 원형을 위한 초기화(서버, 파일) */
	virtual HRESULT Initialize(void* pArg)				override; /* 사본객체를 위한 초기화 */
	virtual void	Update(_float fTimeDelta)			override;
	virtual void	Late_Update(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

protected:
	CTexture*			m_pTextureCom		= { nullptr };
	CDimensionSwitcher* m_pDimensionCom		= { nullptr };
	CTransform*			m_pTransformCom		= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom		= { nullptr };

public:
	virtual HRESULT SetParent(CBoss* ppGameObject);
	HRESULT		Hit_Render();
	void		Hit_Reset();
protected:
	class CBoss* m_pParent = { nullptr };
	virtual HRESULT Ready_Components() PURE;
	virtual void			Free()						override;
private:
	UniqueTimer	m_pHitTimer = nullptr;
	_uint		m_iBlinkCnt = 0;
	_bool		m_bBlink	= false;
};
NS_END
