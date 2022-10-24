#pragma once

#include <vector>
#include <wrl.h>
#include <mutex>

class ID3D12CommandAllocator;

class CommandAllocatorPool
{
	friend class Graphics;
	friend class CommandList;
public:
	enum class Type
	{
		DIRECT,
		BUNDLE,
		COMPUTE,
		COPY
	};
public:
	Type GetType() const;
private:
	CommandAllocatorPool() = default;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetAllocator(unsigned int index);
private:
	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> allocators;
	Type type;
};