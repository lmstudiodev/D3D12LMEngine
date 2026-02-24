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

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	if (FAILED(m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue))))
		return false;

	std::cout << "[D3D12] Command Queue created !!!" << std::endl;

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

	vertices[0] = { -1.0f, -1.0f };
	vertices[1] = { 0.0f, 1.0f };
	vertices[2] = { 1.0f, -1.0f };

	D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
	{
		{"position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

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

	void* uploadBufferAddress;
	D3D12_RANGE uploadRange;
	uploadRange.Begin = 0;
	uploadRange.End = 1023;

	m_uploadBuffer->Map(0, &uploadRange, &uploadBufferAddress);
	memcpy(uploadBufferAddress, vertices, sizeof(vertices));
	m_uploadBuffer->Unmap(0, &uploadRange);

	auto* cmdList = InitCommandList();
	cmdList->CopyBufferRegion(m_vertexBuffer, 0, m_uploadBuffer, 0, 1024);

	ExecuteCommandList();

	//Pipeline state
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gfxPsoDesc{};
	gfxPsoDesc.InputLayout.NumElements = _countof(vertexLayout);
	gfxPsoDesc.InputLayout.pInputElementDescs = vertexLayout;
	gfxPsoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	m_vbv.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vbv.SizeInBytes = sizeof(Vertex) * _countof(vertices);
	m_vbv.StrideInBytes = sizeof(Vertex);
}

void DXContext::Draw()
{
	m_cmdList->IASetVertexBuffers(0, 1, &m_vbv);
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_cmdList->DrawInstanced(_countof(vertices), 1, 0, 0);
}

void DXContext::ShutDown()
{
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

ID3D12GraphicsCommandList7* DXContext::InitCommandList()
{
	m_allocator->Reset();
	m_cmdList->Reset(m_allocator, nullptr);
	
	return m_cmdList;
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
