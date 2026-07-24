unit Crt;

interface

const
  { Turbo color names mapped to Nova's default palette. }
  Black = 0;
  Blue = 6;
  Green = 5;
  Cyan = 3;
  Red = 2;
  Magenta = 4;
  Brown = 9;
  LightGray = 15;
  DarkGray = 11;
  LightBlue = 14;
  LightGreen = 13;
  LightCyan = 3;
  LightRed = 10;
  LightMagenta = 4;
  Yellow = 7;
  White = 1;

procedure ClrScr; external;
procedure ClrEol; external;
procedure DelLine; external;
procedure InsLine; external;
procedure GotoXY(X, Y: Byte); external;
procedure Window(X1, Y1, X2, Y2: Byte); external;
procedure HighVideo; external;
procedure LowVideo; external;
procedure NormVideo; external;
procedure Delay(Milliseconds: Word); external;
function KeyPressed: Boolean; external;
function ReadKey: Char; external;
function WhereX: Byte; external;
function WhereY: Byte; external;
procedure TextColor(Color: Byte); external;
procedure TextBackground(Color: Byte); external;

implementation

end.
