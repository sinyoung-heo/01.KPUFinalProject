#include "stdafx.h"
#include "DmgFont.h"
#include "GraphicDevice.h"
#include "DescriptorHeapMgr.h"
#include "DynamicCamera.h"

CDmgFont::CDmgFont(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

void CDmgFont::Set_DamageList(const _uint& uiDmg)
{
	if (uiDmg > DMG_MAX)
		m_uiDamage = DMG_MAX;
	else
		m_uiDamage = uiDmg;
	m_lstDamage.clear();

	while (true)
	{
		/*____________________________________________________________________
		1. 1의 자리수 값을 얻어온다.
		ex) 978 -> 8의 값을 얻어옴.
		______________________________________________________________________*/
		_int iNum	= m_uiDamage % 10;

		/*____________________________________________________________________
		2. 얻어온 1의 자리 값을 list에 담아준다.
		앞에서 부터 넣어준다.
		ex) 978 -> list 순회 순서 9,  7,  8
		______________________________________________________________________*/
		m_lstDamage.push_front(iNum);

		/*____________________________________________________________________
		3. 1의 자리를 짤라낸다.
		ex) Dmg = 978 / 10  -> 97
		______________________________________________________________________*/
		m_uiDamage = m_uiDamage / 10;

		/*____________________________________________________________________
		4. 1의 자리를 계속 자르다가 자리수가 1개만 남았을 경우,
		ex) 9 / 10 = 0.   9 % 10 = 9.
		______________________________________________________________________*/
		if (m_uiDamage / 10 == 0)
		{
			m_lstDamage.push_front(m_uiDamage % 10);
			m_uiDmgListSize = m_lstDamage.size();
			break;
		}
	}
}

HRESULT CDmgFont::Ready_GameObject(const _vec3& vPos,
								   const _vec3& vScale,
								   const _uint& uiDmg,
								   const DMG_TYPE& eType)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Read_DataFromFilePath(), E_FAIL);

	m_pTransCom->m_vPos   = vPos;
	m_pTransCom->m_vScale = vScale;
	m_eDmgType            = eType;
	Set_DamageList(uiDmg);

	return S_OK;
}

HRESULT CDmgFont::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer(DMG_SIZE + 1);

	return S_OK;
}

_int CDmgFont::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_ALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Update Billboard Matrix ]
	____________________________________________________________________________________________________________*/
	Make_BillboardMatrix(m_pTransCom->m_matWorld, m_pTransCom->m_vScale);
	//Engine::CGameObject::SetUp_BillboardMatrix();

	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);

	return NO_EVENT;
}

_int CDmgFont::LateUpdate_GameObject(const _float& fTimeDelta)
{
	SetUp_DmgFontWorldMatrix();

	return NO_EVENT;
}

void CDmgFont::Render_GameObject(const _float& fTimeDelta)
{
	if (DMG_TYPE::DMGTYPE_END == m_eDmgType)
		return;

	/*__________________________________________________________________________________________________________
	[ Damage Font Back ]
	____________________________________________________________________________________________________________*/
	if (DMG_TYPE::DMG_PLAYER == m_eDmgType || DMG_TYPE::DMG_MONSTER == m_eDmgType)
	{
		_uint uiTexIdx = 0;
		if (DMG_TYPE::DMG_PLAYER == m_eDmgType)
			uiTexIdx = 5;
		else
			uiTexIdx = 4;

		// Set ConstantTable
		Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
		ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
		tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
		tCB_ShaderTexture.fFrameCnt	= 1.0f;
		tCB_ShaderTexture.fCurFrame	= 0.0f;
		tCB_ShaderTexture.fSceneCnt	= 1.0f;
		tCB_ShaderTexture.fCurScene	= 0.0f;
		m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(m_uiDmgListSize, tCB_ShaderTexture);

		// Render Buffer
		m_pShaderCom->Begin_Shader(Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"S1UI_ComboCounter"),
								   m_uiDmgListSize,
								   uiTexIdx,
								   Engine::MATRIXID::PROJECTION);

		m_pBackBufferCom->Begin_Buffer();
		m_pBackBufferCom->Render_Buffer();
	}

	/*__________________________________________________________________________________________________________
	[ Damage Font ]
	____________________________________________________________________________________________________________*/
	_uint uiIdx = 0;
	for (uiIdx = 0; uiIdx < m_uiDmgListSize; ++uiIdx)
	{
		// Set ConstantTable
		Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
		ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
		tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_matWorld[uiIdx]);
		tCB_ShaderTexture.fFrameCnt	= m_vecDmgTextureInfo[m_eDmgType][uiIdx].tFrame.fFrameCnt;
		tCB_ShaderTexture.fCurFrame	= (_float)(_int)m_vecDmgTextureInfo[m_eDmgType][uiIdx].tFrame.fCurFrame;
		tCB_ShaderTexture.fSceneCnt	= m_vecDmgTextureInfo[m_eDmgType][uiIdx].tFrame.fSceneCnt;
		tCB_ShaderTexture.fCurScene	= (_int)m_vecDmgTextureInfo[m_eDmgType][uiIdx].tFrame.fCurScene;

		m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(uiIdx, tCB_ShaderTexture);

		// Render Buffer
		m_pShaderCom->Begin_Shader(m_vecDmgTextureInfo[m_eDmgType][uiIdx].pTexDescriptorHeap,
								   uiIdx,
								   m_vecDmgTextureInfo[m_eDmgType][uiIdx].uiTexIdx,
								   Engine::MATRIXID::PROJECTION);

		m_arrBufferCom[uiIdx]->Begin_Buffer();
		m_arrBufferCom[uiIdx]->Render_Buffer();
	}

}

void CDmgFont::Set_ConstantTable()
{

}

HRESULT CDmgFont::Add_Component()
{
	// Buffer
	Engine::CComponent* pComponent = nullptr;
	_tchar szComponentTag[MIN_STR] = L"";

	for (_int i = 0; i < DMG_SIZE; ++i)
	{
		// Buffer
		pComponent = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		pComponent->AddRef();
		wsprintf(szComponentTag, L"Com_Buffer%d", i);
		m_mapComponent[Engine::ID_STATIC].emplace(szComponentTag, pComponent);
		m_arrBufferCom[i] = static_cast<Engine::CRcTex*>(pComponent);
	}

	m_pBackBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pBackBufferCom, E_FAIL);
	m_pBackBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_BackBuffer", m_pBackBufferCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_pShaderCom->Set_PipelineStatePass(2);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

HRESULT CDmgFont::Read_DataFromFilePath()
{
	_tchar	szFilePath[MAX_STR] = L"";
	wstring wstrFilePath        = L"";
	wstring wstrFileType        = L".texuv";

	wstring	wstrTextureTag = L"";
	_int	iTextureIndex  = 0;
	_float	fFrameCnt	   = 0.0f;
	_float	fCurFrame	   = 0.0f;
	_float	fSceneCnt	   = 0.0f;
	_float	fCurScene	   = 0.0f;
	_float	fCanvasWidth   = 0.0f;
	_float	fCanvasHeight  = 0.0f;
	_float	fGridWidth     = 0.0f;
	_float	fGridHeight    = 0.0f;

	for (_uint i = 0; i < DMG_TYPE::DMG_PLAYER; ++i)
		m_vecDmgTextureInfo[i].reserve(10);

	// ComboCounter Yellow
	for (_uint i = 0; i < 10; ++i)
	{
		wstrFilePath = L"../../Bin/ToolData/TexUVComboCounter_Yellow%d";
		wsprintf(szFilePath, wstrFilePath.c_str(), i);

		wifstream fin{ wstring(szFilePath) + wstrFileType };
		if (fin.fail())
			E_FAIL;

		while (true)
		{
			fin >> wstrTextureTag 	// TextureTag
				>> iTextureIndex	// TextureIndex
				>> fFrameCnt		// FrameCnt
				>> fCurFrame		// CurFrame
				>> fSceneCnt		// SceneCnt
				>> fCurScene		// CurScene
				>> fCanvasWidth		// CanvasWidth
				>> fCanvasHeight	// CanvasHeight
				>> fGridWidth		// GridWidth
				>> fGridHeight;		// GridHeight

			if (fin.eof())
				break;

			DMG_TEXTURE_INFO tDmgTextureInfo;
			tDmgTextureInfo.wstrTextureTag     = wstrTextureTag;
			tDmgTextureInfo.pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(wstrTextureTag);
			tDmgTextureInfo.uiTexIdx           = iTextureIndex;
			tDmgTextureInfo.tFrame.fFrameCnt   = fFrameCnt;
			tDmgTextureInfo.tFrame.fCurFrame   = fCurFrame;
			tDmgTextureInfo.tFrame.fSceneCnt   = fSceneCnt;
			tDmgTextureInfo.tFrame.fCurScene   = fCurScene;

			m_vecDmgTextureInfo[DMG_TYPE::DMG_PLAYER].emplace_back(tDmgTextureInfo);
		}
	}

	// ComboCounter Gray
	for (_uint i = 0; i < 10; ++i)
	{
		wstrFilePath = L"../../Bin/ToolData/TexUVComboCounter_Gray%d";
		wsprintf(szFilePath, wstrFilePath.c_str(), i);

		wifstream fin{ wstring(szFilePath) + wstrFileType };
		if (fin.fail())
			E_FAIL;

		while (true)
		{
			fin >> wstrTextureTag 	// TextureTag
				>> iTextureIndex	// TextureIndex
				>> fFrameCnt		// FrameCnt
				>> fCurFrame		// CurFrame
				>> fSceneCnt		// SceneCnt
				>> fCurScene		// CurScene
				>> fCanvasWidth		// CanvasWidth
				>> fCanvasHeight	// CanvasHeight
				>> fGridWidth		// GridWidth
				>> fGridHeight;		// GridHeight

			if (fin.eof())
				break;

			DMG_TEXTURE_INFO tDmgTextureInfo;
			tDmgTextureInfo.wstrTextureTag     = wstrTextureTag;
			tDmgTextureInfo.pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(wstrTextureTag);
			tDmgTextureInfo.uiTexIdx           = iTextureIndex;
			tDmgTextureInfo.tFrame.fFrameCnt   = fFrameCnt;
			tDmgTextureInfo.tFrame.fCurFrame   = fCurFrame;
			tDmgTextureInfo.tFrame.fSceneCnt   = fSceneCnt;
			tDmgTextureInfo.tFrame.fCurScene   = fCurScene;

			m_vecDmgTextureInfo[DMG_TYPE::DMG_OTHERS].emplace_back(tDmgTextureInfo);
		}
	}

	// ComboCounter Blue
	for (_uint i = 0; i < 10; ++i)
	{
		wstrFilePath = L"../../Bin/ToolData/TexUVComboCounter_Blue%d";
		wsprintf(szFilePath, wstrFilePath.c_str(), i);

		wifstream fin{ wstring(szFilePath) + wstrFileType };
		if (fin.fail())
			E_FAIL;

		while (true)
		{
			fin >> wstrTextureTag 	// TextureTag
				>> iTextureIndex	// TextureIndex
				>> fFrameCnt		// FrameCnt
				>> fCurFrame		// CurFrame
				>> fSceneCnt		// SceneCnt
				>> fCurScene		// CurScene
				>> fCanvasWidth		// CanvasWidth
				>> fCanvasHeight	// CanvasHeight
				>> fGridWidth		// GridWidth
				>> fGridHeight;		// GridHeight

			if (fin.eof())
				break;

			DMG_TEXTURE_INFO tDmgTextureInfo;
			tDmgTextureInfo.wstrTextureTag     = wstrTextureTag;
			tDmgTextureInfo.pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(wstrTextureTag);
			tDmgTextureInfo.uiTexIdx           = iTextureIndex;
			tDmgTextureInfo.tFrame.fFrameCnt   = fFrameCnt;
			tDmgTextureInfo.tFrame.fCurFrame   = fCurFrame;
			tDmgTextureInfo.tFrame.fSceneCnt   = fSceneCnt;
			tDmgTextureInfo.tFrame.fCurScene   = fCurScene;

			m_vecDmgTextureInfo[DMG_TYPE::DMG_RECOVERY].emplace_back(tDmgTextureInfo);
		}
	}

	// ComboCounter Red
	for (_uint i = 0; i < 10; ++i)
	{
		wstrFilePath = L"../../Bin/ToolData/TexUVComboCounter_Red%d";
		wsprintf(szFilePath, wstrFilePath.c_str(), i);

		wifstream fin{ wstring(szFilePath) + wstrFileType };
		if (fin.fail())
			E_FAIL;

		while (true)
		{
			fin >> wstrTextureTag 	// TextureTag
				>> iTextureIndex	// TextureIndex
				>> fFrameCnt		// FrameCnt
				>> fCurFrame		// CurFrame
				>> fSceneCnt		// SceneCnt
				>> fCurScene		// CurScene
				>> fCanvasWidth		// CanvasWidth
				>> fCanvasHeight	// CanvasHeight
				>> fGridWidth		// GridWidth
				>> fGridHeight;		// GridHeight

			if (fin.eof())
				break;

			DMG_TEXTURE_INFO tDmgTextureInfo;
			tDmgTextureInfo.wstrTextureTag     = wstrTextureTag;
			tDmgTextureInfo.pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(wstrTextureTag);
			tDmgTextureInfo.uiTexIdx           = iTextureIndex;
			tDmgTextureInfo.tFrame.fFrameCnt   = fFrameCnt;
			tDmgTextureInfo.tFrame.fCurFrame   = fCurFrame;
			tDmgTextureInfo.tFrame.fSceneCnt   = fSceneCnt;
			tDmgTextureInfo.tFrame.fCurScene   = fCurScene;

			m_vecDmgTextureInfo[DMG_TYPE::DMG_MONSTER].emplace_back(tDmgTextureInfo);
		}
	}

	return S_OK;
}

void CDmgFont::SetUp_DmgFontWorldMatrix()
{
	_vec3 vScale = _vec3(1.0f);
	_vec3 vPos   = m_pTransCom->m_vPos;
	vPos.z -= 0.2f;

	_matrix matTrans = INIT_MATRIX;
	_matrix matScale = INIT_MATRIX;

	CDynamicCamera* pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	_vec3 vCameraRight = pDynamicCamera->Get_Transform()->Get_RightVector();
	// vCameraRight.Normalize();

	/*__________________________________________________________________________________________________________
	[ Damage Font ]
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < m_uiDmgListSize; ++i)
	{
		matScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
		matTrans = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

		m_matWorld[i] = matTrans * matScale;

		Make_BillboardMatrix(m_matWorld[i], vScale);

		vPos += vCameraRight * vScale.x;
	}
}

void CDmgFont::Make_BillboardMatrix(_matrix& matrix, const _vec3& vScale)
{
	_matrix* pmatView = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW);
	if (nullptr == pmatView)
		return;

	_matrix matBillboard = (*pmatView);
	memset(&matBillboard._41, 0, sizeof(_vec3));

	matBillboard = MATRIX_INVERSE(matBillboard);

	/*__________________________________________________________________________________________________________
	[ GameObject의 Scale값 반영. ]
	____________________________________________________________________________________________________________*/
	_float fScale[3] = {vScale.x, vScale.y, vScale.z };

	for (_int i = 0; i < 3; ++i)
	{
		for (_int j = 0; j < 4; ++j)
		{
			matBillboard(i, j) *= fScale[i];
		}
	}

	/*__________________________________________________________________________________________________________
	[ Scale + Rotate 입력 ]
	____________________________________________________________________________________________________________*/
	memcpy(&matrix._11, &matBillboard._11, sizeof(_vec3));
	memcpy(&matrix._21, &matBillboard._21, sizeof(_vec3));
	memcpy(&matrix._31, &matBillboard._31, sizeof(_vec3));
}


Engine::CGameObject* CDmgFont::Create(ID3D12Device* pGraphicDevice, 
									  ID3D12GraphicsCommandList* pCommandList, 
									  const _vec3& vPos,
									  const _vec3& vScale,
									  const _uint& uiDmg,
									  const DMG_TYPE& eType)
{
	CDmgFont* pInstance = new CDmgFont(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vPos, vScale, uiDmg, eType)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CDmgFont::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBackBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	
	for (auto& pBufferCom : m_arrBufferCom)
		Engine::Safe_Release(pBufferCom);

	for (auto& vecDmgTextureInfo : m_vecDmgTextureInfo)
	{
		vecDmgTextureInfo.clear();
		vecDmgTextureInfo.shrink_to_fit();
	}

	m_lstDamage.clear();
}
