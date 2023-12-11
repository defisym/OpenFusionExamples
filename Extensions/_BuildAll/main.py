import argparse
import os
import subprocess

import vswhere
from colorama import Fore, init

scriptPath = os.path.split(__file__)[0]
projectPath = os.path.abspath(scriptPath + '\\..\\') + '\\'

init(autoreset=True)

parser = argparse.ArgumentParser(description='Build all extensions')

# https://stackoverflow.com/questions/15753701/how-can-i-pass-a-list-as-a-command-line-argument-with-argparse
# configs, e.g., debug / release
parser.add_argument('--configs', nargs='+')

# https://learn.microsoft.com/zh-cn/visualstudio/msbuild/msbuild-targets?view=vs-2022
# target, e.g., build, rebuild
parser.add_argument('--target')
parser.add_argument('--platform')

args = parser.parse_args()

configs = args.configs
configs = configs if configs is not None else ['Release Unicode', 'Run_Only Unicode']

target = args.target
target = target if target is not None else 'rebuild'

platform = args.platform
platform = platform if platform is not None else 'Win32'

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
                print(Fore.LIGHTCYAN_EX + 'building project {}'.format(vcxprojPath))

                for config in configs:
                    print(Fore.LIGHTBLUE_EX, 'building config {}...'.format(config))
                    # command line help
                    # https://learn.microsoft.com/zh-cn/cpp/build/walkthrough-using-msbuild-to-create-a-visual-cpp-project?view=msvc-170
                    # https://learn.microsoft.com/zh-cn/visualstudio/msbuild/msbuild-command-line-reference?view=vs-2022
                    status = subprocess.call(
                        '{} -p:configuration="{}" -p:platform="{}" -t:{} "{}"'.format(MSBuild, config, platform, target,
                                                                                      vcxprojPath))
                    result.append([config, resultInfo(status), status])

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
        'Build {} projects, {} configs, {} success, {} failed'.format(len(summary), len(configs), success, failed))
