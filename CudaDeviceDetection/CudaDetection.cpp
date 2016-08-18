#include "CudaDetection.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

using namespace std;

CudaDetection::CudaDetection() { }
CudaDetection::~CudaDetection() { }

bool CudaDetection::QueryDevices() {
	const unsigned int UUID_SIZE = 128;
	try {
		int device_count;
		CUDA_SAFE_CALL(cudaGetDeviceCount(&device_count));
		NVML_SAFE_CALL(nvmlInit());
		for (unsigned int i = 0; i < device_count; ++i) {
			CudaDevice cudaDevice;

			cudaDeviceProp props;
			CUDA_SAFE_CALL(cudaGetDeviceProperties(&props, i));
			// serial stuff
			nvmlPciInfo_t pciInfo;
			nvmlDevice_t device_t;
			char uuid[UUID_SIZE];
			NVML_SAFE_CALL(nvmlDeviceGetHandleByIndex(i, &device_t));
			NVML_SAFE_CALL(nvmlDeviceGetPciInfo(device_t, &pciInfo));
			NVML_SAFE_CALL(nvmlDeviceGetUUID(device_t, uuid, UUID_SIZE));

			// init device info
			cudaDevice.DeviceID = i;
			cudaDevice.DeviceName = props.name;
			cudaDevice.SMVersionString = to_string(props.major) + "." + to_string(props.minor);
			cudaDevice.SM_major = props.major;
			cudaDevice.SM_minor = props.minor;
			cudaDevice.UUID = uuid;
			cudaDevice.DeviceGlobalMemory = props.totalGlobalMem;
			cudaDevice.pciDeviceId = pciInfo.pciDeviceId;
			cudaDevice.pciSubSystemId = pciInfo.pciSubSystemId;

			_cudaDevices.push_back(cudaDevice);
		}
		NVML_SAFE_CALL(nvmlShutdown());
	}
	catch (runtime_error &err) {
		_errorMsgs.push_back(err.what());
		return false;
	}
	return true;
}

void CudaDetection::PrintDevicesJson() {
	cout << "[" << endl;
	for (int i = 0; i < _cudaDevices.size() - 1; ++i) {
		json_print(_cudaDevices[i]);
		cout << "," << endl;
	}
	json_print(_cudaDevices[_cudaDevices.size() - 1]);
	cout << endl << "]" << endl;
}

void CudaDetection::print(CudaDevice &dev) {
	cout << "DeviceID : " << dev.DeviceID << endl;
	cout << "DeviceName : " << dev.DeviceName << endl;
	cout << "SMVersionString : " << dev.SMVersionString << endl;
	cout << "UUID : " << dev.UUID << endl;
	cout << "DeviceGlobalMemory : " << dev.DeviceGlobalMemory << endl;
	cout << "pciDeviceId : " << dev.pciDeviceId << endl;
	cout << "pciSubSystemId : " << dev.pciSubSystemId << endl << endl;
}

void CudaDetection::json_print(CudaDevice &dev) {
	cout << "\t{" << endl;
	cout << "\t\t\"DeviceID\" : " << dev.DeviceID << "," << endl; // num
	cout << "\t\t\"DeviceName\" : \"" << dev.DeviceName << "\"," << endl; // string
	cout << "\t\t\"SMVersionString\" : \"" << dev.SMVersionString << "\"," << endl;  // string
	cout << "\t\t\"SM_major\" : " << dev.SM_major << "," << endl; // num
	cout << "\t\t\"SM_minor\" : " << dev.SM_minor << "," << endl; // num
	cout << "\t\t\"UUID\" : \"" << dev.UUID << "\"," << endl;  // string
	cout << "\t\t\"DeviceGlobalMemory\" : " << dev.DeviceGlobalMemory << "," << endl; // num
	cout << "\t\t\"pciDeviceId\" : " << dev.pciDeviceId << "," << endl; // num
	cout << "\t\t\"pciSubSystemId\" : " << dev.pciSubSystemId << endl; // num
	cout << "\t}";
}