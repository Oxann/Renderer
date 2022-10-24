#include "ConstantBufferPool.h"
#include "../Graphics/Graphics.h"
#include "../EngineAssert.h"


ConstantBufferPool::ConstantBufferPool(size_t bufferCount, size_t bufferSize)
{
	this->bufferCount = bufferCount;
	this->bufferSize = bufferSize;

	ENGINEASSERT(bufferSize % 256 == 0, "Buffer size must be a multiple of 256");

	auto _buffer = Graphics::GetDevice()->CreateCommittedBuffer(bufferCount * bufferSize,
		D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ);

	buffer.reset(_buffer);
}

size_t ConstantBufferPool::GetAddressByIndex(size_t index)
{
	return buffer->GetGPUAddress() + bufferSize * index;
}

size_t ConstantBufferPool::GetBufferCount()
{
	return bufferCount;
}

size_t ConstantBufferPool::GetBufferSize()
{
	return bufferSize;
}
