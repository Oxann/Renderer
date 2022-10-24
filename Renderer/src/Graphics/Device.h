#pragma once

#include <wrl.h>
#include "../thirdparty/D3D12MemAlloc.h"
#include "Resource.h"
#include "DescriptorHeap.h"

class Device
{
	friend class Graphics;
public:
	Resource* CreateCommittedBuffer(size_t size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES state, size_t alignment = 0ull);

	Resource* CreateComittedTexture2D(size_t width,
		size_t height,
		D3D12_RESOURCE_STATES state,
		DXGI_FORMAT format,
		D3D12_RESOURCE_FLAGS flags,
		D3D12_CLEAR_VALUE* clearValue = nullptr,
		unsigned short mipCount = 1u,
		unsigned short arraySize = 1u,
		size_t alignment = 0ull);

	void CreateRenderTargetView(const Resource* resource, const Descriptor& descriptor, DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, unsigned int mipSlice = 0u, unsigned int planeSlice = 0u);
	void CreateDepthStencilView(const Resource* resource, const Descriptor& descriptor, DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, D3D12_DSV_FLAGS flags = D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE, unsigned int mipSlice = 0u);
	void CreateConstantBufferView(const Resource* resource, const Descriptor& descriptor, size_t size = 0, size_t offset = 0);

private:
	Device(bool enableDebug);
private:
	Microsoft::WRL::ComPtr<IDXGIFactory2> factory;
	Microsoft::WRL::ComPtr<ID3D12Device9> device;
	Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter;
	D3D12MA::Allocator* allocator;
};