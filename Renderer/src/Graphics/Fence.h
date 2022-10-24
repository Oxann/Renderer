#pragma once

#include <wrl.h>
#include "../Win.h"

class Fence
{
	friend class Graphics;
	friend class CommandQueue;
public:
	size_t GetCurrentValue();
	size_t GetLastSignalValue();
	void Signal(size_t value);
	void WaitForAValue(size_t value);
private:
	Fence() = default;
private:
	Microsoft::WRL::ComPtr<class ID3D12Fence1> fence;
	size_t lastSignalValue = 0ull;
	HANDLE fenceEvent;
};