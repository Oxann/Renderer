#pragma once

#include <memory>
#include <wrl.h>
#include "d3dx12.h"

#include "Device.h"
#include "SwapChain.h"
#include "CommandQueue.h"
#include "DescriptorHeap.h"
#include "CommandAllocatorPool.h"
#include "CommandList.h"
#include "Fence.h"

class Graphics
{
public:
	static void Init();
	static Device* GetDevice();
	static std::unique_ptr<SwapChain> CreateSwapChain(HWND hWnd, unsigned int bufferWidth, unsigned bufferHeight, int bufferCount);
	static std::unique_ptr<CommandQueue> CreateCommandQueue(CommandQueue::Type type);
	static std::unique_ptr<DescriptorHeap> CreatDescriptorHeap(DescriptorHeap::Type type, unsigned int descriptorCount, bool isShaderVisible);
	static std::unique_ptr<CommandAllocatorPool> CreateCommandAllocatorPool(CommandAllocatorPool::Type type, unsigned int count);
	static std::unique_ptr<CommandList> CreateCommandList(CommandAllocatorPool::Type type);
	static std::unique_ptr<Fence> CreateFence(size_t initialValue);
private:
	inline static std::unique_ptr<Device> device;
};