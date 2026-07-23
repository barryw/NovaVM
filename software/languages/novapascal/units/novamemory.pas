unit NovaMemory;

interface

type
  { Opaque low-RAM allocation: internal pointer plus logical length. }
  RamBlock = array[0..3] of Byte;
  { Opaque: applications never handle the allocator's 24-bit address. }
  XRamBlock = array[0..4] of Byte;

procedure RamAlloc(var Block: RamBlock; Size: Word); external;
procedure RamFree(var Block: RamBlock); external;
procedure RamRead(var Block: RamBlock; Offset: Word;
                  var Destination; Count: Word); external;
procedure RamWrite(var Block: RamBlock; Offset: Word;
                   var Source; Count: Word); external;
function RamAllocated(Block: RamBlock): Boolean; external;
function RamStatus(): Byte; external;

procedure XRamAlloc(var Block: XRamBlock; Size: Word); external;
procedure XRamFree(var Block: XRamBlock); external;
procedure XRamRead(var Block: XRamBlock; Offset: Word;
                   var Destination; Count: Word); external;
procedure XRamWrite(var Block: XRamBlock; Offset: Word;
                    var Source; Count: Word); external;
function XRamAllocated(Block: XRamBlock): Boolean; external;
function XRamStatus(): Byte; external;

implementation

end.
