#pragma once

#include <wrl.h>
#include "CommandAllocatorPool.h"
#include "DescriptorHeap.h"

class CommandList
{
	friend class Graphics;
	friend class CommandQueue;
public:
	CommandAllocatorPool::Type GetType() const;
	void Reset(CommandAllocatorPool* allocatorPool, unsigned int index);
	void Close();
	void ClearDSV(const Descriptor& descriptor, D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH, float depth = 1.0f, unsigned char stencil = 0u);
	void ClearRTV(const Descriptor& descriptor, float clearColor[4]);
	void Barrier(const D3D12_RESOURCE_BARRIER* barriers, unsigned int barrierCount);
private:
	CommandList() = default;
private:
	Microsoft::WRL::ComPtr<class ID3D12GraphicsCommandList5> commandList;
	Microsoft::WRL::ComPtr<class ID3D12CommandList> _commandList;
	CommandAllocatorPool::Type type;
};