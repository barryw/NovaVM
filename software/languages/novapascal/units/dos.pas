unit Dos;

interface

const
  ReadOnly = $01;
  Hidden = $02;
  SysFile = $04;
  VolumeID = $08;
  Directory = $10;
  Archive = $20;
  AnyFile = $3F;

type
  ComStr = string[127];
  PathStr = string[79];
  DirStr = string[67];
  NameStr = string[8];
  ExtStr = string[4];

  DateTime = record
    Year, Month, Day, Hour, Min, Sec: Word;
  end;

  SearchRec = record
    Fill: array[1..21] of Byte;
    Attr: Byte;
    Time: LongInt;
    Size: LongInt;
    Name: string[12];
  end;

var
  DosError: Integer; external;

function DiskFree(Drive: Byte): LongInt; external;
function DiskSize(Drive: Byte): LongInt; external;
procedure FindFirst(Path: PathStr; Attr: Word; var S: SearchRec); external;
procedure FindNext(var S: SearchRec); external;
procedure GetFAttr(var F; var Attr: Word); external;
procedure SetFAttr(var F; Attr: Word); external;
procedure GetFTime(var F; var Time: LongInt); external;
procedure SetFTime(var F; Time: LongInt); external;
procedure GetDate(var Year, Month, Day, DayOfWeek: Word); external;
procedure SetDate(Year, Month, Day: Word); external;
procedure GetTime(var Hour, Minute, Second, Sec100: Word); external;
procedure SetTime(Hour, Minute, Second, Sec100: Word); external;
procedure PackTime(var DT: DateTime; var Time: LongInt); external;
procedure UnpackTime(Time: LongInt; var DT: DateTime); external;
function FExpand(Path: PathStr): PathStr; external;
procedure FSplit(Path: PathStr; var Dir: DirStr; var Name: NameStr;
                 var Ext: ExtStr); external;

implementation

end.
