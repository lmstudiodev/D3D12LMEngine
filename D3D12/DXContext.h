#pragma once
#include <support/WinInclude.h>
#include <support/ComPointer.h>
#include <support/Shader.h>
#include <support/Mesh.h>

class  DXContext
{
public:
	DXContext(const DXContext&) = delete;
	DXContext& operator=(const DXContext&) = delete;

	inline static DXContext& Get()
	{
		static DXContext instance;

		return instance;
	}

private:
	DXContext() = default;

private:
	bool CreateCommandQueue();
	void CheckRaytracingSupport();
	void CreateCommittedResources();
	void SignalAndWait();
	void CreateBuffers(const void* source, size_t size);
	bool CreateRootSignature();
	bool CreatePipeline();
	void SetVertexBufferView();
	void LoadShader();
	void LoadMesh();

public:
	bool Init();
	void ShutDown();
	void Draw();
	void ExecuteCommandList();
	ID3D12GraphicsCommandList7* InitCommandList();

	inline void Flush(size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			SignalAndWait();
		}
	}

	inline ComPointer<IDXGIFactory7>& GetDXGIFactory() { return m_dxgiFactory; }
	inline ComPointer<ID3D12Device10>& GetDevice() { return m_device; }
	inline ComPointer<ID3D12CommandQueue>& GetCommandQueue() { return m_cmdQueue; }

private:
	ComPointer<IDXGIFactory7> m_dxgiFactory;

	ComPointer<ID3D12Device10> m_device;
	ComPointer<ID3D12CommandQueue> m_cmdQueue;
	ComPointer<ID3D12Fence1> m_fence;

	ComPointer<ID3D12CommandAllocator> m_allocator;
	ComPointer<ID3D12GraphicsCommandList7> m_cmdList;

	ComPointer<ID3D12Resource2> m_uploadBuffer;
	ComPointer<ID3D12Resource2> m_vertexBuffer;

	ComPointer<ID3D12RootSignature> m_rootSignature;

	ComPointer<ID3D12PipelineState> m_pso;

	UINT64 m_fenceValue = 0;
	HANDLE m_fenceEvent = nullptr;

	D3D12_VERTEX_BUFFER_VIEW m_vbv{};

	Vertex vertices[3]{};

	Shader m_rootSignatureShader;
	Shader m_vertexShader;
	Shader m_pixelShader;
};
