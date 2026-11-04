#import "CRunFini.h"
#import "CRun.h"
#import "CRunApp.h"
#import "CExtension.h"
#import "CActExtension.h"
#import "CCndExtension.h"
#import "CFile.h"
#import "CValue.h"
#import "CObject.h"
#import "CObjInfo.h"
#import "CRVal.h"
#import "CRMvt.h"
#import "CRCom.h"
#include "../../../Fini/Mobile/Core.h"

using fini::Json;
static std::string utf8(NSString* s) {if(!s)return {};NSData* d=[s dataUsingEncoding:NSUTF8StringEncoding];fini::check(d!=nil,"Invalid Unicode string");return std::string((const char*)[d bytes],[d length]);}
static NSString* ns(const std::string& s) {return [[[NSString alloc] initWithBytes:s.data() length:s.size() encoding:NSUTF8StringEncoding] autorelease];}
static void replace(NSString*& dst,NSString* src) {[src retain];[dst release];dst=src;}
static NSMapTable* stashed;

@implementation CRunFini
-(fini::State&)state {return *static_cast<fini::State*>(finiState);}
-(int)getNumberOfConditions {return 4;}
-(BOOL)createRunObject:(CFile*)file withCOB:(CCreateObjectInfo*)cob andVersion:(int)version {
    cf25p=version>=2&&[file readAByte]!=0;allowRVforCS=version>=2&&[file readAByte]!=0;keep=version>=3&&[file readAByte]!=0;
    currentSection=[@"" retain];currentItem=[@"" retain];sectionLoop=[@"" retain];itemLoop=[@"" retain];
    try {finiState=new fini::State;
        if(!stashed)stashed=[[NSMapTable weakToStrongObjectsMapTable] retain];
        if(keep){NSMutableDictionary* data=[stashed objectForKey:rh->rhApp];NSString* value=[data objectForKey:ho->hoOiList->oilName];if(value){[self state].load(utf8(value));[data removeObjectForKey:ho->hoOiList->oilName];}}
        return YES;
    }catch(const std::exception& e){NSLog(@"Fini create: %s",e.what());return NO;}
}
-(void)destroyRunObject:(BOOL)fast {
    if(!finiState)return;
    try {[self state].flush();}catch(const std::exception& e){NSLog(@"Fini auto save: %s",e.what());}
    if(keep&&[self state].initialized()) {
        NSMutableDictionary* data=[stashed objectForKey:rh->rhApp];
        if(!data){data=[NSMutableDictionary dictionary];[stashed setObject:data forKey:rh->rhApp];}
        NSString* name=ho->hoOiList->oilName;if(![data objectForKey:name])[data setObject:ns([self state].save()) forKey:name];
    }
    delete static_cast<fini::State*>(finiState);finiState=nullptr;
    [currentSection release];[currentItem release];[sectionLoop release];[itemLoop release];
}
-(CObject*)object:(int)fixed {CObject* o=[ho getObjectFromFixed:fixed];return o&&[o fixedValue]==fixed&&!(o->hoFlags&HOF_DESTROYED)?o:nil;}
-(void)updateLocalization {
    if(!linked)return;CObject* o=[self object:localizationFixed];
    if([o isKindOfClass:[CExtension class]]&&[((CExtension*)o)->ext isKindOfClass:[CRunFini class]]) {
        CRunFini* other=(CRunFini*)((CExtension*)o)->ext;
        [self state].localizeFrom(other->finiState?[other state].save():"");
    }else {linked=NO;[self state].localizeFrom("");}
}
-(int)handleRunObject {return 0;}
-(std::string)writePath:(NSString*)path {return utf8([rh->rhApp getPathForWriting:path]);}
-(std::string)readBytes:(NSString*)path {
    // Exporter resolver handles bundled resources as well as sandbox files.
    NSData* bytes=[rh->rhApp loadResourceData:path];fini::check(bytes!=nil,"Open INI input");
    return fini::base64(std::string((const char*)[bytes bytes],[bytes length]));
}
-(BOOL)condition:(int)n withCndExtension:(CCndExtension*)c {
    if(!finiState)return NO;
    try {switch(n){
        case 0:return [sectionLoop isEqualToString:[c getParamExpString:rh withNum:0]];
        case 1:return [itemLoop isEqualToString:[c getParamExpString:rh withNum:0]];
        case 2:return [self state].request("hasItem",{utf8([c getParamExpString:rh withNum:0]),utf8([c getParamExpString:rh withNum:1])}).get<bool>();
        case 3:return [self state].request("hasSection",{utf8([c getParamExpString:rh withNum:0])}).get<bool>();
    }}catch(const std::exception& e){NSLog(@"Fini condition: %s",e.what());}return NO;
}
-(BOOL)valuesAllowed:(CObject*)o {return o&&o->rov&&(o->hoType>=32||o->hoType==2||((o->hoType==3||o->hoType==7)&&cf25p&&allowRVforCS));}
-(int)valueCount:(CObject*)o {return cf25p?o->rov->rvNumberOfValues:std::min(26,o->rov->rvNumberOfValues);}
-(int)stringCount:(CObject*)o {return cf25p?o->rov->rvNumberOfStrings:std::min(10,o->rov->rvNumberOfStrings);}
-(std::string)packValues:(CObject*)o {
    if(![self valuesAllowed:o])return "";
    Json data={{"flags",static_cast<uint32_t>(o->rov->rvValueFlags)},{"values",Json::array()},{"strings",Json::array()}};
    for(int i=0;i<[self valueCount:o];++i){CValue* v=[o->rov getValue:i];if([v getType]==TYPE_INT)data["values"].push_back({{"type",0},{"value",[v getInt]}});else data["values"].push_back({{"type",2},{"value",[v getDouble]}});}
    for(int i=0;i<[self stringCount:o];++i)data["strings"].push_back(utf8([o->rov getString:i]));
    return fini::packValues(data);
}
-(void)unpackValues:(CObject*)o text:(const std::string&)text {
    if(![self valuesAllowed:o])return;
    auto data=fini::unpackValues(text,[self valueCount:o],[self stringCount:o]);
    o->rov->rvValueFlags=data["flags"].get<uint32_t>();
    for(int i=0;i<[self valueCount:o];++i){auto v=data["values"][i];CValue* dst=[o->rov getValue:i];if(v["type"]==0)[dst forceInt:v["value"].get<int>()];else [dst forceDouble:v["value"].get<double>()];}
    for(int i=0;i<[self stringCount:o];++i)[o->rov setString:i withString:ns(data["strings"][i].get<std::string>())];
}
-(void)action:(int)n withActExtension:(CActExtension*)a {
    if(!finiState)return;
    auto s=[&](int i){return utf8([a getParamExpString:rh withNum:i]);};
    auto f=[&](int i){return [a getParamFilename2:rh withNum:i];};
    auto integer=[&](int i){return [a getParamExpression:rh withNum:i];};
    try {auto& state=[self state];switch(n){
        case 0:state.reset();break;case 1:state.release();break;
        case 2:{NSString* file=f(0);auto key=s(1);state.flush();state.request("fileData",{[self readBytes:file],key,[self writePath:file]});break;}
        case 3:{NSString* file=f(0);auto key=s(1);state.request("fileSave",{[self writePath:file],key});break;}
        case 4:{auto sec=s(0),item=s(1);float value=[a getParamExpDouble:rh withNum:2];state.request("setNumber",{sec,item,value});break;}
        case 5:state.request("set",{s(0),s(1),s(2)});break;
        case 6:state.request("delete",{s(0),s(1)});break;
        case 7:{auto loop=s(0);auto names=state.names();for(const auto& name:names){if(!finiState)break;replace(sectionLoop,ns(loop));replace(currentSection,ns(name.get<std::string>()));[ho generateEvent:0 withParam:0];}break;}
        case 8:{auto sec=s(0),loop=s(1);if(!sec.empty())replace(currentSection,ns(sec));sec=utf8(currentSection);auto names=state.names(&sec);for(const auto& name:names){if(!finiState)break;replace(currentSection,ns(sec));replace(itemLoop,ns(loop));replace(currentItem,ns(name.get<std::string>()));[ho generateEvent:1 withParam:0];}break;}
        case 9:state.load(s(0));break;
        case 10:{NSString* file=f(0);state.request("autoOn",{[self writePath:file],s(1)});break;}
        case 11:state.autoSave=false;break;
        case 12:state.request("copy",{s(0),s(1),integer(2)!=0});break;
        case 13:{int fixed=integer(0);auto text=s(1);[self unpackValues:[self object:fixed] text:text];break;}
        case 14:{int fixed=integer(0);auto text=s(1);CObject* o=[self object:fixed];if(o){auto xy=state.request("unpackPosition",{text});o->hoX=xy[0];o->hoY=xy[1];if(o->roc)o->roc->rcChanged=YES;}break;}
        case 15:case 19:{auto data=fini::unbase64(s(0));state.load(n==15?fini::decompress(data):data);break;}
        case 16:{linked=NO;NSString* file=f(0);auto key=s(1);if([file length])state.request("localBytes",{[self readBytes:file],key});else state.localizeFrom("");break;}
        case 17:state.language=s(0);break;
        case 18:localizationFixed=integer(0);linked=YES;[self updateLocalization];break;
    }}catch(const std::exception& e){NSLog(@"Fini action %d: %s",n,e.what());}
}
-(CValue*)expression:(int)n {
    if(!finiState)return n==0?[rh getTempValue:0]:[rh getTempString:@""];
    try {auto& state=[self state];switch(n){
        case 0:{auto sec=utf8([[ho getExpParam] getString]),key=utf8([[ho getExpParam] getString]);return [rh getTempDouble:state.request("number",{sec,key}).get<double>()];}
        case 1:{auto sec=utf8([[ho getExpParam] getString]),key=utf8([[ho getExpParam] getString]);[self updateLocalization];return [rh getTempString:ns(state.get(sec,key))];}
        case 2:return [rh getTempString:currentSection];case 3:return [rh getTempString:currentItem];case 4:return [rh getTempString:ns(state.save())];
        case 5:return [rh getTempString:ns([self packValues:[self object:[[ho getExpParam] getInt]]])];
        case 6:{CObject* o=[self object:[[ho getExpParam] getInt]];return [rh getTempString:o?ns(state.request("packPosition",{o->hoX,o->hoY}).get<std::string>()):@""];}
        case 7:case 8:return [rh getTempString:ns(fini::base64(n==7?fini::compress(state.save()):state.save()))];
        case 9:case 10:{auto text=utf8([[ho getExpParam] getString]);BOOL compressed=[[ho getExpParam] getInt]!=0;return [rh getTempString:ns(state.request(n==9?"encode":"decode",{text,bool(compressed)}).get<std::string>())];}
    }}catch(const std::exception& e){NSLog(@"Fini expression %d: %s",n,e.what());}
    return n==0?[rh getTempValue:0]:[rh getTempString:@""];
}
@end
