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
    for(size_t i = 1; i < iterations; ++i) {
        spawn_cl<float*(float*)>(prog, kernel_name6, {size});
    }
    auto last = spawn_cl<float*(float*)>(prog, kernel_name6, {size});
    vector<float> m1(size);
    iota(m1.begin(), m1.end(), 0);
    send(last, m1);
    receive(
        on_arg_match >> [] (const vector<float>&) {}
    );
    shutdown();
}
