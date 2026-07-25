unit Graph;

interface

const
  Detect = 0;
  VGA = 9;
  NovaGraph = 9;
  Nova320x200 = 0;
  grOk = 0;

  Black = 0;
  Blue = 1;
  Green = 2;
  Cyan = 3;
  Red = 4;
  Magenta = 5;
  Brown = 6;
  LightGray = 7;
  DarkGray = 8;
  LightBlue = 9;
  LightGreen = 10;
  LightCyan = 11;
  LightRed = 12;
  LightMagenta = 13;
  Yellow = 14;
  White = 15;

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
