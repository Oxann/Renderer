#include "SwapChain.h"
#include "Device.h"
#include "CommandQueue.h"
#include "../EngineAssert.h"

using namespace Microsoft::WRL;

CommandQueue* SwapChain::GetCommandQueue()
{
	return commandQueue.get();
}

ComPtr<ID3D12Resource> SwapChain::GetBuffer(unsigned int index)
{
	ComPtr<ID3D12Resource> backBuffer;
	swapChain->GetBuffer(index, __uuidof(ID3D12Resource), &backBuffer);
	return backBuffer;
}

const Descriptor& SwapChain::GetBackBufferDescriptor(unsigned int index)
{
	return backBufferDescriptors[index];
}

const Descriptor& SwapChain::GetDepthBufferDescriptor()
{
	return depthBufferDescriptor;
}

unsigned int SwapChain::GetCurrentBackBufferIndex()
{
	return swapChain->GetCurrentBackBufferIndex();
}

void SwapChain::Present()
{
	CHECK_DX_ERROR(swapChain->Present(1, 0));
}
