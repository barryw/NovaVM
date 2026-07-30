program Starburst;

{ Nova graphics demo.

  Two line fans are drawn across the whole 320x200 plane and redrawn every
  frame with a rotating colour cycle.  Every line goes through the VGC's
  hardware line primitive, so the 65C02 only computes endpoints.

  Press any key to return to the shell. }

uses Graph, NovaInput;

const
  Columns = 40;
  Rows = 25;
  Cycle = 15;

var
  Driver, Mode, Phase: Integer;
  Key: Byte;

function Tint(Index, Offset: Integer): Word;
begin
  Tint := 1 + (Index + Offset) mod Cycle
end;

procedure Fan(Offset: Integer);
var
  I: Integer;
begin
  for I := 0 to Columns - 1 do
  begin
    SetColor(Tint(I, Offset));
    Line(I * 8, 0, 319 - I * 8, 199)
  end;
  for I := 0 to Rows - 1 do
  begin
    SetColor(Tint(I, Offset));
    Line(0, I * 8, 319, 199 - I * 8)
  end
end;

begin
  Driver := Detect;
  Mode := Nova320x200;
  InitGraph(Driver, Mode, '');
  SetBkColor(Black);
  ClearDevice;
  Phase := 0;
  repeat
    Fan(Phase);
    Phase := Phase + 1;
    if Phase = Cycle then
      Phase := 0;
    Key := PollKey()
  until Key <> 0;
  CloseGraph;
  writeln('Starburst finished.')
end.
