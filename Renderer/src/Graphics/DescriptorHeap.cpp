#include "DescriptorHeap.h"
#include "../EngineAssert.h"
#include "d3dx12.h"

DescriptorHeap::Type DescriptorHeap::GetType() const
{
    return type;
}

unsigned int DescriptorHeap::GetDescriptorCount() const
{
    return freeDescriptors.capacity();
}

unsigned int DescriptorHeap::GetFreeDesciptorCount() const
{
    return freeDescriptors.size();
}

Descriptor DescriptorHeap::GetDescriptor()
{
    ENGINEASSERT(freeDescriptors.size() > 0, "There is no free descriptor in the heap.");
    Descriptor newDescriptor;
    newDescriptor.heapIndex = freeDescriptors.back();
    newDescriptor.cpuHandle.ptr = desciptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + static_cast<size_t>(newDescriptor.heapIndex) * descriptorIncrementSize;
    freeDescriptors.pop_back();
    return newDescriptor;
}

void DescriptorHeap::FreeDescriptor(Descriptor& descriptor)
{
    ENGINEASSERT(descriptor.IsValid(), "Descriptor is not valid");
    freeDescriptors.push_back(descriptor.heapIndex);
    descriptor.cpuHandle.ptr = 0;
}
