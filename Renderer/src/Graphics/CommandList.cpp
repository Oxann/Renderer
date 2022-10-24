#include "CommandList.h"
#include "d3dx12.h"
#include "../EngineAssert.h"

CommandAllocatorPool::Type CommandList::GetType() const
{
	return type;
}

void CommandList::Reset(CommandAllocatorPool* allocatorPool, unsigned int index)
{
	auto allocator = allocatorPool->allocators[index];
	allocator->Reset();

	CHECK_DX_ERROR(commandList->Reset(allocator.Get(), nullptr));
}

void CommandList::Close()
{
	CHECK_DX_ERROR(commandList->Close());
}

void CommandList::ClearDSV(const Descriptor& descriptor, D3D12_CLEAR_FLAGS clearFlags, float depth, unsigned char stencil)
{
	commandList->ClearDepthStencilView(descriptor.GetCPUHandle(), clearFlags, depth, stencil, 0, nullptr);
}

void CommandList::ClearRTV(const Descriptor& descriptor, float clearColor[4])
{
	commandList->ClearRenderTargetView(descriptor.GetCPUHandle(), clearColor, 0, nullptr);
}

void CommandList::Barrier(const D3D12_RESOURCE_BARRIER* barriers, unsigned int barrierCount)
{
	commandList->ResourceBarrier(barrierCount, barriers);
}
