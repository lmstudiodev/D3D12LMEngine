#include "DXContext.h"
#include <cstdlib>

bool DXContext::Init()
{	
	if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory))))
		return false;
	
		std::cout << "[D3D12] DXGIFacotiory created !!!" << std::endl;
	
	if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device))))
		return false;

	std::cout << "[D3D12] Device created !!!" << std::endl;

	if (!CreateCommandQueue())
		return false;

	if (FAILED(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
		return false;

	std::cout << "[D3D12] Fence created !!!" << std::endl;

	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);

	if (!m_fenceEvent)
		return false;

	std::cout << "[D3D12] Fence Event created !!!" << std::endl;

	if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_allocator))))
		return false;

	std::cout << "[D3D12] Command Allocator created !!!" << std::endl;

	if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_cmdList))))
		return false;

	std::cout << "[D3D12] Graphic Command List created !!!" << std::endl;

	CheckRaytracingSupport();

	CreateCommittedResources();

	LoadMesh();

	CreateBuffers(vertices, sizeof(vertices));

	LoadShader();

	if (!CreateRootSignature())
		return false;

	if (!CreatePipeline())
		return false;

	SetVertexBufferView();

	return true;
}

void DXContext::Draw(const float width, const float height)
{
	m_cmdList->SetPipelineState(m_pso);
	m_cmdList->SetGraphicsRootSignature(m_rootSignature);

	m_cmdList->IASetVertexBuffers(0, 1, &m_vbv);
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SetViewPort(width, height);

	m_cmdList->DrawInstanced(_countof(vertices), 1, 0, 0);
}

void DXContext::ShutDown()
{
	m_pso.Release();

	m_rootSignature.Release();

	m_vertexBuffer.Release();

	m_uploadBuffer.Release();

	m_cmdList.Release();

	m_allocator.Release();

	if (m_fenceEvent)
	{
		CloseHandle(m_fenceEvent);
	}

	m_fence.Release();

	m_cmdQueue.Release();

	m_device.Release();

	m_dxgiFactory.Release();
}

ID3D12GraphicsCommandList7* DXContext::InitCommandList()
{
	m_allocator->Reset();
	m_cmdList->Reset(m_allocator, nullptr);

	return m_cmdList;
}

void DXContext::SetViewPort(const float width, const float height)
{
	D3D12_VIEWPORT vp{};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 1.0f;
	vp.MaxDepth = 0.0f;

	m_cmdList->RSSetViewports(1, &vp);

	RECT scRect;
	scRect.left = 0;
	scRect.top = 0;
	scRect.right = width;
	scRect.bottom = height;

	m_cmdList->RSSetScissorRects(1, &scRect);
}

void DXContext::ExecuteCommandList()
{
	if (SUCCEEDED(m_cmdList->Close()))
	{
		ID3D12CommandList* lists[] = { m_cmdList };

		m_cmdQueue->ExecuteCommandLists(1, lists);

		SignalAndWait();
	}
}

bool DXContext::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	if (FAILED(m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue))))
		return false;

	std::cout << "[D3D12] Command Queue created !!!" << std::endl;

	return true;
}

void DXContext::CheckRaytracingSupport()
{
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};

	if (SUCCEEDED(m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5))))
	{
		if (options5.RaytracingTier < D3D12_RAYTRACING_TIER_1_0)
		{
			std::cout << "[D3D12] Ray Tracing not supported !!!" << std::endl;
		}
	}

	std::cout << "[D3D12] Ray Tracing supported  !!!" << std::endl;
}

void DXContext::CreateCommittedResources()
{
	D3D12_HEAP_PROPERTIES hpUpload{};
	hpUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
	hpUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hpUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hpUpload.CreationNodeMask = 0;
	hpUpload.VisibleNodeMask = 0;

	D3D12_HEAP_PROPERTIES hpDefault{};
	hpDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
	hpDefault.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hpDefault.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hpDefault.CreationNodeMask = 0;
	hpDefault.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	resDesc.Width = 1024;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	m_device->CreateCommittedResource(&hpUpload, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_uploadBuffer));
	m_device->CreateCommittedResource(&hpDefault, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_vertexBuffer));
}

void DXContext::CreateBuffers(const void* source, size_t size)
{
	void* uploadBufferAddress;
	D3D12_RANGE uploadRange;
	uploadRange.Begin = 0;
	uploadRange.End = 1023;

	m_uploadBuffer->Map(0, &uploadRange, &uploadBufferAddress);
	memcpy(uploadBufferAddress, source, size);
	m_uploadBuffer->Unmap(0, &uploadRange);

	auto* cmdList = InitCommandList();
	cmdList->CopyBufferRegion(m_vertexBuffer, 0, m_uploadBuffer, 0, 1024);

	ExecuteCommandList();
}

bool DXContext::CreateRootSignature()
{
	if (FAILED(m_device->CreateRootSignature(0, m_rootSignatureShader.GetBuffer(), m_rootSignatureShader.GetSize(), IID_PPV_ARGS(&m_rootSignature))))
	{
		std::cout << "[D3D12] Root Signature creation failed !!!" << std::endl;
		return false;
	}

	std::cout << "[D3D12] Root Signature created !!!" << std::endl;
	
	return true;
}

bool DXContext::CreatePipeline()
{
	D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gfxPsoDesc{};
	gfxPsoDesc.pRootSignature = m_rootSignature;
	gfxPsoDesc.InputLayout.NumElements = _countof(vertexLayout);
	gfxPsoDesc.InputLayout.pInputElementDescs = vertexLayout;
	gfxPsoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gfxPsoDesc.VS.BytecodeLength = m_vertexShader.GetSize();
	gfxPsoDesc.VS.pShaderBytecode = m_vertexShader.GetBuffer();
	gfxPsoDesc.PS.BytecodeLength = m_pixelShader.GetSize();
	gfxPsoDesc.PS.pShaderBytecode = m_pixelShader.GetBuffer();
	gfxPsoDesc.GS.BytecodeLength = 0;
	gfxPsoDesc.GS.pShaderBytecode = 0;
	gfxPsoDesc.HS.BytecodeLength = 0;
	gfxPsoDesc.HS.pShaderBytecode = 0;
	gfxPsoDesc.DS.BytecodeLength = 0;
	gfxPsoDesc.DS.pShaderBytecode = 0;
	gfxPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gfxPsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gfxPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gfxPsoDesc.RasterizerState.FrontCounterClockwise = FALSE;
	gfxPsoDesc.RasterizerState.DepthBias = 0;
	gfxPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	gfxPsoDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
	gfxPsoDesc.RasterizerState.DepthClipEnable = FALSE;
	gfxPsoDesc.RasterizerState.MultisampleEnable = FALSE;
	gfxPsoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	gfxPsoDesc.RasterizerState.ForcedSampleCount = 0;

	gfxPsoDesc.StreamOutput.NumEntries = 0;
	gfxPsoDesc.StreamOutput.NumStrides = 0;
	gfxPsoDesc.StreamOutput.pBufferStrides = nullptr;
	gfxPsoDesc.StreamOutput.pSODeclaration = nullptr;
	gfxPsoDesc.StreamOutput.RasterizedStream = 0;

	gfxPsoDesc.NumRenderTargets = 1;
	gfxPsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gfxPsoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

	gfxPsoDesc.BlendState.AlphaToCoverageEnable = FALSE;
	gfxPsoDesc.BlendState.IndependentBlendEnable = FALSE;
	gfxPsoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
	gfxPsoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
	gfxPsoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	gfxPsoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	gfxPsoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	gfxPsoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
	gfxPsoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	gfxPsoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	gfxPsoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	gfxPsoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;;

	gfxPsoDesc.DepthStencilState.DepthEnable = FALSE;
	gfxPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gfxPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	gfxPsoDesc.DepthStencilState.StencilEnable = FALSE;
	gfxPsoDesc.DepthStencilState.StencilReadMask = 0;
	gfxPsoDesc.DepthStencilState.StencilWriteMask = 0;
	gfxPsoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gfxPsoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	gfxPsoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	gfxPsoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	gfxPsoDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gfxPsoDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	gfxPsoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	gfxPsoDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;

	gfxPsoDesc.SampleMask = 0xFFFFFFFF;
	gfxPsoDesc.SampleDesc.Count = 1;
	gfxPsoDesc.SampleDesc.Quality = 0;

	gfxPsoDesc.NodeMask = 0;
	gfxPsoDesc.CachedPSO.CachedBlobSizeInBytes = 0;
	gfxPsoDesc.CachedPSO.pCachedBlob = nullptr;

	gfxPsoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	if (FAILED(m_device->CreateGraphicsPipelineState(&gfxPsoDesc, IID_PPV_ARGS(&m_pso))))
	{
		std::cout << "[D3D12] Pipeline object creation failed !!!" << std::endl;
		return false;
	}

	std::cout << "[D3D12] Pipeline object created!!!" << std::endl;

	return true;
}

void DXContext::SetVertexBufferView()
{
	m_vbv.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vbv.SizeInBytes = sizeof(Vertex) * _countof(vertices);
	m_vbv.StrideInBytes = sizeof(Vertex);
}

void DXContext::LoadShader()
{
	m_rootSignatureShader.LoadFromFile("RootSignature.cso");
	m_vertexShader.LoadFromFile("VertexShader.cso");
	m_pixelShader.LoadFromFile("PixelShader.cso");
}

void DXContext::LoadMesh()
{
	vertices[0] = { { -0.5f, -0.5f, 1.0f }, { 1.0f, 0.5f, 0.5f }, { 0.0f, 0.0f } };
	vertices[1] = { { -0.5f, 0.5f, 1.0f },  { 0.5f, 1.0f, 0.5f }, { 0.0f, 0.0f } };
	vertices[2] = { { 0.5f, -0.5f, 1.0f },  { 0.5f, 0.5f, 1.0f }, { 0.0f, 0.0f } };
	vertices[3] = { { -0.5f, 0.5f, 1.0f },  { 0.5f, 0.5f, 1.0f }, { 0.0f, 0.0f } };
	vertices[4] = { { 0.5f, 0.5f, 1.0f },   { 0.5f, 1.0f, 0.5f }, { 0.0f, 0.0f } };
	vertices[5] = { { 0.5f, -0.5f, 1.0f },  { 1.0f, 0.5f, 0.5f }, { 0.0f, 0.0f } };
}

void DXContext::SignalAndWait()
{
	m_cmdQueue->Signal(m_fence, ++m_fenceValue);

	if (SUCCEEDED(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent)))
	{
		if (WaitForSingleObject(m_fenceEvent, 20000) != WAIT_OBJECT_0)
		{
			std::exit(-1);
		}
	}
	else
	{
		std::exit(-1);
	}
}
