import sys
print (sys.argv[0])
print ("compiling: ",sys.argv[1])
print ("to: ",sys.argv[2])


import os
import subprocess



tocompile = sys.argv[1]
torun = sys.argv[2]

if os.path.exists(torun):
    os.remove(torun)

subprocess.call(['g++',tocompile,'-o', torun,'-Ofast','-lSDL2','-lSDL2_image'])


if os.path.exists(torun):
    #subprocess.call(torun)
    subprocess.call("/home/sanderjelleoneil/"+torun)
else:
    print("no file likely error compiling")
    
