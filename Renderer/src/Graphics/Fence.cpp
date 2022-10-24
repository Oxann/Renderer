#include "Fence.h"
#include "d3dx12.h"
#include "../EngineAssert.h"

size_t Fence::GetCurrentValue()
{
	return fence->GetCompletedValue();
}

size_t Fence::GetLastSignalValue()
{
    return lastSignalValue;
}

void Fence::Signal(size_t value)
{
	CHECK_DX_ERROR(fence->Signal(value));
}

void Fence::WaitForAValue(size_t value)
{
    if (GetCurrentValue() < value)
    {
        fence->SetEventOnCompletion(value, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}
