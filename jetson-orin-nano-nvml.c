#include <nvml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * gcc -I/usr/local/cuda-12.6/targets/aarch64-linux/include -L /usr/local/cuda-12.6/targets/aarch64-linux/lib/stubs jetson-orin-nano-nvml.c -o jetson-orin-nano-nvml -lnvidia-ml
*/

static int get_device_count(unsigned int *device_count_ptr) {
    nvmlReturn_t result;

    result = nvmlDeviceGetCount(device_count_ptr);
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "ERROR: failed to query device count: %s\n", nvmlErrorString(result));

        return -1;
    }

    return 1;
}

static int get_device_name(nvmlDevice_t device_handle, char *device_name) {
    nvmlReturn_t result;

    result = nvmlDeviceGetName(device_handle, device_name, NVML_DEVICE_NAME_BUFFER_SIZE);
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "ERROR: failed to get device name: %s\n", nvmlErrorString(result));

        return -1;
    }

    return 1;
}

static int get_device_uuid(nvmlDevice_t device_handle, char *device_uuid) {
    nvmlReturn_t result;

    result = nvmlDeviceGetUUID(device_handle, device_uuid, NVML_DEVICE_UUID_V2_BUFFER_SIZE);
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "ERROR: failed to get device UUID: %s\n", nvmlErrorString(result));

        return -1;
    }

    return 1;
}

static int get_device_arch(nvmlDevice_t device_handle, nvmlDeviceArchitecture_t *device_arch_ptr) {
    nvmlReturn_t result;

    result = nvmlDeviceGetArchitecture(device_handle, device_arch_ptr);
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "ERROR: failed to get device architecture: %s\n", nvmlErrorString(result));

        return -1;
    }

    return 1;
}

static void get_device_arch_name(nvmlDeviceArchitecture_t device_arch, char *device_arch_name) {
    switch (device_arch) {
        case NVML_DEVICE_ARCH_KEPLER:
            strncpy(device_arch_name, "Kepler", strlen("Kepler") + 1);
            break;
        case NVML_DEVICE_ARCH_MAXWELL:
            strncpy(device_arch_name, "Maxwell", strlen("Maxwell") + 1);
            break;
        case NVML_DEVICE_ARCH_PASCAL:
            strncpy(device_arch_name, "Pascal", strlen("Pascal") + 1);
            break;
        case NVML_DEVICE_ARCH_VOLTA:
            strncpy(device_arch_name, "Volta", strlen("Volta") + 1);
            break;
        case NVML_DEVICE_ARCH_TURING:
            strncpy(device_arch_name, "Turing", strlen("Turing") + 1);
            break;
        case NVML_DEVICE_ARCH_AMPERE:
            strncpy(device_arch_name, "Ampere", strlen("Ampere") + 1);
            break;
        case NVML_DEVICE_ARCH_ADA:
            strncpy(device_arch_name, "Ada", strlen("Ada") + 1);
            break;
        case NVML_DEVICE_ARCH_HOPPER:
            strncpy(device_arch_name, "Hopper", strlen("Hopper") + 1);
            break;
        case NVML_DEVICE_ARCH_BLACKWELL:
            strncpy(device_arch_name, "Blackwell", strlen("Blackwell") + 1);
            break;
        case NVML_DEVICE_ARCH_UNKNOWN:
            strncpy(device_arch_name, "UNKNOWN", strlen("UNKNOWN") + 1);
            break;
    }
}

static int get_device_compute_cap(nvmlDevice_t device_handle, int *major, int *minor) {
    nvmlReturn_t result;

    result = nvmlDeviceGetCudaComputeCapability(device_handle, major, minor);
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "ERROR: failed to get device compute capability: %s\n", nvmlErrorString(result));

        return -1;
    }

    return 1;
}

int main(void) {
    nvmlReturn_t result;
    unsigned int device_count;
    unsigned int i;

    // initialize NVML library
    result = nvmlInit();
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "ERROR: failed to initialize NVML: %s\n", nvmlErrorString(result));

        return 1;
    }

    // get GPU count
    if (get_device_count(&device_count) > 0) {
        fprintf(stdout, "GPU Count: %u\n", device_count);
    } else {
        goto error_handler;
    }

    // iterate each GPU
    // GPU count should be 1 in Jetson Orin Nano. this code block is for ref. use only
    for (i = 0; i < device_count; ++i) {
        nvmlDevice_t device;
        char device_name[NVML_DEVICE_NAME_BUFFER_SIZE];
        char device_uuid[NVML_DEVICE_UUID_V2_BUFFER_SIZE];

        nvmlDeviceArchitecture_t device_arch;
        char device_arch_name[BUFSIZ];

        int major;
        int minor;

        // query for device handle
        result = nvmlDeviceGetHandleByIndex(i, &device);
        if (result != NVML_SUCCESS) {
            fprintf(stderr, "ERROR: failed to query device index %u: %s\n", i, nvmlErrorString(result));

            continue;
        }

        // get device name
        if (get_device_name(device, device_name) < 0) {
            strncpy(device_name, "NO DEVICE NAME", strlen("NO DEVICE NAME") + 1);
        }

        // get device UUID
        if (get_device_uuid(device, device_uuid) < 0) {
            strncpy(device_uuid, "NO DEVICE UUID", strlen("NO DEVICE UUID") + 1);
        }

        // get device architecture name
        if (get_device_arch(device, &device_arch) < 0) {
            strncpy(device_arch_name, "NO DEVICE ARCH NAME", strlen("NO DEVICE ARCH NAME") + 1);
        } else {
            get_device_arch_name(device_arch, device_arch_name);
        }

        // get device compute capability
        if (get_device_compute_cap(device, &major, &minor) < 0) {
            major = 0;
            minor = 0;
        }

        // print final output
        fprintf(stdout, "GPU [%s] [%u: %s] UUID: %s Compute Capability: [%i.%i]\n", device_arch_name, i, device_name, device_uuid, major, minor);
    }

    // shutdown NVML library
    result = nvmlShutdown();
    if (NVML_SUCCESS != result) {
        fprintf(stderr, "ERROR: failed to shutdown NVML: %s\n", nvmlErrorString(result));
    }

    return 0;

error_handler:
    result = nvmlShutdown();
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "ERROR: failed to shutdown NVML: %s\n", nvmlErrorString(result));
    }

    return 1;
}
