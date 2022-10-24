#include "Resource.h"
#include "../EngineAssert.h"

size_t Resource::GetSize() const
{
	return allocation->GetSize();
}

size_t Resource::GetGPUAddress()
{
	return allocation->GetResource()->GetGPUVirtualAddress();
}

void Resource::Map(unsigned int subresource, void** data, size_t readBegin, size_t readEnd)
{
	D3D12_RANGE range;
	range.Begin = readBegin;
	range.End = readEnd;

	CHECK_DX_ERROR(allocation->GetResource()->Map(subresource, &range, data));
}

void Resource::Unmap(unsigned int subresource, size_t writeBegin, size_t writeEnd)
{
	D3D12_RANGE range;
	range.Begin = writeBegin;
	range.End = writeEnd;

	allocation->GetResource()->Unmap(subresource, &range);
}
