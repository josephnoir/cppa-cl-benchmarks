#include <vector>
#include <iomanip>
#include <numeric>
#include <iostream>

#include "cppa/opt.hpp"
#include "cppa/cppa.hpp"
#include "cppa/opencl.hpp"

#include "include/kernel.hpp"

using namespace std;
using namespace cppa;


class copy_guy : public event_based_actor {

 public:

    copy_guy(size_t iterations,
             size_t size,
             actor_ptr worker)
        : m_count(0)
        , m_iterations(iterations)
        , m_size(size)
        , m_worker(worker)
    { }

    void init() {
        become(
            on(atom("calc")) >> [=] {
                vector<float> m1(m_size);

                iota(m1.begin(), m1.end(), 0);

                send(m_worker, move(m1));
                ++m_count;
            },
            on_arg_match >> [=] (const vector<float>&) {
                if (m_count >= m_iterations) {
                    self->quit();
                }
                else {
                    send(self, atom("calc"));
                }
            },
            others() >> [] {
                cout << "unknown message!" << endl;
            }
        );
    }

 private:

    size_t m_count;
    size_t m_iterations;
    size_t m_size;
    actor_ptr m_worker;

};


int main(int argc, char** argv) {
    size_t size = 0;
    size_t iterations  = 1;
    options_description desc;
    bool args_valid = match_stream<string>(argv + 1, argv + argc) (
        on_opt0('h', "help",       &desc, "print this text")
            >> print_desc_and_exit(&desc),
        on_opt1('s', "size",       &desc, "set matrix size (must be >= 0)")
            >> rd_arg(size),
        on_opt1('i', "iterations", &desc, "iterations (default: 1)")
            >> rd_arg(iterations)
    );
    if (!args_valid || size <= 0) print_desc_and_exit(&desc)();
    announce<vector<float>>();
    auto prog = opencl::program::create(kernel_source, 0);
    auto worker = spawn_cl<float*(float*)>(prog,
                                           kernel_name6,
                                           {size});
    auto cpy = spawn<copy_guy>(iterations, size, worker);
    send(cpy, atom("calc"));

    await_all_others_done();
    shutdown();
}
