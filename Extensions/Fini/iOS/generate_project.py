#!/usr/bin/env python3
"""Generate Fini's owned Xcode project on any host; configure local SDKs on a Mac.
No exporter SDK source is copied into the project or distribution.
"""
import argparse
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parent

def uid(name):
    return hashlib.sha1(('Fini.iOS.'+name).encode()).hexdigest()[:24].upper()


def build_model():
    objects={}
    def add(key,isa,**fields):
        objects[uid(key)]={'isa':isa,**fields};return uid(key)
    source=add('source','PBXFileReference',lastKnownFileType='sourcecode.cpp.objcpp',path='Runtime/CRunFini.mm',sourceTree='SOURCE_ROOT')
    header=add('header','PBXFileReference',lastKnownFileType='sourcecode.c.h',path='Runtime/CRunFini.h',sourceTree='SOURCE_ROOT')
    config=add('config','PBXFileReference',lastKnownFileType='text.xcconfig',path='Config/Fini.xcconfig',sourceTree='SOURCE_ROOT')
    product=add('product','PBXFileReference',explicitFileType='archive.ar',path='libFini.a',sourceTree='BUILT_PRODUCTS_DIR',includeInIndex='0')
    build_file=add('source.build','PBXBuildFile',fileRef=source)
    sources=add('sources','PBXSourcesBuildPhase',buildActionMask='2147483647',files=[build_file],runOnlyForDeploymentPostprocessing='0')
    frameworks=add('frameworks','PBXFrameworksBuildPhase',buildActionMask='2147483647',files=[],runOnlyForDeploymentPostprocessing='0')
    packaging=add('package','PBXShellScriptBuildPhase',buildActionMask='2147483647',files=[],inputPaths=[],outputPaths=[],alwaysOutOfDate='1',runOnlyForDeploymentPostprocessing='0',shellPath='/bin/sh',shellScript='set -eu\npython3 "$SRCROOT/../Mobile/build_ios.py" --runtime "$FINI_RUNTIME_ROOT" --openssl "$FINI_OPENSSL_BASE" --output "$FINI_OUTPUT_ROOT" --prepare-openssl\n')
    configurations={}
    for target in ('project','Fini','FiniXCFramework'):
        configs=[]
        for name in ('Debug','Release'):
            settings={'GCC_OPTIMIZATION_LEVEL':'0' if name=='Debug' else 's','DEBUG_INFORMATION_FORMAT':'dwarf' if name=='Debug' else 'dwarf-with-dsym'}
            if target=='Fini':
                settings.update({'PRODUCT_NAME':'Fini','SKIP_INSTALL':'NO','PUBLIC_HEADERS_FOLDER_PATH':'include/Fini'})
            configs.append(add(target+'.'+name,'XCBuildConfiguration',baseConfigurationReference=config,buildSettings=settings,name=name))
        configurations[target]=add(target+'.configs','XCConfigurationList',buildConfigurations=configs,defaultConfigurationIsVisible='0',defaultConfigurationName='Release')
    native=add('Fini','PBXNativeTarget',buildConfigurationList=configurations['Fini'],buildPhases=[sources,frameworks],buildRules=[],dependencies=[],name='Fini',productName='Fini',productReference=product,productType='com.apple.product-type.library.static')
    aggregate=add('FiniXCFramework','PBXAggregateTarget',buildConfigurationList=configurations['FiniXCFramework'],buildPhases=[packaging],dependencies=[],name='FiniXCFramework',productName='FiniXCFramework')
    products=add('products','PBXGroup',children=[product],name='Products',sourceTree='<group>')
    files=add('files','PBXGroup',children=[source,header,config],name='Fini sources and configuration',sourceTree='<group>')
    main=add('main','PBXGroup',children=[files,products],sourceTree='<group>')
    project=add('project','PBXProject',attributes={'BuildIndependentTargetsInParallel':'YES','LastUpgradeCheck':'1500'},buildConfigurationList=configurations['project'],compatibilityVersion='Xcode 14.0',developmentRegion='en',hasScannedForEncodings='0',knownRegions=['en','Base'],mainGroup=main,productRefGroup=products,projectDirPath='',projectRoot='',targets=[native,aggregate])
    return {'archiveVersion':'1','classes':{},'objectVersion':'56','objects':objects,'rootObject':project}


def openstep(value,level=0):
    indent='\t'*level
    if isinstance(value,dict):
        return '{\n'+''.join(indent+'\t'+json.dumps(k)+' = '+openstep(v,level+1)+';\n' for k,v in value.items())+indent+'}'
    if isinstance(value,list):
        return '(\n'+''.join(indent+'\t'+openstep(v,level+1)+',\n' for v in value)+indent+')'
    return json.dumps(str(value),ensure_ascii=False)


def scheme(target,product):
    root=ET.Element('Scheme',LastUpgradeVersion='1500',version='1.3')
    action=ET.SubElement(root,'BuildAction',parallelizeBuildables='YES',buildImplicitDependencies='YES')
    entries=ET.SubElement(action,'BuildActionEntries')
    entry=ET.SubElement(entries,'BuildActionEntry',buildForTesting='NO',buildForRunning='YES',buildForProfiling='NO',buildForArchiving='YES',buildForAnalyzing='YES')
    ET.SubElement(entry,'BuildableReference',BuildableIdentifier='primary',BlueprintIdentifier=uid(target),BuildableName=product,BlueprintName=target,ReferencedContainer='container:Fini.xcodeproj')
    ET.SubElement(root,'AnalyzeAction',buildConfiguration='Debug')
    ET.SubElement(root,'ArchiveAction',buildConfiguration='Release',revealArchiveInOrganizer='NO')
    ET.indent(root)
    return ET.tostring(root,encoding='unicode',xml_declaration=True)+'\n'


def generate(output=ROOT):
    project=output/'Fini.xcodeproj';schemes=project/'xcshareddata/xcschemes';schemes.mkdir(parents=True,exist_ok=True)
    (project/'project.pbxproj').write_text('// !$*UTF8*$!\n'+openstep(build_model())+'\n')
    for name,product in [('Fini','libFini.a'),('FiniXCFramework','FiniXCFramework')]:
        (schemes/(name+'.xcscheme')).write_text(scheme(name,product))
    workspace=project/'project.xcworkspace';workspace.mkdir(exist_ok=True)
    (workspace/'contents.xcworkspacedata').write_text('<?xml version="1.0" encoding="UTF-8"?>\n<Workspace version="1.0"><FileRef location="self:"/></Workspace>\n')
    return project


def xcpath(value):
    # Prevent comments, build-setting expansion or newline injection in xcconfig paths.
    text=str(value.resolve())
    if any(c in text for c in ('\n','\r','$','"')) or '//' in text:
        raise ValueError('Unsupported character in local SDK path: '+text)
    return '"'+text+'"'


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runtime',type=Path,help='Licensed exported project containing Classes/')
    parser.add_argument('--openssl',type=Path,help='OpenSSL prefixes for all three iOS slices')
    args=parser.parse_args()
    local=[]
    if args.runtime:
        if not (args.runtime/'Classes/Extensions/CRunExtension.h').is_file():
            parser.error('--runtime must contain Classes/Extensions/CRunExtension.h')
        local.append('FINI_RUNTIME_ROOT = '+xcpath(args.runtime))
    if args.openssl:
        local.append('FINI_OPENSSL_BASE = '+xcpath(args.openssl))
    if local:
        (ROOT/'Config/Local.xcconfig').write_text('// Machine-local SDK locations. Do not commit.\n'+'\n'.join(local)+'\n')
    print('Xcode project ready:',generate())
    print('Open Fini.xcodeproj, choose FiniXCFramework, then Product > Build.')

if __name__=='__main__':
    main()
