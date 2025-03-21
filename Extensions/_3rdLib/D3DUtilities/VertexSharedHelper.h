#pragma once

#include <directxmath.h>

#ifdef PRE_COMPILE_SHADER
#include "ShaderCompiler.h"
#else
#include "ShaderResourceCompiler.h"
#endif

// ------------------------------------------------
// VertexSharedHelper
// ------------------------------------------------
// 
// for 2D rendering, vertex routine are mostly the same
// so we can create a helper class to simplify the shared process

struct VertexSharedHelper {
    HRESULT hr = S_OK;

	using VertexShaderBundle = ShaderCompiler::VertexShaderBundle;
	VertexShaderBundle vsBundle = ShaderCompiler::GetNullBundle<VertexShaderBundle>();

	ComPtr<ID3D11InputLayout> pInputLayout;
	ComPtr<ID3D11Buffer> pVertexBuffer;

	struct VertexBufferInfo {
		UINT stride = 0u;
		UINT offset = 0u;
	} vertexBufferInfo = {};

	size_t indicesSize = 0u;
	ComPtr<ID3D11Buffer> pIndexBuffer;

#ifdef PRE_COMPILE_SHADER
    VertexSharedHelper(ShaderCompiler* pCompiler, const void* pShaderBytecode, SIZE_T bytecodeLength) {
#else
    VertexSharedHelper(ShaderResourceCompiler * pCompiler, const HMODULE hInstLib, const int resourceId,
        const char* pEntryPoint = ShaderCompiler::DEFAULT_ENTRYPOINT,
        const char* pTarget = ShaderCompiler::DEFAULT_TARGET) {
#endif
		// ------------------------------------
		// device
		// ------------------------------------
		auto pDevice = pCompiler->pDevice;

		// ------------------------------------
		// vertex shader
		// ------------------------------------
#ifdef PRE_COMPILE_SHADER
        vsBundle = pCompiler->CreateVertexShader(pShaderBytecode, bytecodeLength);
#else
        vsBundle = pCompiler->CreateVertexShader(hInstLib, resourceId, pEntryPoint, pTarget);
#endif
		auto& [vsBlob, vs] = vsBundle;

		// ------------------------------------
		// input layouts
		// ------------------------------------

		// What we expect in shader
		// struct VSInput {
		//     float3 position : POSITION;
		//     float3 color : COLOR0;
		// };

		// We should then create an input layout exactly with this format
		using Position = DirectX::XMFLOAT3;
		using Color = DirectX::XMFLOAT3;

		struct VertexShaderInput {
			Position position = {};
			Color color = {};
		};

		D3D11_INPUT_ELEMENT_DESC vertexInputLayoutInfo[] = {
			{
				"POSITION",
				0,
				DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				offsetof(VertexShaderInput, position),
				D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				offsetof(VertexShaderInput, color),
				D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
				0
			}
		};

		hr = pDevice->CreateInputLayout(vertexInputLayoutInfo, std::size(vertexInputLayoutInfo),
#ifdef PRE_COMPILE_SHADER
            pShaderBytecode, bytecodeLength,
#else
            vsBlob.Get()->GetBufferPointer(), vsBlob.Get()->GetBufferSize(),
#endif
            &pInputLayout);
		if (FAILED(hr)) { return; }

		// ------------------------------------
		// vertex buffer
		// ------------------------------------

		// Center: (0, 0)
		// 
		//  (-1, 1)┌──┬──┐(1, 1)
		//         ├──┼──┤
		//  (-1,-1)└──┴──┘(1,-1)
		//

		constexpr Position POSITION_LT = { -1, 1, 0 };  // 0
		constexpr Position POSITION_RT = { 1, 1, 0 };  // 1
		constexpr Position POSITION_RB = { 1,-1, 0 };  // 2
		constexpr Position POSITION_LB = { -1,-1, 0 };  // 3

		constexpr Color COLOR_WHITE = { 1.0f, 1.0f, 1.0f };

		constexpr VertexShaderInput vertices[] = {
			{ POSITION_LT, COLOR_WHITE },
			{ POSITION_RT, COLOR_WHITE },
			{ POSITION_RB, COLOR_WHITE },
			{ POSITION_LB, COLOR_WHITE },
		};

		D3D11_BUFFER_DESC vsibd = {};
		vsibd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vsibd.ByteWidth = sizeof(vertices);
		vsibd.StructureByteStride = sizeof(VertexShaderInput);

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;

		hr = pDevice->CreateBuffer(&vsibd, &sd, &pVertexBuffer);
		if (FAILED(hr)) { return; }

		vertexBufferInfo = {
			.stride = sizeof(VertexShaderInput),
			.offset = 0u
		};

		// ------------------------------------
		// indice buffer
		// ------------------------------------

		// D3D render triangle in clockwise order

		//
		//  (0)┌──┬──┐(1)
		//     ├──┼──┤
		//  (3)└──┴──┘(2)
		//

		const UINT16 indices[] = {
			0,1,2,      // first
			0,2,3,      // second
		};

		indicesSize = std::size(indices);
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.ByteWidth = sizeof(indices);
		ibd.StructureByteStride = sizeof(UINT16);

		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices;

		hr = pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);
		if (FAILED(hr)) { return; }
	}

    inline void UpdateContext(ID3D11DeviceContext* pDeviceCtx) {
        pDeviceCtx->IASetInputLayout(pInputLayout.Get());

        ID3D11Buffer* vertexBuffers[] = { pVertexBuffer.Get() };
        pDeviceCtx->IASetVertexBuffers(0, std::size(vertexBuffers), vertexBuffers,
            &vertexBufferInfo.stride,
            &vertexBufferInfo.offset);

        pDeviceCtx->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        pDeviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        auto& [vsBlob, vs] = vsBundle;
        pDeviceCtx->VSSetShader(vs.Get(), 0, 0);
    }
};