#include <qfile.h>
#include <assert.h>

#include "sortdict.h"
#include "marshal.h"
#include "entry.h"
#include "section.h"
#include "memberlist.h"
#include "definition.h"
#include "groupdef.h"
#include "example.h"


void marshalInt(StorageIntf *s,int v)
{
  uchar b[4];
  b[0]=((uint)v)>>24;
  b[1]=(((uint)v)>>16)&0xff;
  b[2]=(((uint)v)>>8)&0xff;
  b[3]=v&0xff;
  s->write((const char *)b,4);
}

void marshalUInt(StorageIntf *s,uint v)
{
  uchar b[4];
  b[0]=v>>24;
  b[1]=(v>>16)&0xff;
  b[2]=(v>>8)&0xff;
  b[3]=v&0xff;
  s->write((const char *)b,4);
}

void marshalBool(StorageIntf *s,bool b)
{
  char c = b;
  s->write(&c,sizeof(char));
}

void marshalQCString(StorageIntf *s,const QCString &str)
{
  uint l=str.length();
  marshalUInt(s,l);
  if (l>0) s->write(str.data(),l);
}

void marshalQGString(StorageIntf *s,const QGString &str)
{
  uint l=str.length();
  marshalUInt(s,l);
  if (l>0) s->write(str.data(),l);
}

void marshalArgumentList(StorageIntf *s,ArgumentList *argList)
{
  if (argList==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,argList->count());
    if (argList->count()>0)
    {
      ArgumentListIterator ali(*argList);
      Argument *a;
      for (ali.toFirst();(a=ali.current());++ali)
      {
        marshalQCString(s,a->attrib);    
        marshalQCString(s,a->type);    
        marshalQCString(s,a->canType);    
        marshalQCString(s,a->name);    
        marshalQCString(s,a->array);    
        marshalQCString(s,a->defval);    
        marshalQCString(s,a->docs);    
      }
    }
    marshalBool(s,argList->constSpecifier);
    marshalBool(s,argList->volatileSpecifier);
    marshalBool(s,argList->pureSpecifier);
  }
}

void marshalArgumentLists(StorageIntf *s,QList<ArgumentList> *argLists)
{
  if (argLists==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,argLists->count());
    QListIterator<ArgumentList> ali(*argLists);
    ArgumentList *al;
    for (ali.toFirst();(al=ali.current());++ali)
    {
      marshalArgumentList(s,al);
    }
  }
}

void marshalBaseInfoList(StorageIntf *s, QList<BaseInfo> *baseList)
{
  if (baseList==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,baseList->count());
    QListIterator<BaseInfo> bli(*baseList);
    BaseInfo *bi;
    for (bli.toFirst();(bi=bli.current());++bli)
    {
      marshalQCString(s,bi->name);
      marshalInt(s,(int)bi->prot);
      marshalInt(s,(int)bi->virt);
    }
  }
}

void marshalGroupingList(StorageIntf *s, QList<Grouping> *groups)
{
  if (groups==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,groups->count());
    QListIterator<Grouping> gli(*groups);
    Grouping *g;
    for (gli.toFirst();(g=gli.current());++gli)
    {
      marshalQCString(s,g->groupname);
      marshalInt(s,(int)g->pri);
    }
  }
}

void marshalSectionInfoList(StorageIntf *s, QList<SectionInfo> *anchors)
{
  if (anchors==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,anchors->count());
    QListIterator<SectionInfo> sli(*anchors);
    SectionInfo *si;
    for (sli.toFirst();(si=sli.current());++sli)
    {
      marshalQCString(s,si->label);
      marshalQCString(s,si->title);
      marshalQCString(s,si->ref);
      marshalInt(s,(int)si->type);
      marshalQCString(s,si->fileName);
    }
  }
}

void marshalItemInfoList(StorageIntf *s, QList<ListItemInfo> *sli)
{
  if (sli==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,sli->count());
    QListIterator<ListItemInfo> liii(*sli);
    ListItemInfo *lii;
    for (liii.toFirst();(lii=liii.current());++liii)
    {
      marshalQCString(s,lii->type);
      marshalInt(s,lii->itemId);
    }
  }
}

void marshalObjPointer(StorageIntf *s,void *obj)
{
  char *b = (char *)&obj;
  s->write(b,sizeof(void *));
}

void marshalSectionDict(StorageIntf *s,SectionDict *sections)
{
  if (sections==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,sections->count());
    QDictIterator<SectionInfo> sli(*sections);
    SectionInfo *si;
    for (sli.toFirst();(si=sli.current());++sli)
    {
      marshalQCString(s,sli.currentKey());
      marshalObjPointer(s,si);
    }
  }
}

void marshalMemberSDict(StorageIntf *s,MemberSDict *memberSDict)
{
  if (memberSDict==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,memberSDict->count());
    //printf("  marshalMemberSDict: items=%d\n",memberSDict->count());
    SDict<MemberDef>::IteratorDict mdi(*memberSDict);
    MemberDef *md;
    int count=0;
    for (mdi.toFirst();(md=mdi.current());++mdi)
    {
      //printf("  marshalMemberSDict: %d: key=%s value=%p\n",count,mdi.currentKey().data(),md);
      marshalQCString(s,mdi.currentKey());
      marshalObjPointer(s,md);
      count++;
    }
    assert(count==memberSDict->count());
  }
}

void marshalDocInfo(StorageIntf *s,DocInfo *docInfo)
{
  if (docInfo==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,1); 
    marshalQCString(s,docInfo->doc);
    marshalInt(s,docInfo->line);
    marshalQCString(s,docInfo->file);
  }
}

void marshalBriefInfo(StorageIntf *s,BriefInfo *briefInfo)
{
  if (briefInfo==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,1); 
    marshalQCString(s,briefInfo->doc);
    marshalQCString(s,briefInfo->tooltip);
    marshalInt(s,briefInfo->line);
    marshalQCString(s,briefInfo->file);
  }
}

void marshalBodyInfo(StorageIntf *s,BodyInfo *bodyInfo)
{
  if (bodyInfo==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,1); 
    marshalInt(s,bodyInfo->startLine);
    marshalInt(s,bodyInfo->endLine);
    marshalObjPointer(s,bodyInfo->fileDef);
  }
}

void marshalGroupList(StorageIntf *s,GroupList *groupList)
{
  if (groupList==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,groupList->count());
    QListIterator<GroupDef> gli(*groupList);
    GroupDef *gd=0;
    for (gli.toFirst();(gd=gli.current());++gli)
    {
      marshalObjPointer(s,gd);
    }
  }
}

void marshalMemberList(StorageIntf *s,MemberList *ml)
{
  if (ml==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,1); // not a null pointer
    ml->marshal(s);
  }
}

void marshalExampleSDict(StorageIntf *s,ExampleSDict *ed)
{
  if (ed==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,ed->count());
    //printf("  marshalMemberSDict: items=%d\n",memberSDict->count());
    SDict<Example>::IteratorDict edi(*ed);
    Example *e;
    for (edi.toFirst();(e=edi.current());++edi)
    {
      //printf("  marshalMemberSDict: %d: key=%s value=%p\n",count,mdi.currentKey().data(),md);
      marshalQCString(s,edi.currentKey());
      marshalQCString(s,e->anchor);
      marshalQCString(s,e->name);
      marshalQCString(s,e->file);
    }
  }
}

void marshalMemberLists(StorageIntf *s,SDict<MemberList> *mls)
{
  if (mls==0)
  {
    marshalUInt(s,NULL_LIST); // null pointer representation
  }
  else
  {
    marshalUInt(s,mls->count());
    //printf("  marshalMemberSDict: items=%d\n",memberSDict->count());
    SDict<MemberList>::IteratorDict mli(*mls);
    MemberList *ml;
    for (mli.toFirst();(ml=mli.current());++mli)
    {
      //printf("  marshalMemberSDict: %d: key=%s value=%p\n",count,mdi.currentKey().data(),md);
      marshalQCString(s,mli.currentKey());
      marshalObjPointer(s,ml); // assume we are not owner of the list
    }
  }
}

//------------------------------------------------------------------

int unmarshalInt(StorageIntf *s)
{
  uchar b[4];
  s->read((char *)b,4);
  int result=(int)((((uint)b[0])<<24)+((uint)b[1]<<16)+((uint)b[2]<<8)+(uint)b[3]);
  //printf("unmarshalInt: %x %x %x %x: %x offset=%llx\n",b[0],b[1],b[2],b[3],result,f.pos());
  return result;
}

uint unmarshalUInt(StorageIntf *s)
{
  uchar b[4];
  s->read((char *)b,4);
  uint result=(((uint)b[0])<<24)+((uint)b[1]<<16)+((uint)b[2]<<8)+(uint)b[3];
  //printf("unmarshalUInt: %x %x %x %x: %x offset=%llx\n",b[0],b[1],b[2],b[3],result,f.pos());
  return result;
}

bool unmarshalBool(StorageIntf *s)
{
  char result;
  s->read(&result,sizeof(result));
  //printf("unmarshalBool: %x offset=%llx\n",result,f.pos());
  return result;
}

QCString unmarshalQCString(StorageIntf *s)
{
  uint len = unmarshalUInt(s);
  //printf("unmarshalQCString: len=%d offset=%llx\n",len,f.pos());
  QCString result(len+1);
  result.at(len)='\0';
  if (len>0)
  {
    s->read(result.data(),len);
  }
  //printf("unmarshalQCString: result=%s\n",result.data());
  return result;
}

QGString unmarshalQGString(StorageIntf *s)
{
  uint len = unmarshalUInt(s);
  //printf("unmarshalQCString: len=%d offset=%llx\n",len,f.pos());
  QGString result(len+1);
  result.at(len)='\0';
  if (len>0)
  {
    s->read(result.data(),len);
  }
  //printf("unmarshalQCString: result=%s\n",result.data());
  return result;
}

ArgumentList *unmarshalArgumentList(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s);
  if (count==NULL_LIST) return 0; // null list
  ArgumentList *result = new ArgumentList;
  assert(count<1000000);
  //printf("unmarshalArgumentList: %d\n",count);
  for (i=0;i<count;i++)
  {
    Argument *a = new Argument;
    a->attrib  = unmarshalQCString(s);
    a->type    = unmarshalQCString(s);
    a->canType = unmarshalQCString(s);
    a->name    = unmarshalQCString(s);
    a->array   = unmarshalQCString(s);
    a->defval  = unmarshalQCString(s);
    a->docs    = unmarshalQCString(s);
    result->append(a);
  }
  result->constSpecifier    = unmarshalBool(s);
  result->volatileSpecifier = unmarshalBool(s);
  result->pureSpecifier     = unmarshalBool(s);
  return result;
}

QList<ArgumentList> *unmarshalArgumentLists(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s);
  if (count==NULL_LIST) return 0; // null list
  QList<ArgumentList> *result = new QList<ArgumentList>;
  result->setAutoDelete(TRUE);
  assert(count<1000000);
  //printf("unmarshalArgumentLists: %d\n",count);
  for (i=0;i<count;i++)
  {
    result->append(unmarshalArgumentList(s));
  }
  return result;
}

QList<BaseInfo> *unmarshalBaseInfoList(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s);
  if (count==NULL_LIST) return 0; // null list
  QList<BaseInfo> *result = new QList<BaseInfo>;
  result->setAutoDelete(TRUE);
  assert(count<1000000);
  for (i=0;i<count;i++)
  {
    QCString name   = unmarshalQCString(s);
    Protection prot = (Protection)unmarshalInt(s);
    Specifier virt  = (Specifier)unmarshalInt(s);
    result->append(new BaseInfo(name,prot,virt));
  }
  return result;
}

QList<Grouping> *unmarshalGroupingList(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s);
  if (count==NULL_LIST) return 0; // null list
  QList<Grouping> *result = new QList<Grouping>;
  result->setAutoDelete(TRUE);
  assert(count<1000000);
  for (i=0;i<count;i++)
  {
    QCString name = unmarshalQCString(s);
    Grouping::GroupPri_t prio = (Grouping::GroupPri_t)unmarshalInt(s);
    result->append(new Grouping(name,prio));
  }
  return result;
}

QList<SectionInfo> *unmarshalSectionInfoList(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s);
  if (count==NULL_LIST) return 0; // null list
  QList<SectionInfo> *result = new QList<SectionInfo>;
  result->setAutoDelete(TRUE);
  assert(count<1000000);
  for (i=0;i<count;i++)
  { 
    QCString label = unmarshalQCString(s);
    QCString title = unmarshalQCString(s);
    QCString ref   = unmarshalQCString(s);
    SectionInfo::SectionType type = (SectionInfo::SectionType)unmarshalInt(s);
    QCString fileName = unmarshalQCString(s);
    result->append(new SectionInfo(fileName,label,title,type,ref));
  }
  return result;
}

QList<ListItemInfo> *unmarshalItemInfoList(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s);
  if (count==NULL_LIST) return 0; // null list
  QList<ListItemInfo> *result = new QList<ListItemInfo>;
  result->setAutoDelete(TRUE);
  assert(count<1000000);
  for (i=0;i<count;i++)
  { 
    ListItemInfo *lii = new ListItemInfo;
    lii->type   = unmarshalQCString(s);
    lii->itemId = unmarshalInt(s);
    result->append(lii);
  }
  return result;
}

void *unmarshalObjPointer(StorageIntf *s)
{
  void *result;
  s->read((char *)&result,sizeof(void*));
  return result;
}

SectionDict *unmarshalSectionDict(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s);
  //printf("unmarshalSectionDict count=%d\n",count);
  if (count==NULL_LIST) return 0; // null list
  SectionDict *result = new SectionDict(17);
  assert(count<1000000);
  for (i=0;i<count;i++)
  {
    QCString key    = unmarshalQCString(s);
    SectionInfo *si = (SectionInfo *)unmarshalObjPointer(s);
    //printf("  unmarshalSectionDict i=%d key=%s si=%s\n",count,key.data(),si->label.data());
    result->insert(key,si);
  }
  return result;
}

MemberSDict *unmarshalMemberSDict(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s);
  //printf("--- unmarshalMemberSDict count=%d\n",count);
  if (count==NULL_LIST) 
  {
    //printf("--- end unmarshalMemberSDict\n");
    return 0; // null list
  }
  MemberSDict *result = new MemberSDict;
  assert(count<1000000);
  //printf("Reading %d key-value pairs\n",count);
  for (i=0;i<count;i++)
  {
    //printf("  unmarshaling pair %d\n",i);
    QCString key    = unmarshalQCString(s);
    //printf("  unmarshaling key %s\n",key.data());
    MemberDef *md = (MemberDef *)unmarshalObjPointer(s);
    //printf("  unmarshalMemberSDict i=%d key=%s md=%p\n",i,key.data(),md);
    result->inSort(key,md); // note: this can lead to unmarshalling another object!
  }
  //printf("--- end unmarshalMemberSDict\n");
  return result;
}

DocInfo *unmarshalDocInfo(StorageIntf *s)
{
  uint count = unmarshalUInt(s); 
  if (count==NULL_LIST) return 0;
  DocInfo *result = new DocInfo;
  result->doc  = unmarshalQCString(s);
  result->line = unmarshalInt(s);
  result->file = unmarshalQCString(s);
  return result;
}

BriefInfo *unmarshalBriefInfo(StorageIntf *s)
{
  uint count = unmarshalUInt(s); 
  if (count==NULL_LIST) return 0;
  BriefInfo *result = new BriefInfo;
  result->doc     = unmarshalQCString(s);
  result->tooltip = unmarshalQCString(s);
  result->line    = unmarshalInt(s);
  result->file    = unmarshalQCString(s);
  return result;
}

BodyInfo *unmarshalBodyInfo(StorageIntf *s)
{
  uint count = unmarshalUInt(s); 
  if (count==NULL_LIST) return 0;
  BodyInfo *result = new BodyInfo;
  result->startLine = unmarshalInt(s);
  result->endLine   = unmarshalInt(s);
  result->fileDef   = (FileDef*)unmarshalObjPointer(s);
  return result;
}

GroupList *unmarshalGroupList(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s);
  if (count==NULL_LIST) return 0; // null list
  assert(count<1000000);
  GroupList *result = new GroupList;
  for (i=0;i<count;i++)
  {
    GroupDef *gd = (GroupDef *)unmarshalObjPointer(s);
    result->append(gd);
  }
  return result;
}

MemberList *unmarshalMemberList(StorageIntf *s)
{
  uint count = unmarshalUInt(s); 
  if (count==NULL_LIST) return 0;
  MemberList *ml = new MemberList;
  ml->unmarshal(s);
  return ml;
}

ExampleSDict *unmarshalExampleSDict(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s); 
  if (count==NULL_LIST) return 0;
  ExampleSDict *result = new ExampleSDict;
  assert(count<1000000);
  for (i=0;i<count;i++)
  {
    QCString key = unmarshalQCString(s);
    Example *e = new Example;
    marshalQCString(s,e->anchor);
    marshalQCString(s,e->name);
    marshalQCString(s,e->file);
    result->inSort(key,e);
  }
  return result;
}

SDict<MemberList> *unmarshalMemberLists(StorageIntf *s)
{
  uint i;
  uint count = unmarshalUInt(s); 
  if (count==NULL_LIST) return 0;
  SDict<MemberList> *result = new SDict<MemberList>(7);
  assert(count<1000000);
  for (i=0;i<count;i++)
  {
    QCString key = unmarshalQCString(s);
    MemberList *ml = (MemberList *)unmarshalObjPointer(s);
    result->append(key,ml);
  }
  return result;
}

