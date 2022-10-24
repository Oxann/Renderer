#pragma once

#include <wrl.h>
#include "../thirdparty/D3D12MemAlloc.h"

class Resource
{
	friend class Device;
private:
	Resource() = default;
public:
	Resource(const Resource&) = delete;
	Resource(Resource&&) = delete;

	size_t GetSize() const;
	size_t GetGPUAddress();
	void Map(unsigned int subresource, void** data, size_t readBegin = 0, size_t readEnd = 0);
	void Unmap(unsigned int subresource, size_t writeBegin = 0, size_t writeEnd = 0);
private:
	Microsoft::WRL::ComPtr<D3D12MA::Allocation> allocation;
};