#include <vector>
#include <string>
#include <cstring>
#include <numeric>
#include <iostream>

#if defined __APPLE__ || defined(MACOSX)
    #include <OpenCL/opencl.h>
#else
    #include <CL/opencl.h>
#endif

#include "include/cmd.hpp"
#include "include/util.hpp"
#include "include/kernel.hpp"

using namespace std;


int main(int argc, char** argv) {
    if (argc < 5 || string(argv[1]) != "-s" || string(argv[3]) != "-i") {
        cout << "usage: ./" << argv[0]
             << " -s <matrix size> -i <iterations>" << endl;
        return 0;
    }
    auto matrix_size = static_cast<size_t>(stoi(argv[2]));
    auto iterations  = static_cast<size_t>(stoi(argv[4]));

    cl_int err = 0;


    // find platforms
    cl_uint num_p = 0;
    err = clGetPlatformIDs(0, nullptr, &num_p);
    if (err != CL_SUCCESS) {
        cout << "[!!!] clGetPlatformIDs: "
             << get_opencl_error(err) << endl;
        return err;
    }
    vector<cl_platform_id> p_ids(num_p);
    err = clGetPlatformIDs(num_p, p_ids.data(), nullptr);
    if (err != CL_SUCCESS) {
        cout << "[!!!] clGetPlatformIDs: "
             << get_opencl_error(err) << endl;
        return err;
    }


    // find devices
    int used_p = 0;
    cl_uint num_g = 0;
    err = clGetDeviceIDs(p_ids[used_p], CL_DEVICE_TYPE_GPU, 0, nullptr, &num_g);
    if (err != CL_SUCCESS) {
        cout << "[!!!] clGetDeviceIDs: "
             << get_opencl_error(err) << endl;
        return err;
    }
    vector<cl_device_id> devs_g(num_g);
    err = clGetDeviceIDs(p_ids[used_p], CL_DEVICE_TYPE_GPU,
                         num_g, devs_g.data(), nullptr);
    if (err != CL_SUCCESS) {
        cout << "[!!!] clGetDeviceIDs: "
             << get_opencl_error(err) << endl;
        return err;
    }


    // choose device
    cl_device_id device = devs_g[0];


    // create context
    context_ptr context;
    context.adopt(clCreateContext(0, 1, &device,
                                  nullptr,
                                  nullptr, &err));
    if (err != CL_SUCCESS) {
        cout << "[!!!] clCreateContext: "
             << get_opencl_error(err) << endl;
        return err;
    }


    // create command queue
    command_queue_ptr queue;
    queue.adopt(clCreateCommandQueue(context.get(),
                                     device,
                                     CL_QUEUE_PROFILING_ENABLE,
                                     &err));
    if (err != CL_SUCCESS) {
        cout << "[!!!] clCreateCommandQueue: "
             << get_opencl_error(err) << endl;
        return EXIT_FAILURE;
    }


    // create program
    const char* src     = kernel_source;
    size_t      src_len = strlen(src);
    program_ptr prog;
    prog.adopt(clCreateProgramWithSource(context.get(), 1,
                                         static_cast<const char**>(&src),
                                         &src_len, &err));
    if (err != CL_SUCCESS) {
        cout << "[!!!] clCreateProgramWithSource: "
             << get_opencl_error(err) << endl;
        return err;
    }


    // build program from program object
    err = clBuildProgram(prog.get(), 0, nullptr, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        cout << "[!!!] clBuildProgram: "
             << get_opencl_error(err) << endl;
        cl_build_status bs;
        err = clGetProgramBuildInfo(prog.get(), device, CL_PROGRAM_BUILD_STATUS,
                                    sizeof(cl_build_status), &bs, nullptr);
        size_t ret_val_size;
        err = clGetProgramBuildInfo(prog.get(), device, CL_PROGRAM_BUILD_LOG,
                                    0, nullptr, &ret_val_size);
        vector<char> build_log(ret_val_size+1);
        err = clGetProgramBuildInfo(prog.get(), device, CL_PROGRAM_BUILD_LOG,
                                    ret_val_size, build_log.data(), nullptr);
        build_log[ret_val_size] = '\0';
        cout << "Build log (size: " << ret_val_size
             << "): \n" << build_log.data() << endl;
        return err;
    }


    // init kernel
    kernel_ptr kernel;
    kernel.adopt(clCreateKernel(prog.get(), kernel_name, &err));
    if (err != CL_SUCCESS) {
        cout << "[!!!] clCreateKernel: "
             << get_opencl_error(err) << endl;
        return err;
    }

    cmd c(matrix_size,
          kernel,
          context,
          queue);
    for (size_t i = 0; i < iterations; ++i) {
        c.enqueue();
        c.wait();
    }

    return 0;
}
