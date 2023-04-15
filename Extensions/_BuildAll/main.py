import os
import subprocess

import vswhere

scriptPath = os.path.split(__file__)[0]
projectPath = scriptPath + '\\..\\'

if __name__ == '__main__':
    print(projectPath)
    MSBuild = vswhere.get_latest_path() + '\\MSBuild\\Current\\Bin\\MSBuild.exe'

    for root, ds, fs in os.walk(projectPath):
        for d in ds:
            vcxprojPath = '{0}{1}\\{1}.vcxproj'.format(root, d)

            if os.path.exists(vcxprojPath):
                status = subprocess.call(
                    '{} -p:configuration="release unicode" -p:platform="Win32" -t:rebuild "{}"'.format(MSBuild,
                                                                                                       vcxprojPath))
