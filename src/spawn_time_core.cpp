#include <vector>
#include <iomanip>
#include <numeric>
#include <iostream>

#include "cppa/opt.hpp"
#include "cppa/cppa.hpp"


using namespace std;
using namespace cppa;

class dummy : public event_based_actor {
 public:
    void init() {
        become(
            others() >> [] { return self->last_dequeued(); }
        );
    }
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
    for(size_t i = 1; i < iterations; ++i) {
        spawn<dummy>();
    }
    auto last = spawn<dummy>();
    send(last, atom("done"));
    receive(
        on(atom("done")) >> [] {}
    );
    shutdown();
}
