unit Strings;

interface

type
  PChar = ^Char;

function StrLen(Str: PChar): Word; external;
function StrEnd(Str: PChar): PChar; external;
function StrCopy(Dest, Source: PChar): PChar; external;
function StrECopy(Dest, Source: PChar): PChar; external;
function StrPCopy(Dest: PChar; Source: string): PChar; external;
function StrPas(Source: PChar): string[255]; external;
function StrCat(Dest, Source: PChar): PChar; external;
function StrComp(Str1, Str2: PChar): Integer; external;
function StrIComp(Str1, Str2: PChar): Integer; external;
function StrLower(Str: PChar): PChar; external;
function StrUpper(Str: PChar): PChar; external;
function StrNew(Str: PChar): PChar; external;
procedure StrDispose(Str: PChar); external;

implementation

end.
