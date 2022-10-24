#include "Device.h"
#include "../EngineAssert.h"
#include "d3dx12.h"
#include <dxgi1_6.h>

using namespace Microsoft::WRL;
using namespace D3D12MA;

Device::Device(bool enableDebug)
{
    UINT factoryFlags = enableDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
    CHECK_DX_ERROR(CreateDXGIFactory2(factoryFlags, __uuidof(IDXGIFactory2), &factory));

    ComPtr<IDXGIAdapter> tempAdapter;
    CHECK_DX_ERROR(factory->EnumAdapters(0, &tempAdapter));
    tempAdapter.As(&adapter);

    ComPtr<ID3D12Device> tempDevice;
    CHECK_DX_ERROR(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_2, __uuidof(ID3D12Device), &tempDevice));
    CHECK_DX_ERROR(tempDevice.As(&device));

    ALLOCATOR_DESC allocatorDesc = {};
    allocatorDesc.pDevice = tempDevice.Get();
    allocatorDesc.pAdapter = tempAdapter.Get();

    CHECK_DX_ERROR(CreateAllocator(&allocatorDesc, &allocator));
}

Resource* Device::CreateCommittedBuffer(size_t size, D3D12_HEAP_TYPE heapType,D3D12_RESOURCE_STATES state, size_t alignment)
{
    ALLOCATION_DESC allocationDesc = {};
    allocationDesc.HeapType = heapType;
    allocationDesc.Flags = ALLOCATION_FLAG_COMMITTED;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = alignment;
    resourceDesc.Width = size;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ComPtr<Allocation> allocation;
    CHECK_DX_ERROR(allocator->CreateResource(&allocationDesc, &resourceDesc, state, nullptr, &allocation, IID_NULL, nullptr));

    Resource* newBuffer = new Resource;
    newBuffer->allocation = allocation;

    return newBuffer;
}

Resource* Device::CreateComittedTexture2D(size_t width, 
    size_t height, 
    D3D12_RESOURCE_STATES state, 
    DXGI_FORMAT format, 
    D3D12_RESOURCE_FLAGS flags,
    D3D12_CLEAR_VALUE* clearValue,
    unsigned short mipCount, 
    unsigned short arraySize, 
    size_t alignment)
{
    ALLOCATION_DESC allocationDesc = {};
    allocationDesc.HeapType = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
    allocationDesc.Flags = ALLOCATION_FLAG_COMMITTED;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Alignment = alignment;
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.DepthOrArraySize = arraySize;
    resourceDesc.MipLevels = mipCount;
    resourceDesc.Format = format;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags = flags;

    ComPtr<Allocation> allocation;
    CHECK_DX_ERROR(allocator->CreateResource(&allocationDesc, &resourceDesc, state, clearValue, &allocation, IID_NULL, nullptr));

    Resource* newBuffer = new Resource;
    newBuffer->allocation = allocation;

    return newBuffer;
}

void Device::CreateRenderTargetView(const Resource* resource, const Descriptor& descriptor, DXGI_FORMAT format, unsigned int mipSlice, unsigned int planeSlice)
{
    ENGINEASSERT(descriptor.IsValid(), "Descriptor is not valid");

    D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
    viewDesc.Format = format;
    viewDesc.ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D;
    viewDesc.Texture2D.MipSlice = mipSlice;
    viewDesc.Texture2D.PlaneSlice = planeSlice;

    device->CreateRenderTargetView(resource->allocation->GetResource(), &viewDesc, descriptor.GetCPUHandle());
}

void Device::CreateDepthStencilView(const Resource* resource, const Descriptor& descriptor, DXGI_FORMAT format, D3D12_DSV_FLAGS flags, unsigned int mipSlice)
{
    ENGINEASSERT(descriptor.IsValid(), "Descriptor is not valid");

    D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc;
    viewDesc.Format = format;
    viewDesc.Flags = flags;
    viewDesc.ViewDimension = D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;
    viewDesc.Texture2D.MipSlice = mipSlice;

    device->CreateDepthStencilView(resource->allocation->GetResource(), &viewDesc, descriptor.GetCPUHandle());
}

void Device::CreateConstantBufferView(const Resource* resource, const Descriptor& descriptor, size_t size, size_t offset)
{
    ENGINEASSERT(descriptor.IsValid(), "Descriptor is not valid");

    D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
    desc.SizeInBytes = size;
    desc.BufferLocation = resource->allocation->GetResource()->GetGPUVirtualAddress() + offset;

    device->CreateConstantBufferView(&desc, descriptor.GetCPUHandle());
}
