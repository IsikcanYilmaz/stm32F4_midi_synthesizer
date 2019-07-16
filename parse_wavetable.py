#!/usr/bin/python3
import os, sys, argparse
import numpy as np
import matplotlib.pyplot as plt

'''
Read .pd file, generate .c/.h files from the arrays.

usage: parse_wavetable.py [-h] --input INPUTFILENAME [--generate_header]
                          [--outputDir OUTPUTFILENAME]

optional arguments:
  -h, --help            show this help message and exit
  --input INPUTFILENAME
                        input pd file name
  --generate_header     generate .h file
  --outputDir OUTPUTFILENAME
                        directory to output to


'''

cSourceTemplate = '''
#include "%s_wavetable.h"
const float %s_wavetable[%d] = {
%s
};
'''

hSourceTemplate = '''
#include "main.h"
#include <math.h>
extern const float %s_wavetable[%d];
'''

def displayWavetable(wavetable):
    plt.plot(wavetable)
    plt.show()

def smoothWavetable(wavetable):
    pass

def generateSource(wavetable, arrayName, generateHeader=False): # todo enable custom path
    cFileName = '%s_wavetable.c' % arrayName
    hFileName = cFileName.replace('c', 'h')
    bodyString = ''

    for p in wavetable:
        bodyString += (str(p) + ', ')
    with open(cFileName, 'w') as cFile:
        cSource = cSourceTemplate % (arrayName, arrayName, int(len(wavetable)), bodyString)
        cFile.write(cSource)
        cFile.close()

    if (generateHeader):
        hSource = hSourceTemplate % (arrayName, int(len(wavetable)))
        with open(hFileName, 'w') as hFile:
            hFile.write(hSource)
            hFile.close()


def parsePdFile(inputFileName, generateSourceCode = True, generateHeader = False):
    waveData = []
    with open(inputFileName) as inputFile:
        arraySize = None
        arrayName = ''
        bodyString = ''
        for line in inputFile:
            if ('array' in line):
                lineArr = line.split(' ')
                arrayName = lineArr[2]
                arraySize = lineArr[3]
                break

        for line in inputFile:
            line = line.replace(' ', ', ').replace('\n', '').replace(';', '')
            lineArr = line.split(", ")
            if (len(line) == 0):
                continue
            if (line[0] == '#'):
                if (line[1] == 'A'):
                    lineArr = lineArr[2:]
                else:
                    break
            lineArrFloat = np.array(lineArr).astype(np.float)
            waveData.extend(lineArrFloat)

        inputFile.close()
        return {'data':waveData, 'wavetableName':arrayName}

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', action='store', required=True, help='input pd file name', dest='inputFileName')
    parser.add_argument('--generate_header', action='store_true', required=False, default=False, help='generate .h file', dest='generateHeader')
    parser.add_argument('--output_dir', action='store', required=False, default='./', help='directory to output to', dest='outputDir')
    args = parser.parse_args()
    w = parsePdFile(args.inputFileName, args.generateHeader)
    generateSource(w['data'], w['wavetableName'], True)
    #displayWavetable(w['data'])

