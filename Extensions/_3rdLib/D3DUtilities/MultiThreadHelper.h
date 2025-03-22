#pragma once

#include <d3d11_4.h>
#pragma comment(lib, "d3d11.lib")

#include "D3DDefinition.h"

// Helper class for multi-threading
// This class is used to enable multi-threading for the Direct3D 11 device context
struct MultiThreadHelper {
	HRESULT hr = S_OK;
	ComPtr<ID3D11Multithread> pD3D11Multithread;

	MultiThreadHelper(ID3D11DeviceContext* pDeviceCtx) {
		hr = pDeviceCtx->QueryInterface(IID_PPV_ARGS(&pD3D11Multithread));
		if (FAILED(hr)) { return; }

		pD3D11Multithread->SetMultithreadProtected(TRUE);
	}

	~MultiThreadHelper() {
		if (FAILED(hr)) { return; }
		pD3D11Multithread->SetMultithreadProtected(FALSE);
	}

	void Enter(){
		if (FAILED(hr)) { return; }
		pD3D11Multithread->Enter();
	}

	void Leave(){
		if (FAILED(hr)) { return; }
		pD3D11Multithread->Leave();
	}

	struct LockHelper {
		MultiThreadHelper* pMultiThreadHelper = nullptr;

		LockHelper(MultiThreadHelper* pMTH) :pMultiThreadHelper(pMTH) {
			if (pMultiThreadHelper) { pMultiThreadHelper->Enter(); }
		}

		~LockHelper() {
			if (pMultiThreadHelper) { pMultiThreadHelper->Leave(); }
		}
	};

	LockHelper GetLockHelper() { return {this};	}
};