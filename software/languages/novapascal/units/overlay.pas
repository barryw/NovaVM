unit Overlay;

interface

const
  OvrOk = 0;
  OvrError = 1;
  OvrIoError = 2;
  OvrBadMagic = 3;
  OvrBadVersion = 4;
  OvrBadLoadAddress = 5;
  OvrTooLarge = 6;
  OvrNotLoaded = 7;
  OvrNoEntry = 8;
  OvrEntryError = 9;

var
  OvrResult: Integer; external;

procedure OvrInitAt(FileName: string; LoadAddress, SlotSize: Word); external;
procedure OvrCallInit; external;
procedure OvrRun; external;
procedure OvrTick; external;
procedure OvrDone; external;

implementation

end.
