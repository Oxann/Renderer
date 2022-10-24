#include "Graphics.h"
#include "../EngineAssert.h"
#include <dxgi1_6.h>
#include <numeric>

using namespace Microsoft::WRL;

void Graphics::Init()
{
#ifndef NDEBUG
	ComPtr<ID3D12Debug1> debug;
	CHECK_DX_ERROR(D3D12GetDebugInterface(__uuidof(ID3D12Debug1), &debug));
	debug->SetEnableGPUBasedValidation(TRUE);
	debug->EnableDebugLayer();
	
	device.reset(new Device(true));

	ComPtr<ID3D12InfoQueue> infoQueue;
	CHECK_DX_ERROR(device->device.As(&infoQueue));

	if (infoQueue)
	{
		CHECK_DX_ERROR(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_ERROR, TRUE));
		CHECK_DX_ERROR(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE));
		CHECK_DX_ERROR(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_WARNING, TRUE));
	}
#else
	device.reset(new Device(false));
#endif
}

Device* Graphics::GetDevice()
{
	return device.get();
}

std::unique_ptr<SwapChain> Graphics::CreateSwapChain(HWND hWnd, unsigned int bufferWidth, unsigned int bufferHeight, int bufferCount)
{
	ENGINEASSERT(bufferCount > 1, "Swap chain buffer count must be greater than 1.")

	std::unique_ptr<SwapChain> swapChain(new SwapChain);
	swapChain->commandQueue = CreateCommandQueue(CommandQueue::Type::DIRECT);
	swapChain->backBufferDescriptorHeap = Graphics::CreatDescriptorHeap(DescriptorHeap::Type::RTV, bufferCount, false);
	swapChain->depthBufferDescriptorHeap = Graphics::CreatDescriptorHeap(DescriptorHeap::Type::DSV, 1, false);
	
	D3D12_CLEAR_VALUE depthClearValue;
	depthClearValue.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.DepthStencil.Stencil = 0;

	swapChain->depthBuffer.reset(device->CreateComittedTexture2D(bufferWidth,
		bufferHeight,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
		DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT,
		D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		&depthClearValue
	));
	swapChain->depthBufferDescriptor = swapChain->depthBufferDescriptorHeap->GetDescriptor();

	device->CreateDepthStencilView(swapChain->depthBuffer.get(), swapChain->depthBufferDescriptor);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	swapChainDesc.Width = bufferWidth;
	swapChainDesc.Height = bufferHeight;
	swapChainDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = 0;

	ComPtr<IDXGISwapChain1> tempSwapChain;
	CHECK_DX_ERROR(device->factory->CreateSwapChainForHwnd(swapChain->commandQueue->commandQueue.Get(), hWnd, &swapChainDesc, NULL, NULL, &tempSwapChain));
	CHECK_DX_ERROR(tempSwapChain.As(&swapChain->swapChain));

	for (int i = 0; i < bufferCount; i++)
	{
		ComPtr<ID3D12Resource> backBuffer;
		CHECK_DX_ERROR(swapChain->swapChain->GetBuffer(i, __uuidof(ID3D12Resource), &backBuffer));
		swapChain->backBufferDescriptors[i] = swapChain->backBufferDescriptorHeap->GetDescriptor();

		D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
		viewDesc.Format = swapChainDesc.Format;
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;
		viewDesc.Texture2D.PlaneSlice = 0;
	
		device->device->CreateRenderTargetView(backBuffer.Get(), &viewDesc, swapChain->backBufferDescriptors[i].GetCPUHandle());
	}

	return swapChain;
}

std::unique_ptr<CommandQueue> Graphics::CreateCommandQueue(CommandQueue::Type type)
{
	D3D12_COMMAND_QUEUE_DESC desc;
	desc.NodeMask = 0;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;

	switch (type)
	{
	case CommandQueue::Type::DIRECT:
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	case CommandQueue::Type::COMPUTE:
		desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	case CommandQueue::Type::COPY:
		desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		break;
	default:
		break;
	}

	std::unique_ptr<CommandQueue> commandQueue(new CommandQueue);
	commandQueue->type = type;
	CHECK_DX_ERROR(device->device->CreateCommandQueue(&desc, __uuidof(ID3D12CommandQueue), &commandQueue->commandQueue));

	return commandQueue;
}

std::unique_ptr<DescriptorHeap> Graphics::CreatDescriptorHeap(DescriptorHeap::Type type, unsigned int descriptorCount, bool isShaderVisible)
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.NumDescriptors = descriptorCount;
	descriptorHeapDesc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;

	std::unique_ptr<DescriptorHeap> desciptorHeap(new DescriptorHeap);
	desciptorHeap->freeDescriptors.resize(descriptorCount);
	desciptorHeap->type = type;
	std::iota(desciptorHeap->freeDescriptors.begin(), desciptorHeap->freeDescriptors.end(), 0);

	switch (type)
	{
	case DescriptorHeap::Type::RTV:
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desciptorHeap->descriptorIncrementSize = device->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		break;
	case DescriptorHeap::Type::DSV:
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desciptorHeap->descriptorIncrementSize = device->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		break;
	case DescriptorHeap::Type::CBV_SRV_UAV:
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desciptorHeap->descriptorIncrementSize = device->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		break;
	case DescriptorHeap::Type::Sampler:
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desciptorHeap->descriptorIncrementSize = device->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		break;
	default:
		break;
	}

	CHECK_DX_ERROR(device->device->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), &desciptorHeap->desciptorHeap));

	return desciptorHeap;
}

std::unique_ptr<CommandAllocatorPool> Graphics::CreateCommandAllocatorPool(CommandAllocatorPool::Type type, unsigned int count)
{
	D3D12_COMMAND_LIST_TYPE listType;

	switch (type)
	{
	case CommandAllocatorPool::Type::DIRECT:
		listType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	case CommandAllocatorPool::Type::COMPUTE:
		listType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	case CommandAllocatorPool::Type::COPY:
		listType = D3D12_COMMAND_LIST_TYPE_COPY;
		break;
	case CommandAllocatorPool::Type::BUNDLE:
		listType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
		break;
	default:
		break;
	}

	std::unique_ptr<CommandAllocatorPool> newPool;
	newPool.reset(new CommandAllocatorPool);
	newPool->type = type;
	newPool->allocators.resize(count);

	for (int i = 0; i < count; i++)
	{
		CHECK_DX_ERROR(device->device->CreateCommandAllocator(listType, __uuidof(ID3D12CommandAllocator), &(newPool->allocators[i])));
	}

	return newPool;
}

std::unique_ptr<CommandList> Graphics::CreateCommandList(CommandAllocatorPool::Type type)
{
	D3D12_COMMAND_LIST_TYPE listType;

	switch (type)
	{
	case CommandAllocatorPool::Type::DIRECT:
		listType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	case CommandAllocatorPool::Type::COMPUTE:
		listType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	case CommandAllocatorPool::Type::COPY:
		listType = D3D12_COMMAND_LIST_TYPE_COPY;
		break;
	case CommandAllocatorPool::Type::BUNDLE:
		listType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
		break;
	default:
		break;
	}

	ComPtr<ID3D12GraphicsCommandList> tempList;
	CHECK_DX_ERROR(device->device->CreateCommandList1(0, listType, D3D12_COMMAND_LIST_FLAGS::D3D12_COMMAND_LIST_FLAG_NONE, __uuidof(ID3D12GraphicsCommandList), &tempList));

	std::unique_ptr<CommandList> commandList;
	commandList.reset(new CommandList);
	commandList->type = type;
	CHECK_DX_ERROR(tempList.As(&(commandList->commandList)));
	CHECK_DX_ERROR(tempList.As(&(commandList->_commandList)));

	return commandList;
}

std::unique_ptr<Fence> Graphics::CreateFence(size_t initialValue)
{
	std::unique_ptr<Fence> fence;
	fence.reset(new Fence);
	fence->fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);;

	ComPtr<ID3D12Fence> tempFence;
	CHECK_DX_ERROR(device->device->CreateFence(initialValue, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), &tempFence));

	CHECK_DX_ERROR(tempFence.As(&fence->fence));

	return fence;
}
