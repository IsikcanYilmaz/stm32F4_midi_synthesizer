#!/usr/bin/python3
import os, sys, argparse
import numpy as np
import matplotlib.pyplot as plt

'''
Read .pd file, generate .c/.h files from the arrays.

usage: parse_wavetable.py [-h] --input INPUTFILENAME [--generate_header]
                          [--output_dir OUTPUTFILENAME]

optional arguments:
  -h, --help            show this help message and exit
  --input INPUTFILENAME
                        input pd file name
  --generate_header     generate .h file
  --output_dir OUTPUTFILENAME
                        directory to output to
  --plotonly            only display the wave
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

def smoothWavetable(wavetable, box_pts):
    box = np.ones(box_pts)/box_pts
    y_smooth = np.convolve(wavetable, box, mode='same')
    return y_smooth

def generateSource(wavetable, arrayName, generateHeader=False): # todo enable custom path
    cFileName = '%s_wavetable.c' % arrayName
    hFileName = cFileName.replace('c', 'h')
    bodyString = ''

    for j,p in enumerate(wavetable):
        bodyString += (str(p) + ', ')
        if (j % 8 == 0 and j > 0):
            bodyString += "\ \n"
    with open(cFileName, 'w') as cFile:
        cSource = cSourceTemplate % (arrayName, arrayName, int(len(wavetable)), bodyString)
        cFile.write(cSource)
        cFile.close()
        print("[*] %s" % (cFileName))

    if (generateHeader):
        hSource = hSourceTemplate % (arrayName, int(len(wavetable)))
        with open(hFileName, 'w') as hFile:
            hFile.write(hSource)
            hFile.close()
        print("[*] %s" % (hFileName))

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
        return (waveData, arrayName)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', action='store', required=True, help='input pd file name', dest='inputFileName')
    parser.add_argument('--generate_header', action='store_true', required=False, default=False, help='generate .h file', dest='generateHeader')
    parser.add_argument('--output_dir', action='store', required=False, default='./', help='directory to output to', dest='outputDir')
    parser.add_argument('--plotonly', action='store_true', required=False, default=False, help='only display the wave', dest='plotonly')
    args = parser.parse_args()
    waveTableData, waveTableName = parsePdFile(args.inputFileName, args.generateHeader)
    if (args.plotonly):
        displayWavetable(waveTableData)
        displayWavetable(smoothWavetable(waveTableData,128))
    else:
        pass
        generateSource(waveTableData, waveTableName, True)

