#!/usr/bin/env python3
"""Idempotently link Fini and embed one common encryption framework in an iOS export."""
import argparse
import difflib
import hashlib
from pathlib import Path
import plistlib
import shutil
import sys
import xml.etree.ElementTree as ET

# Exported packages carry this generic parser next to the configurator.
try:
    from xcode_project import Project,render,words,version,payload_digest
except ImportError:
    sys.path.insert(0,str(Path(__file__).resolve().parents[2]/'iOS/Tools'))
    from xcode_project import Project,render,words,version,payload_digest


def configure(bundle,common=None,target=None,minimum='17.0',dry_run=False):
    bundle=Path(bundle).resolve()
    if bundle.suffix!='.xcodeproj':
        found=list(bundle.glob('*.xcodeproj'))
        if len(found)!=1:raise ValueError('Specify one exported .xcodeproj')
        bundle=found[0]
    root=bundle.parent;path=bundle/'project.pbxproj';text=path.read_text(encoding='utf-8-sig');project=Project(text)
    objects={key:project.plain(key) for key in project.objects}
    apps=[(key,obj) for key,obj in objects.items() if obj.get('isa')=='PBXNativeTarget' and obj.get('productType')=='com.apple.product-type.application' and (not target or obj.get('name')==target)]
    if len(apps)!=1:raise ValueError('Expected one App target; use --target')
    app_id,app=apps[0]
    for sdk in ('iphoneos','iphonesimulator'):
        native=root/f'libFini_{sdk}.a'
        if not native.is_file() or (native.read_bytes()[:8]!=b'!<arch>\n' and native.read_bytes()[:4] not in (b'\xca\xfe\xba\xbe',b'\xbe\xba\xfe\xca',b'\xca\xfe\xba\xbf',b'\xbf\xba\xfe\xca')):raise ValueError('Missing/invalid exported archive: '+str(native))
    destination=root/'Frameworks/OFEEncryption.xcframework';source=Path(common).resolve() if common else destination
    if not (source/'Info.plist').is_file():raise ValueError('Supply the separately built common runtime with --common /path/to/OFEEncryption.xcframework')
    info=plistlib.loads((source/'Info.plist').read_bytes())
    variants={entry.get('SupportedPlatformVariant','device') for entry in info.get('AvailableLibraries',[]) if entry.get('SupportedPlatform')=='ios' and 'arm64' in entry.get('SupportedArchitectures',[])}
    if not {'device','simulator'}.issubset(variants):raise ValueError('Shared framework requires arm64 device/simulator slices')
    changes={};additions={}
    def current(key):return changes.get(key,objects[key])
    def stable(name):return hashlib.sha256(('Fini export '+app_id+' '+name).encode()).hexdigest()[:24].upper()
    def add(name,obj):
        key=stable(name)
        if key in objects and objects[key]!=obj:raise ValueError('Project identifier collision: '+key)
        if key not in objects:additions[key]=obj
        return key
    refs=[key for key,obj in objects.items() if obj.get('isa')=='PBXFileReference' and obj.get('path')=='Frameworks/OFEEncryption.xcframework']
    ref=refs[0] if refs else add('shared-ref',dict(isa='PBXFileReference',lastKnownFileType='wrapper.xcframework',path='Frameworks/OFEEncryption.xcframework',sourceTree='SOURCE_ROOT'))
    framework_phase=next((phase for phase in app['buildPhases'] if objects[phase]['isa']=='PBXFrameworksBuildPhase'),None)
    if not framework_phase:raise ValueError('App has no Frameworks build phase')
    if not any(objects[file].get('fileRef')==ref for file in objects[framework_phase].get('files',[])):
        build=add('shared-link',dict(isa='PBXBuildFile',fileRef=ref));obj=dict(objects[framework_phase]);obj['files']=[*obj.get('files',[]),build];changes[framework_phase]=obj
    if not any(objects.get(file,{}).get('fileRef')==ref for phase in app['buildPhases'] if objects[phase]['isa']=='PBXCopyFilesBuildPhase' for file in objects[phase].get('files',[])):
        build=add('shared-embed',dict(isa='PBXBuildFile',fileRef=ref,settings={'ATTRIBUTES':['CodeSignOnCopy','RemoveHeadersOnCopy']}))
        phase=add('embed-phase',dict(isa='PBXCopyFilesBuildPhase',buildActionMask='2147483647',dstPath='',dstSubfolderSpec='10',files=[build],name='Embed shared OFEEncryption',runOnlyForDeploymentPostprocessing='0'))
        app=dict(app);app['buildPhases']=[*app['buildPhases'],phase];changes[app_id]=app
    main=objects[project.root.value['rootObject'].value]['mainGroup']
    if not refs:
        obj=dict(current(main));obj['children']=[*obj.get('children',[]),ref];changes[main]=obj
    # Static SDK libraries must use required linkage and explicit build dependencies.
    products={obj.get('productReference'):key for key,obj in objects.items() if obj.get('isa')=='PBXNativeTarget'}
    app=dict(current(app_id));dependencies=list(app.get('dependencies',[]))
    known={objects[dep].get('target') for dep in dependencies}
    for build_id in objects[framework_phase].get('files',[]):
        build=objects[build_id];file=objects[build['fileRef']]
        if not file.get('path','').endswith('.a'):continue
        if 'Weak' in build.get('settings',{}).get('ATTRIBUTES',[]):
            replacement=dict(build);settings=dict(build['settings']);settings['ATTRIBUTES']=[value for value in settings['ATTRIBUTES'] if value!='Weak'];replacement['settings']=settings;changes[build_id]=replacement
        dependency=products.get(build['fileRef'])
        if dependency and dependency not in known:
            proxy=add('proxy-'+dependency,dict(isa='PBXContainerItemProxy',containerPortal=project.root.value['rootObject'].value,proxyType='1',remoteGlobalIDString=dependency,remoteInfo=objects[dependency]['name']))
            item=add('dependency-'+dependency,dict(isa='PBXTargetDependency',target=dependency,targetProxy=proxy));dependencies.append(item);known.add(dependency)
    if dependencies!=app.get('dependencies',[]):app['dependencies']=dependencies;changes[app_id]=app
    configs=list(objects[app['buildConfigurationList']]['buildConfigurations'])
    app_configs=set(configs)
    linked={objects[file].get('fileRef') for file in objects[framework_phase].get('files',[])}
    for obj in objects.values():
        if obj.get('isa')=='PBXNativeTarget' and obj.get('productType')=='com.apple.product-type.library.static' and obj.get('productReference') in linked:
            configs.extend(objects[obj['buildConfigurationList']]['buildConfigurations'])
    project_configs=objects[objects[project.root.value['rootObject'].value]['buildConfigurationList']]['buildConfigurations']
    floor=max([minimum]+[objects[key]['buildSettings'].get('IPHONEOS_DEPLOYMENT_TARGET',minimum) for key in set(configs+project_configs)],key=version)
    for key in configs:
        obj=dict(objects[key]);settings=dict(obj['buildSettings'])
        for sdk in ('iphoneos','iphonesimulator'):
            name=f'OTHER_LDFLAGS[sdk={sdk}*]';flags=words(settings.get(name));base=words(settings.get('OTHER_LDFLAGS'))
            if base and not any(flags[index:index+len(base)]==base for index in range(len(flags)-len(base)+1)):flags=base+flags
            for flag in ('$(inherited)',f'-lFini_{sdk}','-ObjC','-lc++','-lz'):
                if flag not in flags:flags.append(flag)
            settings[name]=flags
        search=words(settings.get('LIBRARY_SEARCH_PATHS'))
        for item in ('$(inherited)','$(SRCROOT)'):
            if item not in search:search.append(item)
        settings['LIBRARY_SEARCH_PATHS']=search
        for name in list(settings):
            if name.split('[',1)[0] in ('ARCHS','VALID_ARCHS','EXCLUDED_ARCHS','ONLY_ACTIVE_ARCH'):del settings[name]
        settings['ARCHS']='arm64';settings['ONLY_ACTIVE_ARCH']='NO';settings['EXCLUDED_ARCHS']=''
        settings['CODE_SIGNING_ALLOWED[sdk=iphonesimulator*]']='NO'
        settings['IPHONEOS_DEPLOYMENT_TARGET']=floor
        obj['buildSettings']=settings
        if obj!=objects[key]:changes[key]=obj
    edits=[]
    for key,obj in changes.items():
        node=project.objects[key];edits.append((node.start,node.end,render(obj,2)))
    if additions:
        at=project.root.value['objects'].end-1
        edits.append((at,at,''.join(key+' = '+render(obj,2)+';\n' for key,obj in additions.items())))
    changed=text
    for start,end,replacement in sorted(edits,reverse=True):changed=changed[:start]+replacement+changed[end:]
    Project(changed)
    updates={path:changed} if changed!=text else {}
    native={key:obj for key,obj in objects.items() if obj.get('isa')=='PBXNativeTarget'}
    for scheme in bundle.glob('xcshareddata/xcschemes/*.xcscheme'):
        old=scheme.read_text();xml=ET.fromstring(old);dirty=False
        for node in xml.iter('BuildableReference'):
            obj=native.get(node.get('BlueprintIdentifier'))
            if not obj:continue
            desired={'ReferencedContainer':'container:'+bundle.name,'BlueprintName':obj['name'],'BuildableName':objects[obj['productReference']]['path']}
            for key,value in desired.items():
                if node.get(key)!=value:node.set(key,value);dirty=True
        if dirty:updates[scheme]=ET.tostring(xml,encoding='unicode')+'\n'
    if dry_run:
        for file,data in updates.items():print(''.join(difflib.unified_diff(file.read_text().splitlines(True),data.splitlines(True),fromfile=str(file),tofile=str(file))))
    else:
        if source!=destination:
            if destination.exists():
                left={p.relative_to(source).as_posix():payload_digest(p) for p in source.rglob('*') if p.is_file()}
                right={p.relative_to(destination).as_posix():payload_digest(p) for p in destination.rglob('*') if p.is_file()}
                if left!=right:
                    changed=sorted(name for name in set(left)|set(right) if left.get(name)!=right.get(name))
                    raise ValueError('Refusing to overwrite a different installed common framework: '+', '.join(changed))
            else:
                destination.parent.mkdir(parents=True,exist_ok=True);shutil.copytree(source,destination)
        for file,data in updates.items():file.write_text(data)
    return len(updates)

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('project',type=Path);parser.add_argument('--common',type=Path);parser.add_argument('--target');parser.add_argument('--dry-run',action='store_true');parser.add_argument('--minimum-ios',default='17.0')
    args=parser.parse_args();print('Updated project files:',configure(args.project,args.common,args.target,args.minimum_ios,args.dry_run))
