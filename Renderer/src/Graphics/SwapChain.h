#pragma once

#include <wrl.h>
#include <memory>
#include "DescriptorHeap.h"
#include "Device.h"

class CommandQueue;

class SwapChain
{
	friend class Graphics;
public:
	CommandQueue* GetCommandQueue();
	Microsoft::WRL::ComPtr<ID3D12Resource> GetBuffer(unsigned int index);
	const Descriptor& GetBackBufferDescriptor(unsigned int index);
	const Descriptor& GetDepthBufferDescriptor();
	unsigned int GetCurrentBackBufferIndex();
	void Present();
private:
	SwapChain() = default;
private:
	std::unique_ptr<CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<class IDXGISwapChain3> swapChain;
	
	std::unique_ptr<DescriptorHeap> backBufferDescriptorHeap;
	Descriptor backBufferDescriptors[3] = {};
	
	std::unique_ptr<Resource> depthBuffer;
	std::unique_ptr<DescriptorHeap> depthBufferDescriptorHeap;
	Descriptor depthBufferDescriptor = {};
};