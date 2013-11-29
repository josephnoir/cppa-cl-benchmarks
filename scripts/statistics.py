#! /opt/local/bin/python3.3 

import math

from optparse import OptionParser
import pandas as pd
import numpy as np
import scipy as sp

from scipy import stats

from pandas import rolling_std as stdev
from pandas import rolling_var as var
from pandas import rolling_mean as mean
from pandas import rolling_median as med

def confidence_interval(data, confidence=0.95):
    a = 1.0 * np.array(data)
    n = len(a)
    se = stats.sem(a)
    h = se * stats.t._ppf((1 + confidence) / 2. , n - 1)
    return h

def eval():
    return

def main():
    parser = OptionParser(usage="usage: %prog [options] file", version="%prog 0.1")
    #parser.add_option("-t", "--template", action="store", type="string", dest="template", help="declare output format")
    parser.add_option("-s", "--separator", action="store", type="string", dest="separator", help="seperator in the output file [defualt = ' ']", default=' ')

    (options, args) = parser.parse_args()
    if len(args) != 1:
        parser.error("incorrect number of arguments")
    data = np.loadtxt(args[0])
    #with open(args[0]) as fd:
    #    data2 = np.fromfile(fd, sep='\n', dtype=float)
    #print(data2)

    #df = pd.DataFrame(data)

    #print(data)
    #print(df)

    # numpy
    mean = data.mean()
    med  = np.median(data)
    var  = data.var()
    sdev = data.std()
    delta = confidence_interval(data)

    # pandas
    '''
    mean = df.mean()
    med  = df.median()
    var  = df.var()
    sdev = df.std()
    '''

    # scipy stats
    sem = stats.sem(data) # stand error of mean

    #'''
    print('mean              : {}'.format(mean))
    print('median            : {}'.format(med))
    print('variance          : {}'.format(var))
    print('standard deviation: {}'.format(sdev))
    print('stats.sem         : {}'.format(sem))
    print('conf. interval    : {}'.format(delta))
    #'''

    #print('{0}{1}{2}'.format(mean[0], options.separator, sdev[0]))

    dn, dmin_max, dmean, dvar, dskew, dkurt = stats.describe(data)
    dstd = math.sqrt(dvar)

    print('dn                 : {}'.format(dn))
    print('dmin_max           : {}'.format(dmin_max))
    print('dmean              : {}'.format(dmean))
    print('dvariance          : {}'.format(dvar))
    print('dstandard deviation: {}'.format(dstd))
    print('dskew              : {}'.format(dskew))
    print('dkurt    : {}'.format(dkurt))


if __name__ == "__main__":
    main()



