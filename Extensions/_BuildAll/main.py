import os
import subprocess

import vswhere
from colorama import Fore, init

scriptPath = os.path.split(__file__)[0]
projectPath = os.path.abspath(scriptPath + '\\..\\') + '\\'

init(autoreset=True)

if __name__ == '__main__':
    print(Fore.LIGHTGREEN_EX + '============================')
    print(Fore.LIGHTGREEN_EX + 'Process folder {}'.format(projectPath))
    print(Fore.LIGHTGREEN_EX + '============================')

    MSBuild = vswhere.get_latest_path() + '\\MSBuild\\Current\\Bin\\MSBuild.exe'
    summary = []

    for root, ds, fs in os.walk(projectPath):
        for d in ds:
            vcxprojPath = '{0}{1}\\{1}.vcxproj'.format(root, d)
            resultInfo = lambda s: 'success' if s == 0 else 'failed'

            result = []
            if os.path.exists(vcxprojPath):
                status = subprocess.call(
                    '{} -p:configuration="Release Unicode" -p:platform="Win32" -t:rebuild "{}"'.format(MSBuild,
                                                                                                       vcxprojPath))
                result.append(['Release Unicode', resultInfo(status), status])

                status = subprocess.call(
                    '{} -p:configuration="Run_Only Unicode" -p:platform="Win32" -t:rebuild "{}"'.format(MSBuild,
                                                                                                        vcxprojPath))
                result.append(['Run_Only Unicode', resultInfo(status), status])

                summary.append([d, result])

    print(Fore.LIGHTGREEN_EX + '============================')
    print(Fore.LIGHTGREEN_EX + 'Summary')
    print(Fore.LIGHTGREEN_EX + '============================')

    success = 0
    failed = 0

    for proj, info in summary:
        print(Fore.LIGHTCYAN_EX + '  {}:'.format(proj))
        for buildType, buildResult, status in info:
            success += (status == 0)
            failed += (status != 0)

            fore = Fore.LIGHTGREEN_EX if status == 0 else Fore.RED
            print(fore + '    Build {} {}'.format(buildType, buildResult))

    print(
        'Build {} projects, {} configs, {} success, {} failed'.format(len(summary), 2 * len(summary), success, failed))
