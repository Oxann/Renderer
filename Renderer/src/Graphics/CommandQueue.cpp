#include "CommandQueue.h"
#include "d3dx12.h"
#include "../EngineAssert.h"
#include "CommandList.h"

CommandQueue::Type CommandQueue::GetType() const
{
    return type;
}

void CommandQueue::SignalFence(Fence* fence)
{
    fence->lastSignalValue++;
    CHECK_DX_ERROR(commandQueue->Signal(fence->fence.Get(), fence->lastSignalValue));
}

void CommandQueue::ExecuteCommandLists(unsigned int count, class CommandList* const* lists)
{
    static ID3D12CommandList* _lists[100] = { nullptr };

    for (int i = 0; i < count; i++)
        _lists[i] = lists[i]->_commandList.Get();

    commandQueue->ExecuteCommandLists(count, _lists);
}
