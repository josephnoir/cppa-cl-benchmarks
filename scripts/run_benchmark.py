#! /opt/local/bin/python3.3 

import os
import subprocess
import sys

from os import listdir
from os.path import isfile, join

from optparse import OptionParser

root_path = "../"
bin_path = root_path + "build/bin/"
result_path = root_path + "results/"

# find all executables in build/bin
# we assumes those to be the available benchmarks
# if they are not build yet, you cant run anything
if not os.path.exists(bin_path):
    print("build binaries first ('./configure && make')")
    sys.exit()
bins = [f for f in listdir(bin_path) if isfile(join(bin_path, f))]

def benchmark(executable, iterations, size, postfix, samples, test):
    print("Running 'time {0} -i {1} -s {2}' for {3} samples".format(executable,
                                                                    iterations,
                                                                    size,
                                                                    samples))
    file_name = result_path + executable + postfix + ".dat"
    if not test:
        fd = open(file_name, "w")
    print("Output to: {}".format(file_name))
    for n in range(0, samples):
        rc = subprocess.check_output(["time",
                                      bin_path + executable,
                                      "-i", str(iterations),
                                      "-s", str(size)], 
                                      universal_newlines=True,
                                      stderr=subprocess.STDOUT)
        rc = [i for i in rc.split(' ') if i != ''][0]
        if not test:
            fd.write("%s\n" % rc)
        print("[{0}] {1}".format(n, rc))
    if not test:
        fd.close()
    print("Done.")


def main():
    parser = OptionParser(version="%prog 0.1")
    parser.add_option("-i", "--iterations",
                      action="store", type="int", dest="iterations",
                      help="set number of iterations [default=7]",
                      default=7)
    parser.add_option("-s", "--size",
                      action="store", type="int", dest="size",
                      help="set problem size [default=1]",
                      default=1)
    parser.add_option("-e", "--end",
                      action="store", type="int", dest="end",
                      help="make benchmarks for different sizes [size end]",
                      default=0)
    parser.add_option("-a", "--increment",
                      action="store", type="int", dest="increment",
                      help="increment size after each sample until end is reached",
                      default=0)
    parser.add_option("-n", "--numberofsamples",
                      action="store", type="int", dest="samples",
                      help="number of samples per program",
                      default=1)
    parser.add_option("-t", "--test",
                      action="store_true", dest="test",
                      default=False)

    (options, args) = parser.parse_args()
    if len(args) < 1:
        parser.error("Minimum number of arguments is one.")

    unknown = set(args) - set(bins)
    if len(unknown) != 0:
        parser.error("Unknown executable: " + repr(unknown))

    # make sure directory for results exists
    if not os.path.exists(result_path):
        os.makedirs(result_path)

    if options.end <= options.size:
        postfix = ""
        for e in args:
            benchmark(e,
                      options.iterations,
                      options.size,
                      postfix,
                      options.samples,
                      options.test)
    elif options.end > options.size and options.increment > 0:
        sample_sizes = range(options.size, options.end + 1, options.increment)
        for e in args:
            for s in sample_sizes:
                postfix = "_" + repr(s)
                benchmark(e,
                          options.iterations,
                          s,
                          postfix,
                          options.samples,
                          options.test)
    else:
        parser.error("Increment needs to be greater than 0.")


if __name__ == "__main__":
    main()



