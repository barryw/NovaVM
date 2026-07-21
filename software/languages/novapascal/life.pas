program Life;

uses NovaGraphics, NovaInput, NovaRandom;

var
  Cells, Next: array[0..1999] of Boolean;
  I: Word;
  Row, Col, Neighbors, DrawColor, Key: Byte;

function RandomCell(): Boolean;
begin
  if RandomByte() < 128 then
    RandomCell := True
  else
    RandomCell := False
end;

procedure Seed;
begin
  I := 0;
  Row := 0;
  while Row < 25 do
  begin
    Col := 0;
    while Col < 80 do
    begin
      Cells[I] := False;
      Next[I] := RandomCell();
      I := I + 1;
      Col := Col + 1
    end;
    Row := Row + 1
  end
end;

procedure Draw;
begin
  GraphicsWait(1);
  DrawColor := 255;
  I := 0;
  Row := 0;
  while Row < 25 do
  begin
    Col := 0;
    while Col < 80 do
    begin
      if Cells[I] <> Next[I] then
      begin
        if Next[I] <> DrawColor then
        begin
          DrawColor := Next[I];
          if DrawColor = 1 then
            GraphicsColor(10)
          else
            GraphicsColor(0)
        end;
        GraphicsTile4x8(Col, Row)
      end;
      I := I + 1;
      Col := Col + 1
    end;
    Row := Row + 1
  end
end;

procedure CountNeighbors;
begin
  Neighbors := 0;
  if Row > 0 then
  begin
    if Col > 0 then
      Neighbors := Neighbors + Cells[I - 81];
    Neighbors := Neighbors + Cells[I - 80];
    if Col < 79 then
      Neighbors := Neighbors + Cells[I - 79]
  end;
  if Col > 0 then
    Neighbors := Neighbors + Cells[I - 1];
  if Col < 79 then
    Neighbors := Neighbors + Cells[I + 1];
  if Row < 24 then
  begin
    if Col > 0 then
      Neighbors := Neighbors + Cells[I + 79];
    Neighbors := Neighbors + Cells[I + 80];
    if Col < 79 then
      Neighbors := Neighbors + Cells[I + 81]
  end
end;

function NextCell(): Boolean;
begin
  if Cells[I] then
  begin
    if Neighbors < 2 then
      NextCell := False
    else if Neighbors > 3 then
      NextCell := False
    else
      NextCell := True
  end
  else if Neighbors = 3 then
    NextCell := True
  else
    NextCell := False
end;

procedure Evolve;
begin
  I := 0;
  Row := 0;
  while Row < 25 do
  begin
    Col := 0;
    while Col < 80 do
    begin
      CountNeighbors;
      Next[I] := NextCell();
      I := I + 1;
      Col := Col + 1
    end;
    Row := Row + 1
  end
end;

procedure Commit;
begin
  asm
    ; 2000 bytes = seven full pages and a $D0-byte tail.
    ldx #0
  @pages:
    lda Next,x
    sta Cells,x
    lda Next+$0100,x
    sta Cells+$0100,x
    lda Next+$0200,x
    sta Cells+$0200,x
    lda Next+$0300,x
    sta Cells+$0300,x
    lda Next+$0400,x
    sta Cells+$0400,x
    lda Next+$0500,x
    sta Cells+$0500,x
    lda Next+$0600,x
    sta Cells+$0600,x
    inx
    bne @pages
  @tail:
    lda Next+$0700,x
    sta Cells+$0700,x
    inx
    cpx #$D0
    bne @tail
  end
end;

begin
  Seed;
  GraphicsOpen;
  Draw;
  Commit;
  Key := 0;
  while Key <> 13 do
  begin
    Evolve;
    Draw;
    Commit;
    Key := PollKey()
  end;
  GraphicsClose
end.
