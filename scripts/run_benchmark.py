#! /opt/local/bin/python3.3 

import os
import subprocess

from os import listdir
from os.path import isfile, join

from optparse import OptionParser

root_path = "../"
bin_path = root_path + "build/bin/"
result_path = root_path + "results/"

bins = [f for f in listdir(bin_path) if isfile(join(bin_path, f))]

def main():
    parser = OptionParser(version="%prog 0.1")
    parser.add_option("-i", "--iterations", action="store", type="int", dest="iter",
                      help="set number of iterations [default=7]", default=7)
    parser.add_option("-s", "--size"      , action="store", type="int", dest="size",
                      help="set problem size [default=1]", default=1)
    parser.add_option("-n", "--numberofsamples", action="store", type="int", dest="samples",
                      help="number of samples per program", default=1)

    (options, args) = parser.parse_args()
    if len(args) < 1:
        parser.error("[!!!] Minimum number of arguments is 1.")
    unknown = set(args) - set(bins)
    if len(unknown) != 0:
        parser.error("[!!!] Unknown executable: " + repr(unknown))

    for e in args:
        print("Running 'time {0} -i {1} -s {2}'".format(e, options.iter, options.size))
        if not os.path.exists(result_path):
            os.makedirs(result_path)
        fd = open(result_path + e + ".dat", "w")
        for n in range(0, options.samples):
            rc = subprocess.check_output(["time", bin_path + e, "-i", str(options.iter), "-s", str(options.size)], 
                                         universal_newlines=True,
                                         stderr=subprocess.STDOUT)
            rc = [i for i in rc.split(' ') if i != ''][0]
            fd.write("%s\n" % rc)
            print("[{0}] Output: {1}".format(n, rc))
        fd.close()
        print("Done.")


if __name__ == "__main__":
    main()

