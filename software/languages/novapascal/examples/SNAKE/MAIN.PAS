program Snake;

{ Nova text game.

  A complete little arcade game in 80x50 text: Crt for the screen, the NDK
  input adapter for non-blocking keys, and the NDK audio module for
  fire-and-forget SID effects.  Occupancy lives in a 2-D Boolean array and the
  body in a circular buffer, so each step only repaints the head and the tail
  cell.

  Keys: W A S D steer, Esc or Q quits. }

uses Crt, NovaInput, NovaAudio, NovaRandom;

const
  Left = 8;                            { screen column of the left wall }
  Top = 3;                             { screen row of the top wall }
  Width = 62;                          { wall to wall, inclusive }
  Height = 24;
  MaxBody = 250;
  StartLength = 5;
  Wall = 6;                            { Nova palette: blue }
  Fruit = 7;                           { yellow }
  Body = 13;                           { light green }

var
  Cells: array[0..61, 0..23] of Boolean;
  Bx, By: array[0..249] of Integer;
  Head, Len, Speed, Score: Integer;
  Hx, Hy, Dx, Dy, Fx, Fy, Tail: Integer;
  Key, Sample: Byte;
  Alive, Quit: Boolean;

procedure Blip(Note, Frames: Byte);
begin
  AUDIO_NOTE := Note;
  AUDIO_DURATION := Frames;
  AUDIO_SOUND_INSTRUMENT := 0;
  Sample := AUDIO_SOUND_ASYNC()
end;

procedure PutCell(X, Y: Integer; Glyph: Char; Tint: Byte);
begin
  GotoXY(Left + X, Top + Y);
  TextColor(Tint);
  write(Glyph)
end;

procedure Frame;
var
  I: Integer;
begin
  ClrScr;
  TextColor(Wall);
  for I := 0 to Width - 1 do
  begin
    GotoXY(Left + I, Top - 1);
    write('=');
    GotoXY(Left + I, Top + Height);
    write('=')
  end;
  for I := 0 to Height - 1 do
  begin
    GotoXY(Left - 1, Top + I);
    write('|');
    GotoXY(Left + Width, Top + I);
    write('|')
  end;
  TextColor(15);
  GotoXY(Left, Top - 3);
  write('NovaPascal snake -- W A S D to steer, Esc to quit')
end;

procedure ShowScore;
begin
  GotoXY(Left, Top + Height + 2);
  TextColor(15);
  write('Score ');
  write(Score);
  write('  ')
end;

procedure NewFruit;
var
  Free: Boolean;
begin
  Free := false;
  while not Free do
  begin
    Sample := RandomByte();
    Fx := Sample;
    Fx := Fx mod Width;
    Sample := RandomByte();
    Fy := Sample;
    Fy := Fy mod Height;
    Free := not Cells[Fx, Fy]
  end;
  PutCell(Fx, Fy, '*', Fruit)
end;

procedure Grow;
begin
  Head := Head + 1;
  if Head = MaxBody then
    Head := 0;
  Bx[Head] := Hx;
  By[Head] := Hy;
  Cells[Hx, Hy] := true;
  PutCell(Hx, Hy, 'O', Body)
end;

procedure DropTail;
var
  X, Y: Integer;
begin
  Tail := Head - Len;
  while Tail < 0 do
    Tail := Tail + MaxBody;
  X := Bx[Tail];
  Y := By[Tail];
  Cells[X, Y] := false;
  PutCell(X, Y, ' ', 0)
end;

procedure Steer(Nx, Ny: Integer);
begin
  { A snake may not reverse straight into its own neck. }
  if (Nx <> -Dx) or (Ny <> -Dy) then
  begin
    Dx := Nx;
    Dy := Ny
  end
end;

procedure Start;
var
  I: Integer;
begin
  for Hx := 0 to Width - 1 do
    for Hy := 0 to Height - 1 do
      Cells[Hx, Hy] := false;
  Head := 0;
  Len := StartLength;
  Score := 0;
  Speed := 110;
  Dx := 1;
  Dy := 0;
  Hx := Width div 2;
  Hy := Height div 2;
  for I := 0 to StartLength - 1 do
  begin
    Bx[I] := Hx - StartLength + I;
    By[I] := Hy;
    Cells[Hx - StartLength + I, Hy] := true;
    PutCell(Hx - StartLength + I, Hy, 'O', Body)
  end;
  Head := StartLength - 1
end;

begin
  Frame;
  Start;
  NewFruit;
  ShowScore;
  Alive := true;
  Quit := false;
  repeat
    Key := PollKey();
    case Chr(Key) of
      'w', 'W': Steer(0, -1);
      's', 'S': Steer(0, 1);
      'a', 'A': Steer(-1, 0);
      'd', 'D': Steer(1, 0);
      'q', 'Q': Quit := true
    end;
    if Key = 27 then
      Quit := true;

    Hx := Hx + Dx;
    Hy := Hy + Dy;
    if (Hx < 0) or (Hx >= Width) or (Hy < 0) or (Hy >= Height) then
      Alive := false
    else if Cells[Hx, Hy] then
      Alive := false
    else
    begin
      Grow;
      if (Hx = Fx) and (Hy = Fy) then
      begin
        Len := Len + 3;
        Score := Score + 10;
        if Speed > 45 then
          Speed := Speed - 4;
        Blip(84, 3);
        ShowScore;
        NewFruit
      end
      else
        DropTail
    end;
    Delay(Speed)
  until Quit or not Alive;

  if not Alive then
  begin
    Blip(36, 20);
    GotoXY(Left, Top + Height + 3);
    TextColor(Fruit);
    write('Caught! Final score ');
    write(Score)
  end;
  GotoXY(1, Top + Height + 5);
  TextColor(15);
  writeln('Snake finished.')
end.
