#include "Engine/Graphics/WebGPU/WebGPURenderer.h"

using namespace andromeda::graphics;

WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options) {
	// A simple structure holding the local information shared with the
	// onAdapterRequestEnded callback.
	struct UserData {
		WGPUAdapter adapter = nullptr;
		bool requestEnded = false;
	};
	UserData userData;

	// Callback called by wgpuInstanceRequestAdapter when the request returns
	// This is a C++ lambda function, but could be any function defined in the
	// global scope. It must be non-capturing (the brackets [] are empty) so
	// that it behaves like a regular C function pointer, which is what
	// wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
	// is to convey what we want to capture through the pUserData pointer,
	// provided as the last argument of wgpuInstanceRequestAdapter and received
	// by the callback as its last argument.
	auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* pUserData, void* pUserdata2) {
		UserData& userData = *reinterpret_cast<UserData*>(pUserData);
		if (status == WGPURequestAdapterStatus_Success) {
			userData.adapter = adapter;
		} else {
			std::cout << "Could not get WebGPU adapter: " << message.data << std::endl;
		}
		userData.requestEnded = true;
	};

	WGPURequestAdapterCallbackInfo rcbi;
	rcbi.callback = onAdapterRequestEnded;
	rcbi.userdata1 = (void*)&userData;

	// Call to the WebGPU request adapter procedure
	wgpuInstanceRequestAdapter(instance /* equivalent of navigator.gpu */, options, rcbi);

	// We wait until userData.requestEnded gets true
	// [...] Wait for request to end

	assert(userData.requestEnded);
	return userData.adapter;
}

/**
 * Utility function to get a WebGPU device, so that
 *     WGPUDevice device = requestDeviceSync(adapter, options);
 * is roughly equivalent to
 *     const device = await adapter.requestDevice(descriptor);
 * It is very similar to requestAdapter
 */
WGPUDevice requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor) {
	struct UserData {
		WGPUDevice device = nullptr;
		bool requestEnded = false;
	};
	UserData userData;

	auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* pUserData, void* pUserData2) {
		UserData& userData = *reinterpret_cast<UserData*>(pUserData);
		if (status == WGPURequestDeviceStatus_Success) {
			userData.device = device;
		} else {
			std::cout << "Could not get WebGPU device: " << message.data << std::endl;
		}
		userData.requestEnded = true;
	};

	WGPURequestDeviceCallbackInfo requestDeviceCallback;
	requestDeviceCallback.callback = onDeviceRequestEnded;
	requestDeviceCallback.userdata1 = (void*)&userData;

	wgpuAdapterRequestDevice(adapter, descriptor, requestDeviceCallback);

#ifdef __EMSCRIPTEN__
	while (!userData.requestEnded) {
		emscripten_sleep(100);
	}
#endif // __EMSCRIPTEN__

	assert(userData.requestEnded);

	return userData.device;
}

bool WGPURenderer::Initialize() {
	WGPUInstanceDescriptor desc = {};
	m_instance = wgpuCreateInstance(&desc);

	if (!m_instance) {
		andromeda::error("Could not intialize WebGPU");
		return false;
	}


	WGPURequestAdapterOptions adapterOptions = {};
	adapterOptions.nextInChain = nullptr;

	m_adapter = requestAdapterSync(m_instance, &adapterOptions);

	WGPULimits supportedLimits = {};
	auto res = wgpuAdapterGetLimits(m_adapter, &supportedLimits);
	if (res == WGPUStatus_Success) {
		std::cout << "Adapter limits:" << std::endl;
		std::cout << " - maxTextureDimension1D: " << supportedLimits.maxTextureDimension1D << std::endl;
		std::cout << " - maxTextureDimension2D: " << supportedLimits.maxTextureDimension2D << std::endl;
		std::cout << " - maxTextureDimension3D: " << supportedLimits.maxTextureDimension3D << std::endl;
		std::cout << " - maxTextureArrayLayers: " << supportedLimits.maxTextureArrayLayers << std::endl;
	}

	WGPUDeviceDescriptor deviceDescriptor = {};
	deviceDescriptor.nextInChain = nullptr;
	deviceDescriptor.label = {"My Device", 10 }; // anything works here, that's your call
	deviceDescriptor.requiredFeatureCount = 0; // we do not require any specific feature
	deviceDescriptor.requiredLimits = nullptr; // we do not require any specific limit
	deviceDescriptor.defaultQueue.nextInChain = nullptr;
	deviceDescriptor.defaultQueue.label = {"The default queue", 18 };

	m_device = requestDeviceSync(m_adapter, &deviceDescriptor);
    m_queue = wgpuDeviceGetQueue(m_device);

	return true;
}

void WGPURenderer::Shutdown() {
    wgpuQueueRelease(m_queue);
	wgpuDeviceRelease(m_device);
	wgpuAdapterRelease(m_adapter);
	wgpuInstanceRelease(m_instance);
    andromeda::trace("shut down WGPU");
}