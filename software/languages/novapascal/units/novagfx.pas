unit NovaGraphics;

interface

procedure GraphicsOpen; external;
procedure GraphicsClear(Color: Byte); external;
procedure GraphicsColor(Color: Byte); external;
procedure GraphicsFill(X0, Y0, X1, Y1: Byte); external;
procedure GraphicsTile16(Column, Row: Byte); external;
procedure GraphicsTile4x8(Column, Row: Byte); external;
procedure GraphicsWait(Frames: Byte); external;
procedure GraphicsClose; external;

implementation

end.
