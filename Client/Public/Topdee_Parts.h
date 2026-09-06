#pragma once
#include "Client_Defines.h" 
#include "GameObject.h"
#include "Topdee.h"


NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CTopdee;
class CDimensionSwitcher;
NS_END

NS_BEGIN(Client)
class CTopdee_Parts abstract :public CGameObject
{
protected:
	CTopdee_Parts(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTopdee_Parts(const CTopdee_Parts& Prototype);
	virtual ~CTopdee_Parts() = default;

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
	virtual void SetParent(CTopdee* ppGameObject) = 0;
protected:
	CTopdee* m_pParent = { nullptr };
	virtual HRESULT Ready_Components() = 0;
};
NS_END
