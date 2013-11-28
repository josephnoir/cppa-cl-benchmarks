#ifndef CMD_HPP
#define CMD_HPP

#include <mutex>
#include <vector>
#include <string>
#include <future>
#include <condition_variable>

#include "include/util.hpp"

class cmd {

 public:

    cmd(size_t size,
        kernel_ptr kernel,
        context_ptr constext,
        command_queue_ptr queue);
    cmd(const cmd& other);
    cmd(cmd&& other);
    void enqueue();

    void wait();

 private:

    size_t m_size;

    std::mutex m_mtx;
    std::condition_variable m_cv;

    mem_ptr buf_in_1;
    mem_ptr buf_in_2;
    mem_ptr buf_out;
    event_ptr m_event;
    kernel_ptr m_kernel;
    context_ptr m_context;
    command_queue_ptr m_queue;

    std::vector<float> m_matrix_1;
    std::vector<float> m_matrix_2;

    void handle_results();
};

#endif //CMD_HPP
