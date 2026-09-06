#pragma once
#include "Block.h"

NS_BEGIN(Client)
class CTopdee;

class CBlock_Box final  : public CBlock
{
private:
	CBlock_Box(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBlock_Box(const CBlock_Box& Prototype);
	virtual ~CBlock_Box() = default;
public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;
	void Priority_Update(_float fTimeDelta)	override;
	void Update(_float fTimeDelta)			override;
	void Late_Update(_float fTimeDelta)		override;
	HRESULT Render()						override;
private:
	HRESULT	Ready_Components();
public:
	static CBlock_Box* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	void Free() override;

	// CBlock을(를) 통해 상속됨
	virtual Engine::CGameObject* Clone(void* pArg);

};
NS_END
