#pragma once
#include <support/WinInclude.h>
#include <support/ComPointer.h>
#include <support/ImageLoader.h>
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
	void CreateCommittedResources(const ImageLoader::ImageData& textureData, uint32_t textureSize);
	void SignalAndWait();
	void CreateBuffers(const void* source, size_t size, const ImageLoader::ImageData& texture, uint32_t textureSize);
	bool CreateRootSignature();
	bool CreatePipeline();
	void SetVertexBufferView();
	void SetViewPort(const float width, const float height);
	void LoadShader();
	void LoadMesh();
	void PukeColor(float* color);
	UINT CalculateConstantBufferAlignement(const UINT allocation);
	void Draw(const float width, const float height);

public:
	bool Init();
	bool CreateResources(const ImageLoader::ImageData& textureData);
	void ShutDown();
	void Update(const float width, const float height);
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
	ComPointer<ID3D12Resource2> m_texture;

	ComPointer<ID3D12RootSignature> m_rootSignature;

	ComPointer<ID3D12PipelineState> m_pso;

	ComPointer<ID3D12DescriptorHeap> m_textureDescriptorHeap;

	UINT64 m_fenceValue = 0;
	HANDLE m_fenceEvent = nullptr;

	D3D12_VERTEX_BUFFER_VIEW m_vbv{};

	Vertex vertices[18]{};

	Shader m_rootSignatureShader;
	Shader m_vertexShader;
	Shader m_pixelShader;

	float m_FoV;

	DirectX::XMMATRIX m_ModelMatrix;
	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_ProjectionMatrix;
};
