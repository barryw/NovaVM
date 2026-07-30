unit Graph;

interface

const
  Detect = 0;
  VGA = 9;
  NovaGraph = 9;
  Nova320x200 = 0;
  grOk = 0;

  { Turbo colour names mapped to Nova's default palette, exactly as Crt
    maps them, so a name means the same colour in text and graphics. }
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

procedure DetectGraph(var GraphDriver, GraphMode: Integer); external;
procedure InitGraph(var GraphDriver, GraphMode: Integer;
                    PathToDriver: string); external;
procedure CloseGraph; external;
function GraphResult: Integer; external;

function GetMaxX: Integer; external;
function GetMaxY: Integer; external;
function GetMaxColor: Word; external;

procedure ClearDevice; external;
procedure SetColor(Color: Word); external;
function GetColor: Word; external;
procedure SetBkColor(Color: Word); external;
function GetBkColor: Word; external;
procedure PutPixel(X, Y: Integer; Pixel: Word); external;
function GetPixel(X, Y: Integer): Word; external;

procedure MoveTo(X, Y: Integer); external;
procedure MoveRel(Dx, Dy: Integer); external;
function GetX: Integer; external;
function GetY: Integer; external;
procedure LineTo(X, Y: Integer); external;
procedure LineRel(Dx, Dy: Integer); external;
procedure Line(X1, Y1, X2, Y2: Integer); external;
procedure Rectangle(X1, Y1, X2, Y2: Integer); external;
procedure Bar(X1, Y1, X2, Y2: Integer); external;
procedure Circle(X, Y: Integer; Radius: Word); external;
procedure FloodFill(X, Y: Integer; Border: Word); external;

implementation

end.
