{.PA}
{*******************************************************************}
{*  SOURCE CODE MODULE: MC-MOD02                                   *}
{*  PURPOSE:            Display values in cells and move between   *}
{*                      cells in the spread sheet.                 *}
{*******************************************************************}


procedure FlashType;
var Cell: CellRec;
begin
  LoadCell(FX,FY,Cell);
  with Cell do
  begin
    GotoXY(1,23);
    Write(FX,FY:2,' ');
    if Formula in CellStatus  then write('Formula:')  else
    if Constant in CellStatus then Write('Numeric ') else
    if Txt in CellStatus then Write('Text    ');
    GotoXY(1,24); ClrEol;
    if Formula in CellStatus then Write(Contents);
  end;
end;


{ The following procedures move between the Cells on the calc sheet.}
{ Each Cell has an associated record containing its X,Y coordinates }
{ and data. See the type definition for "Cell".                     }

procedure ClearCellField(X,Y: integer);
begin
  GotoXY(X,Y);
  Write('           ');
  asm
    .include "NOVA.INC"
    lda VGC_CURSX
    sec
    sbc #11
    sta VGC_CURSX
  end
end;

procedure GotoCell(GX: ScreenIndex; GY: integer);
var Cell: CellRec;
begin
  LoadCell(GX,GY,Cell);
  with Cell do
  begin
    HighVideo;
    ClearCellField(XPos[GX],GY+1);
    if Txt in CellStatus then Write(Contents)
    else
    begin
      if DEC>=0 then Write(Value:FW:DEC)
      else Write(Value:FW);
    end;
    FlashType;
    GotoXY(XPos[GX],GY+1);
  end;
  LowVideo;
end;

{.CP20}

procedure LeaveCell(FX:ScreenIndex;FY: integer);
var Cell: CellRec;
begin
  LoadCell(FX,FY,Cell);
  with Cell do
  begin
    LowVideo;
    ClearCellField(XPos[FX],FY+1);
    if Txt in CellStatus then Write(Contents)
    else
    begin
      if DEC>=0 then Write(Value:FW:DEC)
      else Write(Value:FW);
    end;
  end;
end;


{.CP20}

procedure Update;
var
  UFX: ScreenIndex;
  UFY: integer;
  Cell: CellRec;
begin
  ClrScr;
  Grid;
  for UFX:='A' to FXMax do for UFY:=1 to FYMax do
  begin
    LoadCell(UFX,UFY,Cell);
    if Cell.Contents<>'' then LeaveCell(UFX,UFY);
  end;
  GotoCell(FX,FY);
end;

{.CP20}

procedure MoveDown;
var Start: integer; Cell: CellRec;
begin
  LeaveCell(FX,FY);
  Start:=FY;
  repeat
    FY:=FY+1;
    if FY>FYMax then FY:=1;
    LoadCell(FX,FY,Cell);
  until (Cell.CellStatus*[OverWritten,Locked]=[]) or (FY=Start);
  if FY<>Start then GotoCell(FX,FY);
end;

{.CP20}

procedure MoveUp;
var Start: integer; Cell: CellRec;
begin
  LeaveCell(FX,FY);
  Start:=FY;
  repeat
    FY:=FY-1;
    if FY<1 then FY:=FYMax;
    LoadCell(FX,FY,Cell);
  until (Cell.CellStatus*[OverWritten,Locked]=[]) or  (FY=Start);
  if FY<>Start then GotoCell(FX,FY);
end;

{.CP20}

procedure MoveRight;
var Start: ScreenIndex; Cell: CellRec;
begin
  LeaveCell(FX,FY);
  Start:=FX;
  repeat
    FX:=Succ(FX);
    if FX>FXMax then
    begin
      FX:='A';
      FY:=FY+1;
      if FY>FYMax then FY:=1;
    end;
    LoadCell(FX,FY,Cell);
  until (Cell.CellStatus*[OverWritten,Locked]=[]) or (FX=Start);
  if FX<>Start then GotoCell(FX,FY);
end;

{.CP20}

procedure MoveLeft;
var Start: ScreenIndex; Cell: CellRec;
begin
  LeaveCell(FX,FY);
  Start:=FX;
  repeat
    FX:=Pred(FX);
    if FX<'A' then
    begin
      FX:=FXMax;
      FY:=FY-1;
      if FY<1 then FY:=FYMax;
    end;
    LoadCell(FX,FY,Cell);
  until (Cell.CellStatus*[OverWritten,Locked]=[]) or (FX=Start);
  if FX<>Start then GotoCell(FX,FY);
end;

ellStatus*[OverWritten,Locked]=[]) or (FX=Start);
  if FX<>Start then GotoCell(FX,FY);
end;


