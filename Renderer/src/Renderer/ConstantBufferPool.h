#pragma once


#include <memory>
#include "../Graphics/DescriptorHeap.h"

class ConstantBufferPool
{
public:
	ConstantBufferPool(size_t bufferCount, size_t bufferSize = 256);
	size_t GetAddressByIndex(size_t index);
	size_t GetBufferCount();
	size_t GetBufferSize();
private:
	std::unique_ptr<class Resource> buffer;
	size_t bufferCount;
	size_t bufferSize;
};