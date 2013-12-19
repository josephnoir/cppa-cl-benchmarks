#! /opt/local/bin/python3.3 

import os
import subprocess

from os import listdir
from os.path import isfile, join

from optparse import OptionParser

root_path = "../"
bin_path = root_path + "build/bin/"
bins = [f for f in listdir(bin_path) if isfile(join(bin_path, f))]

def main():
    parser = OptionParser(version="%prog 0.1")
    parser.add_option("-s", "--iterations", action="store", type="int", dest="iter",
                      help="set number of iterations [default=7]", default=7)
    parser.add_option("-i", "--size"      , action="store", type="int", dest="size",
                      help="set problem size [default=1]", default=1)

    (options, args) = parser.parse_args()
    if len(args) < 1:
        parser.error("[!!!] Minimum number of arguments is 1.")
    unknown = set(args) - set(bins)
    if len(unknown) != 0:
        parser.error("[!!!] Unknown executable: " + repr(unknown))

    for e in args:
        print("Running '{0} -s {1} -i {2}'".format(e, options.size, options.iter))
        subprocess.check_output([bin_path + e, "-s", str(options.size), "-i ", str(options.iter)])
        print("Done.")


if __name__ == "__main__":
    main()

