#include "RendererManager.h"
#include "ConstantBufferPool.h"
#include <vector>

std::vector<ConstantBufferPool> perObjectFrameBuffers;

RendererManager::RendererManager(HWND hWnd,unsigned int width, unsigned int height)
{
	swapChain = Graphics::CreateSwapChain(hWnd, width, height, 3);
	commandAllocators = Graphics::CreateCommandAllocatorPool(CommandAllocatorPool::Type::DIRECT, 3);
	fence = Graphics::CreateFence(0);
	commandList = Graphics::CreateCommandList(CommandAllocatorPool::Type::DIRECT);

	for (int i = 0; i < 3; i++)
	{
		perObjectFrameBuffers.emplace_back(100000);
	}
}

void RendererManager::Render()
{
	commandList->Reset(commandAllocators.get(), frameIndex);

	auto currentBackBuffer = swapChain->GetBuffer(frameIndex);
	auto currentBackBufferPresentToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(currentBackBuffer.Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);
	auto currentBackBufferRenderTargetToPresent = CD3DX12_RESOURCE_BARRIER::Transition(currentBackBuffer.Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT);
	commandList->Barrier(&currentBackBufferPresentToRenderTarget, 1);
	commandList->ClearRTV(swapChain->GetBackBufferDescriptor(frameIndex), clearColor);
	commandList->ClearDSV(swapChain->GetDepthBufferDescriptor());
	commandList->Barrier(&currentBackBufferRenderTargetToPresent, 1);
	commandList->Close();
	
	CommandList* lists[1] = {
		commandList.get()
	};

	swapChain->GetCommandQueue()->ExecuteCommandLists(1,lists);
	swapChain->Present();
	swapChain->GetCommandQueue()->SignalFence(fence.get());
	
	frameFenceValues[frameIndex] = fence->GetLastSignalValue();
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	fence->WaitForAValue(frameFenceValues[frameIndex]);
}
