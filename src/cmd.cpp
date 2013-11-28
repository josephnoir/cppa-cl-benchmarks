#include <numeric>
#include <iterator>
#include <iostream>

#include "cmd.hpp"

using namespace std;

cmd::cmd(size_t size,
         kernel_ptr kernel,
         context_ptr context,
         command_queue_ptr queue)
    : m_size(size)
    , m_kernel(kernel)
    , m_context(context)
    , m_queue(queue)
    , m_matrix_1(size * size)
    , m_matrix_2(size * size) { }

cmd::cmd(const cmd& other)
    : m_size(other.m_size)
    , m_kernel(other.m_kernel)
    , m_context(other.m_context)
    , m_queue(other.m_queue)
    , m_matrix_1(other.m_matrix_1)
    , m_matrix_2(other.m_matrix_2) { }

cmd::cmd(cmd&& other)
    : m_size(other.m_size)
    , m_kernel(other.m_kernel)
    , m_context(other.m_context)
    , m_queue(other.m_queue)
    , m_matrix_1(move(other.m_matrix_1))
    , m_matrix_2(move(other.m_matrix_2)) { }

void cmd::enqueue() {
    iota(begin(m_matrix_1), end(m_matrix_1), 0);
    iota(begin(m_matrix_2), end(m_matrix_2), 0);

    cl_int err;

    buf_in_1.adopt(clCreateBuffer(m_context.get(),
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  sizeof(float) * m_size * m_size,
                                  m_matrix_1.data(),
                                  &err));
    if (err != CL_SUCCESS) {
        throw runtime_error("clCreateBuffer: "
                            + get_opencl_error(err));
    }

    buf_in_2.adopt(clCreateBuffer(m_context.get(),
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  sizeof(float) * m_size * m_size,
                                  m_matrix_2.data(),
                                  &err));
    if (err != CL_SUCCESS) {
        throw runtime_error("clCreateBuffer: "
                            + get_opencl_error(err));
    }

    buf_out.adopt(clCreateBuffer(m_context.get(),
                                 CL_MEM_WRITE_ONLY,
                                 sizeof(float) * m_size * m_size,
                                 nullptr,
                                 &err));
    if (err != CL_SUCCESS) {
        throw runtime_error("clCreateBuffer: "
                            + get_opencl_error(err));
    }


    auto bi_1 = buf_in_1.get();
    err = clSetKernelArg(m_kernel.get(),
                         0,
                         sizeof(cl_mem),
                         (void*) &bi_1);
    if (err != CL_SUCCESS) {
        throw runtime_error("clSetKernelArg: "
                            + get_opencl_error(err));
    }

    auto bi_2 = buf_in_2.get();
    err = clSetKernelArg(m_kernel.get(),
                         1,
                         sizeof(cl_mem),
                         (void*) &bi_2);
    if (err != CL_SUCCESS) {
        throw runtime_error("clSetKernelArg: "
                            + get_opencl_error(err));
    }


    auto bo = buf_out.get();
    err = clSetKernelArg(m_kernel.get(),
                         2,
                         sizeof(cl_mem),
                         (void*) &bo);
    if (err != CL_SUCCESS) {
        throw runtime_error("clSetKernelArg: "
                            + get_opencl_error(err));

    }


    auto event = m_event.get();
    vector<size_t> dims = {m_size, m_size};

    // enqueue with callback ...
    err = clEnqueueNDRangeKernel(m_queue.get(),
                                 m_kernel.get(),
                                 dims.size(),
                                 nullptr, // offsets
                                 dims.data(), // golbal dimensions
                                 nullptr, // local dimensions
                                 0,
                                 nullptr,
                                 &event);
    if (err != CL_SUCCESS) {
        throw runtime_error("clEnqueueNDRangeKernel: "
                            + get_opencl_error(err));
    }
    clFlush(m_queue.get());

    err = clSetEventCallback(event,
                             CL_COMPLETE,
                             [](cl_event, cl_int, void* data) {
                                 auto c = reinterpret_cast<cmd*>(data);
                                 c->handle_results();
                             },
                             this);
    if (err != CL_SUCCESS) {
        throw runtime_error("clSetEventCallback: "
                            + get_opencl_error(err));
    }
}

void cmd::wait() {
    unique_lock<mutex> lk(m_mtx);
    m_cv.wait(lk);
}

void cmd::handle_results() {
    cl_int err{0};
    vector<float> result(m_size * m_size);
    err = clEnqueueReadBuffer(m_queue.get(),
                              buf_out.get(),
                              CL_TRUE,
                              0,
                              sizeof(float) * m_size * m_size,
                              result.data(),
                              0,
                              nullptr,
                              nullptr);
//    for (size_t column = 0; column < m_size; ++column) {
//        for (size_t row = 0; row < m_size; ++row) {
//            cout << result[row + column * m_size];
//        }
//        cout << endl;
//    }
    if (err != CL_SUCCESS) {
        throw std::runtime_error("clEnqueueReadBuffer: "
                                 + get_opencl_error(err));
    }
    m_cv.notify_all();
}
