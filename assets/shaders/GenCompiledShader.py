import os
import sys
from os import walk
import subprocess

def run(cmd):
    completed = subprocess.run(["powershell", "-Command", cmd], capture_output=True)
    if(completed.stderr is not None and completed.stderr!=b''):
        print(completed.stderr)
    return completed
def main():
    args = sys.argv
    p = args[0]
    if(not os.path.isabs(p)):
        folder=  os.getcwd()
        p = folder+'/'+p
    dirName = os.path.dirname(p)
    compilePath = dirName+'/compiled'
    isExist = os.path.exists(compilePath)
    if not isExist:
        os.makedirs(compilePath)
    fs = []
    for (dirpath, dirnames, filenames) in walk(dirName):
        fs.extend(filenames)
        break
    cmd = ''
    for f in fs:
        file_ext = None
        if f.endswith('.frag'):
            file_ext = '.frag'
        if f.endswith('.vert'):
            file_ext = '.vert'
        if file_ext is not None:
            fileName = os.path.basename(f)
            fileName = os.path.splitext(fileName)[0]
            spvFileName = fileName+file_ext+'.spv'
            spvDestFullPath =  dirName+'/compiled/'+spvFileName
            cmd += 'glslc '+dirName+'/'+f+' -o  '+ spvDestFullPath+'\n'
            #run(cmd)
            cmd += '..\../third\spirv-cross/bin\spirv-cross ' +spvDestFullPath+' --reflect --output '+ spvDestFullPath+'.meta'+'\n'
            #run(cmd)
    run(cmd)
    print('all finish')
if __name__ == "__main__":
    main()