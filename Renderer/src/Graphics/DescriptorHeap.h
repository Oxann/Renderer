#pragma once

#include <wrl.h>
#include <vector>
#include "d3dx12.h"

class DescriptorHeap;

class Descriptor
{
	friend DescriptorHeap;
public:
	unsigned int GetHeapIndex() const { return heapIndex; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return cpuHandle; }
	bool IsValid() const { return cpuHandle.ptr != 0; }
private:
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
	unsigned int heapIndex = 0;
};

class DescriptorHeap
{
	friend class Graphics;
public:
	enum class Type
	{
		RTV,
		DSV,
		CBV_SRV_UAV,
		Sampler
	};
public:
	DescriptorHeap(const DescriptorHeap&) = delete;
	DescriptorHeap(DescriptorHeap&&) = delete;
public:
	Type GetType() const;
	unsigned int GetDescriptorCount() const;
	unsigned int GetFreeDesciptorCount() const;
	Descriptor GetDescriptor();
	void FreeDescriptor(Descriptor& descriptor);
private:
	DescriptorHeap() = default;
private:
	Type type = Type::CBV_SRV_UAV;
	std::vector<unsigned long long> freeDescriptors;
	Microsoft::WRL::ComPtr<class ID3D12DescriptorHeap> desciptorHeap;
	unsigned int descriptorIncrementSize;
};