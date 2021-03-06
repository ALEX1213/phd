// Get information about available OpenCL devices.

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <stdexcept>

#include "gpu/clinfo/libclinfo.h"

namespace phd {

namespace gpu {

namespace clinfo {

const char *OpenClErrorString(cl_int err) {
  // Based on code written by @Selmar http://stackoverflow.com/a/24336429 .
  switch (err) {
    // Run-time and JIT compiler errors.
    case 0:
      return "CL_SUCCESS";
    case -1:
      return "CL_DEVICE_NOT_FOUND";
    case -2:
      return "CL_DEVICE_NOT_AVAILABLE";
    case -3:
      return "CL_COMPILER_NOT_AVAILABLE";
    case -4:
      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5:
      return "CL_OUT_OF_RESOURCES";
    case -6:
      return "CL_OUT_OF_HOST_MEMORY";
    case -7:
      return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8:
      return "CL_MEM_COPY_OVERLAP";
    case -9:
      return "CL_IMAGE_FORMAT_MISMATCH";
    case -10:
      return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11:
      return "CL_BUILD_PROGRAM_FAILURE";
    case -12:
      return "CL_MAP_FAILURE";
    case -13:
      return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14:
      return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15:
      return "CL_COMPILE_PROGRAM_FAILURE";
    case -16:
      return "CL_LINKER_NOT_AVAILABLE";
    case -17:
      return "CL_LINK_PROGRAM_FAILURE";
    case -18:
      return "CL_DEVICE_PARTITION_FAILED";
    case -19:
      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

      // Compile-time errors.
    case -30:
      return "CL_INVALID_VALUE";
    case -31:
      return "CL_INVALID_DEVICE_TYPE";
    case -32:
      return "CL_INVALID_PLATFORM";
    case -33:
      return "CL_INVALID_DEVICE";
    case -34:
      return "CL_INVALID_CONTEXT";
    case -35:
      return "CL_INVALID_QUEUE_PROPERTIES";
    case -36:
      return "CL_INVALID_COMMAND_QUEUE";
    case -37:
      return "CL_INVALID_HOST_PTR";
    case -38:
      return "CL_INVALID_MEM_OBJECT";
    case -39:
      return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40:
      return "CL_INVALID_IMAGE_SIZE";
    case -41:
      return "CL_INVALID_SAMPLER";
    case -42:
      return "CL_INVALID_BINARY";
    case -43:
      return "CL_INVALID_BUILD_OPTIONS";
    case -44:
      return "CL_INVALID_PROGRAM";
    case -45:
      return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46:
      return "CL_INVALID_KERNEL_NAME";
    case -47:
      return "CL_INVALID_KERNEL_DEFINITION";
    case -48:
      return "CL_INVALID_KERNEL";
    case -49:
      return "CL_INVALID_ARG_INDEX";
    case -50:
      return "CL_INVALID_ARG_VALUE";
    case -51:
      return "CL_INVALID_ARG_SIZE";
    case -52:
      return "CL_INVALID_KERNEL_ARGS";
    case -53:
      return "CL_INVALID_WORK_DIMENSION";
    case -54:
      return "CL_INVALID_WORK_GROUP_SIZE";
    case -55:
      return "CL_INVALID_WORK_ITEM_SIZE";
    case -56:
      return "CL_INVALID_GLOBAL_OFFSET";
    case -57:
      return "CL_INVALID_EVENT_WAIT_LIST";
    case -58:
      return "CL_INVALID_EVENT";
    case -59:
      return "CL_INVALID_OPERATION";
    case -60:
      return "CL_INVALID_GL_OBJECT";
    case -61:
      return "CL_INVALID_BUFFER_SIZE";
    case -62:
      return "CL_INVALID_MIP_LEVEL";
    case -63:
      return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64:
      return "CL_INVALID_PROPERTY";
    case -65:
      return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66:
      return "CL_INVALID_COMPILER_OPTIONS";
    case -67:
      return "CL_INVALID_LINKER_OPTIONS";
    case -68:
      return "CL_INVALID_DEVICE_PARTITION_COUNT";

      // Extension errors.
    case -1000:
      return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001:
      return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002:
      return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003:
      return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004:
      return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005:
      return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";

    default:
      return "Unknown OpenCL error";
  }
}

void OpenClCheckError(const char *api_call, cl_int err) {
  if (err != CL_SUCCESS) {
    fprintf(stderr, "%s %s\\n", api_call, OpenClErrorString(err));
    exit(1);
  }
}

template<char Remove>
bool BothAre(char lhs, char rhs) {
  return lhs == rhs && lhs == Remove;
}

void EscapeOpenCLString(std::string &string) {
  std::replace(string.begin(), string.end(), ' ', '_');
  string.erase(std::unique(string.begin(), string.end(), BothAre<'_'>),
               string.end());
}

std::string GetOpenClDeviceType(const cl::Device &device) {
  cl_device_type num = (cl_device_type) device.getInfo<CL_DEVICE_TYPE>();
  switch (num) {
    case CL_DEVICE_TYPE_CPU:
      return "CPU";
    case CL_DEVICE_TYPE_GPU:
      return "GPU";
    case CL_DEVICE_TYPE_ACCELERATOR:
      return "Accelerator";
    case 15:
      // Non-standard value '15' is used by oclgrind.
      return "Emulator";
    case 3:
      // Non-standard value '3' is used by pocl.
      return "CPU";
    default:
      return "Unknown";
  }
}

void SetOpenClDevice(const cl::Platform &platform, const cl::Device &device,
                     const int platform_id, const int device_id,
                     ::gpu::clinfo::OpenClDevice *const message) {
  std::string platform_name, device_name, driver_version, opencl_version;
  int major, minor = -1;

  platform.getInfo(CL_PLATFORM_NAME, &platform_name);
  message->set_platform_name(platform_name.c_str());

  device.getInfo(CL_DEVICE_NAME, &device_name);
  message->set_device_name(device_name.c_str());

  device.getInfo(CL_DRIVER_VERSION, &driver_version);
  message->set_driver_version(driver_version.c_str());

  platform.getInfo(CL_PLATFORM_VERSION, &opencl_version);
  sscanf(opencl_version.c_str(), "OpenCL %d.%d", &major, &minor);
  sprintf(&opencl_version[0], "%d.%d", major, minor);
  message->set_opencl_version(opencl_version.c_str());

  std::string device_type = GetOpenClDeviceType(device);
  message->set_device_type(device_type.c_str());

  EscapeOpenCLString(platform_name);
  EscapeOpenCLString(device_name);
  EscapeOpenCLString(driver_version);

  std::stringstream name;
  name << device_type.c_str() << "|"
       << platform_name.c_str() << "|"
       << device_name.c_str() << "|"
       << driver_version.c_str() << "|"
       << opencl_version.c_str();
  message->set_name(name.str());

  message->set_platform_id(platform_id);
  message->set_device_id(device_id);
}

::gpu::clinfo::OpenClDevices GetOpenClDevices() {
  ::gpu::clinfo::OpenClDevices message;
  std::vector <cl::Platform> platforms;

  try {
    cl::Platform::get(&platforms);
  } catch (cl::Error err) {
    // In environments where there are no OpenCL platforms installed, the call
    // to clGetPlatformIDs() will throw CL_PLATFORM_NOT_FOUND_KHR. We don't
    // want to treat that as an error, but instead as a lack of platforms.
    if (strcmp(err.what(), "clGetPlatformIDs") == 0 &&
        strcmp(phd::gpu::clinfo::OpenClErrorString(err.err()), "CL_PLATFORM_NOT_FOUND_KHR") == 0) {
      return message;
    }
    throw err;
  }

  std::vector <cl::Device> devices;
  int platform_id = 0;
  for (const auto &platform : platforms) {
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    int device_id = 0;
    for (const auto &device : devices) {
      phd::gpu::clinfo::SetOpenClDevice(platform, device, platform_id,
                                        device_id, message.add_device());
      ++device_id;
    }
    ++platform_id;
  }
  return message;
}

::gpu::clinfo::OpenClDevice GetOpenClDevice(const int platform_id,
                                            const int device_id) {
  ::gpu::clinfo::OpenClDevice message;
  std::vector <cl::Platform> platforms;
  cl::Platform::get(&platforms);
  std::vector <cl::Device> devices;
  int cur_platform = 0;
  for (const auto &platform : platforms) {
    if (cur_platform == platform_id) {
      platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
      int cur_device = 0;
      for (const auto &device : devices) {
        if (cur_device == device_id) {
          phd::gpu::clinfo::SetOpenClDevice(platform, device, cur_platform,
                                            cur_device, &message);
          return message;
        }
        ++cur_device;
      }
    }
    ++cur_platform;
  }
  throw std::invalid_argument("Platform and device ID not found");
}

}  // namespace clinfo

}  // namespace gpu

}  // phd
