#include "CommandAllocatorPool.h"
#include "../EngineAssert.h"
#include "d3dx12.h"

CommandAllocatorPool::Type CommandAllocatorPool::GetType() const
{
    return type;
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocatorPool::GetAllocator(unsigned int index)
{
    return allocators[index];
}
