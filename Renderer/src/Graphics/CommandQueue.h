#pragma once

#include <wrl.h>
#include "Fence.h"

class CommandQueue
{
	friend class Graphics;
public:
	enum class Type
	{
		DIRECT,
		COMPUTE,
		COPY
	};
private:
	CommandQueue() = default;
public:
	Type GetType() const;
	void SignalFence(Fence* fence);
	void ExecuteCommandLists(unsigned int count, class CommandList* const*);
private:
	Type type;
	Microsoft::WRL::ComPtr<class ID3D12CommandQueue> commandQueue;
};