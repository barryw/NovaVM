program Breakout;

{ Nova graphics game.

  Bricks, paddle, and ball are VGC bars, the frame is paced by the canonical
  vertical-blank wait, keys arrive through the NDK input adapter, and every
  bounce fires an asynchronous SID effect.  Only the cells that actually
  change are repainted, so a 1 MHz 65C02 keeps a steady 60 Hz frame.

  Keys: A and D (or , and .) move the paddle, Esc quits. }

uses Graph, NovaInput, NovaAudio, NovaVgc;

const
  Columns = 10;
  Rows = 5;
  BrickLeft = 10;
  BrickTop = 24;
  BrickWidth = 30;
  BrickHeight = 10;
  PaddleY = 186;
  PaddleWidth = 40;
  BallSize = 4;
  StartLives = 3;
  { Nova's default palette is C64-ordered. }
  Ink = 1;
  Bat = 14;
  Ball = 7;

var
  Driver, Mode: Integer;
  Bricks: array[0..9, 0..4] of Boolean;
  Standing, Score, Lives: Integer;
  Px, Ox, Bx, By, Vx, Vy, Ax, Ay: Integer;
  Key, Sample: Byte;
  Playing: Boolean;

procedure Blip(Note, Frames: Byte);
begin
  AUDIO_NOTE := Note;
  AUDIO_DURATION := Frames;
  AUDIO_SOUND_INSTRUMENT := 0;
  Sample := AUDIO_SOUND_ASYNC()
end;

procedure Block(X, Y, W, H: Integer; Tint: Word);
begin
  SetColor(Tint);
  Bar(X, Y, X + W - 1, Y + H - 1)
end;

function RowTint(Row: Integer): Word;
begin
  case Row of
    0: RowTint := 2;                   { red }
    1: RowTint := 8;                   { orange }
    2: RowTint := 7;                   { yellow }
    3: RowTint := 5;                   { green }
  else
    RowTint := 14                      { light blue }
  end
end;

procedure BuildWall;
var
  Col, Row: Integer;
begin
  Standing := 0;
  for Row := 0 to Rows - 1 do
    for Col := 0 to Columns - 1 do
    begin
      Bricks[Col, Row] := true;
      Standing := Standing + 1;
      Block(BrickLeft + Col * BrickWidth, BrickTop + Row * BrickHeight,
            BrickWidth - 2, BrickHeight - 2, RowTint(Row))
    end
end;

procedure ShowLives;
var
  I: Integer;
begin
  Block(0, 194, 60, 6, 0);
  for I := 0 to Lives - 1 do
    Block(4 + I * 10, 194, 6, 6, Ink)
end;

procedure ServeBall;
begin
  Bx := 158;
  By := 120;
  Vx := 2;
  Vy := -2;
  Ax := Bx;
  Ay := By
end;

procedure MovePaddle;
begin
  Ox := Px;
  case Chr(Key) of
    'a', 'A', ',': Px := Px - 8;
    'd', 'D', '.': Px := Px + 8
  end;
  if Px < 0 then
    Px := 0;
  if Px > 319 - PaddleWidth then
    Px := 319 - PaddleWidth;
  if Px <> Ox then
  begin
    Block(Ox, PaddleY, PaddleWidth, 4, 0);
    Block(Px, PaddleY, PaddleWidth, 4, Bat)
  end
end;

procedure HitBrick;
var
  Col, Row: Integer;
begin
  if (Bx < BrickLeft) or (By < BrickTop) then
    Exit;
  Col := (Bx - BrickLeft) div BrickWidth;
  Row := (By - BrickTop) div BrickHeight;
  if (Col < 0) or (Col >= Columns) or (Row < 0) or (Row >= Rows) then
    Exit;
  if not Bricks[Col, Row] then
    Exit;
  Bricks[Col, Row] := false;
  Standing := Standing - 1;
  Score := Score + 10;
  Block(BrickLeft + Col * BrickWidth, BrickTop + Row * BrickHeight,
        BrickWidth - 2, BrickHeight - 2, 0);
  Vy := -Vy;
  Blip(84 - Row * 3, 3)                { the top row rings highest }
end;

procedure StepBall;
begin
  Ax := Bx;
  Ay := By;
  Bx := Bx + Vx;
  By := By + Vy;
  if Bx < 2 then
  begin
    Bx := 2;
    Vx := -Vx;
    Blip(60, 2)
  end;
  if Bx > 315 - BallSize then
  begin
    Bx := 315 - BallSize;
    Vx := -Vx;
    Blip(60, 2)
  end;
  if By < 2 then
  begin
    By := 2;
    Vy := -Vy;
    Blip(60, 2)
  end;
  HitBrick;
  if (Vy > 0) and (By + BallSize >= PaddleY) and (By < PaddleY + 4) and
     (Bx + BallSize >= Px) and (Bx <= Px + PaddleWidth) then
  begin
    By := PaddleY - BallSize;
    Vy := -Vy;
    if Bx + BallSize div 2 < Px + PaddleWidth div 3 then
      Vx := -2
    else if Bx + BallSize div 2 > Px + 2 * PaddleWidth div 3 then
      Vx := 2;
    Blip(67, 2)
  end;
  Block(Ax, Ay, BallSize, BallSize, 0);
  Block(Bx, By, BallSize, BallSize, Ball)
end;

begin
  Driver := Detect;
  Mode := Nova320x200;
  InitGraph(Driver, Mode, '');
  SetBkColor(Black);
  ClearDevice;

  Score := 0;
  Lives := StartLives;
  Px := 140;
  Ox := Px;
  BuildWall;
  ShowLives;
  Block(Px, PaddleY, PaddleWidth, 4, Bat);
  ServeBall;
  Playing := true;

  repeat
    VGC_WAIT_FRAMES(1);
    Key := PollKey();
    if Key = 27 then
      Playing := false;
    MovePaddle;
    StepBall;
    if By > PaddleY + 8 then
    begin
      Blip(30, 15);
      Lives := Lives - 1;
      ShowLives;
      Block(Bx, By, BallSize, BallSize, 0);
      if Lives = 0 then
        Playing := false
      else
      begin
        VGC_WAIT_FRAMES(40);
        ServeBall
      end
    end;
    if Standing = 0 then
      Playing := false
  until not Playing;

  CloseGraph;
  write('Breakout finished. Score ');
  writeln(Score);
  if Standing = 0 then
    writeln('Wall cleared!')
  else if Lives = 0 then
    writeln('Out of balls.')
end.
