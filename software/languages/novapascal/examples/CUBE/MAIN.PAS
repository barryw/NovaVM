program Cube;

{ Nova graphics demo: a rotating wireframe cube.

  Real trigonometry runs once at startup to build a 64-entry sine table
  scaled by 256; the animation loop is then pure 16-bit integer maths, and
  the VGC draws every edge.  The previous frame is erased edge by edge, so
  the demo never clears the whole plane.

  Press any key to return to the shell. }

uses Graph, NovaInput, NovaVgc;

const
  Steps = 64;
  Quarter = 16;
  Scale = 256;
  Size = 45;
  Distance = 240;
  Zoom = 240;
  { Nova's default palette is C64-ordered: 3 is cyan, 0 is black. }
  EdgeColour = 3;

var
  Driver, Mode, AngleA, AngleB: Integer;
  SinTab: array[0..63] of Integer;
  Vx, Vy, Vz: array[0..7] of Integer;
  Px, Py, Ox, Oy: array[0..7] of Integer;
  Ea, Eb: array[0..11] of Integer;
  R: Real;
  I: Integer;
  Key: Byte;

procedure Corner(Index, X, Y, Z: Integer);
begin
  Vx[Index] := X;
  Vy[Index] := Y;
  Vz[Index] := Z
end;

procedure Edge(Index, From, Upto: Integer);
begin
  Ea[Index] := From;
  Eb[Index] := Upto
end;

function Sine(Angle: Integer): Integer;
begin
  Sine := SinTab[Angle mod Steps]
end;

procedure Project(A, B: Integer);
var
  Sa, Ca, Sb, Cb, X1, Z1, Y2, Z2, Depth, J: Integer;
begin
  Sa := Sine(A);
  Ca := Sine(A + Quarter);
  Sb := Sine(B);
  Cb := Sine(B + Quarter);
  for J := 0 to 7 do
  begin
    X1 := (Vx[J] * Ca - Vz[J] * Sa) div Scale;
    Z1 := (Vx[J] * Sa + Vz[J] * Ca) div Scale;
    Y2 := (Vy[J] * Cb - Z1 * Sb) div Scale;
    Z2 := (Vy[J] * Sb + Z1 * Cb) div Scale;
    Depth := Z2 + Distance;
    Ox[J] := Px[J];
    Oy[J] := Py[J];
    Px[J] := 160 + (X1 * Zoom) div Depth;
    Py[J] := 100 + (Y2 * Zoom) div Depth
  end
end;

procedure DrawFrame(Tint: Word; Previous: Boolean);
var
  J, A, B: Integer;
begin
  SetColor(Tint);
  for J := 0 to 11 do
  begin
    A := Ea[J];
    B := Eb[J];
    if Previous then
      Line(Ox[A], Oy[A], Ox[B], Oy[B])
    else
      Line(Px[A], Py[A], Px[B], Py[B])
  end
end;

begin
  for I := 0 to Steps - 1 do
  begin
    { Real is Q16.16, so keep every constant inside +/-32767: take the
      fraction of a turn first, then scale it by 2*Pi. }
    R := I / Steps;
    R := Sin(R * 6283 / 1000);
    SinTab[I] := Trunc(R * Scale)
  end;

  Corner(0, -Size, -Size, -Size);
  Corner(1, Size, -Size, -Size);
  Corner(2, Size, Size, -Size);
  Corner(3, -Size, Size, -Size);
  Corner(4, -Size, -Size, Size);
  Corner(5, Size, -Size, Size);
  Corner(6, Size, Size, Size);
  Corner(7, -Size, Size, Size);

  Edge(0, 0, 1); Edge(1, 1, 2); Edge(2, 2, 3); Edge(3, 3, 0);
  Edge(4, 4, 5); Edge(5, 5, 6); Edge(6, 6, 7); Edge(7, 7, 4);
  Edge(8, 0, 4); Edge(9, 1, 5); Edge(10, 2, 6); Edge(11, 3, 7);

  Driver := Detect;
  Mode := Nova320x200;
  InitGraph(Driver, Mode, '');
  SetBkColor(Black);
  ClearDevice;

  AngleA := 0;
  AngleB := 0;
  Project(AngleA, AngleB);
  repeat
    VGC_WAIT_FRAMES(1);
    DrawFrame(Black, true);
    DrawFrame(EdgeColour, false);
    AngleA := (AngleA + 1) mod Steps;
    AngleB := (AngleB + 3) mod Steps;
    Project(AngleA, AngleB);
    Key := PollKey()
  until Key <> 0;
  CloseGraph;
  writeln('Cube finished.')
end.
