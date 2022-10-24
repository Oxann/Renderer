#pragma once

#include "../Graphics/Graphics.h"

class RendererManager
{
public:
	RendererManager(HWND hWnd, unsigned int width, unsigned int height);
	RendererManager(const RendererManager&) = delete;
	RendererManager(RendererManager&&) = delete;
public:
	void Render();
private:
	std::unique_ptr<SwapChain> swapChain;
	std::unique_ptr<CommandAllocatorPool> commandAllocators;
	std::unique_ptr<Fence> fence;
	std::unique_ptr<CommandList> commandList;
	size_t frameFenceValues[3] = { 0 };
	unsigned int frameIndex = 0u;
	float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
};