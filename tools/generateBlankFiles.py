#!/usr/bin/python3
#
# Generate a blank C++ header and compilation unit (the 'source file'). 
#
# Usage : generateBlankFiles.py module_name include_dir source_dir
# * module_name : should follow the CamelCase convention. The first char will be capitalized.
# * include_dir : path to the folder gathering the header files.
# * source_dir : path to the folder gathering the source files.

import sys
import re
import json

# check usage
if len(sys.argv) < 4:
    print('Usage : generateBlankFiles.py module_name include_dir source_dir')
    exit()
    pass

indexOfOutput=len(sys.argv) - 1
# parse args
targetFileName=sys.argv[1][0].upper() + sys.argv[1][1:]
targetIncludeDir=sys.argv[2]
targetSourceDir=sys.argv[3]

# tokenize the class name
def camel_case_split(identifier):
    matches = re.finditer('.+?(?:(?<=[a-z])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])|$)', identifier)
    return [m.group(0) for m in matches]


nameComponents = camel_case_split(targetFileName)
guardComponents = [c.upper() for c in nameComponents]
guardComponents.extend(['HPP'])
guardName = '_'.join(guardComponents)

# generate header file

with open(targetIncludeDir+'/'+targetFileName+'.hpp', 'w') as outfile:
    outfile.write(f"""#ifndef {guardName}
#define {guardName}

// standard includes
#include <cstdint>

// esp32 includes

// project includes

// write code here...

#endif
""")


with open(targetSourceDir+'/'+targetFileName+'.cpp', 'w') as outfile:
    outfile.write('\n')
    outfile.write('// header include\n')
    outfile.write('#include "'+ targetFileName +'.hpp"\n')
    outfile.write('\n')
    outfile.write('// write code here...\n')
    outfile.write('\n')
    outfile.write(f"""
// header include
#include "{targetFileName}.hpp"

// write code here...
""")

