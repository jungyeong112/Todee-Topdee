#pragma once
#include "Client_Defines.h" 
#include "GameObject.h"
#include "TopTookee.h"


NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CTopdee;
class CDimensionSwitcher;
NS_END

NS_BEGIN(Client)
class CTookee_Parts abstract :public CGameObject
{
protected:
	CTookee_Parts(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTookee_Parts(const CTookee_Parts& Prototype);
	virtual ~CTookee_Parts() = default;

protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CDimensionSwitcher* m_pDimensionCom = { nullptr };

protected:
	_int     m_iCurTextureIdx = {};
	_float   m_fRatio = {};
	_float   m_fAngle = {};
	_float   m_fPreAngle = {};

public:
	virtual void SetParent(CTopTookee* ppGameObject) = 0;
protected:
	CTopTookee* m_pParent = { nullptr };
	virtual HRESULT Ready_Components() = 0;
};
NS_END
