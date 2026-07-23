{.PA}
{*******************************************************************}
{*  SOURCE CODE MODULE: MC-MOD05                                   *}
{*  PURPOSE:            Read the contents of a cell and update     *}
{*                      associated cells.                          *}
{*******************************************************************}


{ Procedure GetLine will let the user type and/or edit a string of   }
{ maximum length "MAX". The string will start at cursor position:    }
{ ColNO,LineNO. If ErrPos <> 0 then the cursor will jump to position }
{ ErrPos in the string. If the last parameter is "True" then all     }
{ characters entered will be translated to upper case.               }
{ If the user at anytimes types <ESCAPE> then the string returned    }
{ contain $FF to indicate that editing was aborted.                  }


procedure GetLine(var S: AnyString;           { String to edit       }
                         ColNO,LineNO,        { Where start line     }
                         MAX,                 { Max length           }
                         ErrPos: integer;     { Where to begin       }
                         UpperCase:Boolean);  { True if auto Upcase  }
var
  X: integer;
  InsertOn: boolean;
  OkChars: set of Char;


  procedure GotoX;
  begin
    GotoXY(X+ColNo-1,LineNo);
  end;

begin
  OkChars:=[' '..'}'];
  InsertOn:=true;
  X:=1; GotoX;
  Write(S);
  if Length(S)=1 then X:=2;
  if ErrPos<>0 then X:=ErrPos;
  GotoX;
  repeat
    Read(Kbd,Ch);
    if UpperCase then Ch:=UpCase(Ch);
    case Ch of
       ^[: begin
             S:=chr($FF); { abort editing }
             Ch:=^M;
           end;
       ^D: begin { Move cursor right }
             X:=X+1;
             if (X>length(S)+1) or (X>MAX) then X:=X-1;
             GotoX;
           end;
       ^G: begin { Delete right char }
             if X<=Length(S) then
             begin
               Delete(S,X,1);
               Write(copy(S,X,Length(S)-X+1),' ');
               GotoX;
             end;
           end;
    ^S,^H: begin { Move cursor left }
             X:=X-1;
             if X<1 then X:=1;
             GotoX;
           end;
       ^F: begin { Move cursor to end of line }
              X:=Length(S)+1;
              GotoX;
           end;
       ^A: begin { Move cursor to beginning of line }
             X:=1;
             GotoX;
           end;
     #127: begin { Delete left char }
             X:=X-1;
             if (Length(S)>0) and (X>0)  then
             begin
               Delete(S,X,1);
               Write(copy(S,X,Length(S)-X+1),' ');
               GotoX;
               if X<1 then X:=1;
             end else X:=1;
           end;
       ^V: InsertOn:= not InsertOn;

{.PA}

    else
      begin
        if Ch in OkChars  then
        begin
          if InsertOn then
          begin
            insert(Ch,S,X);
            Write(copy(S,X,Length(S)-X+1),' ');
          end else
          begin
            write(Ch);
            if X=length(S) then S:=S+Ch
              else S[X]:=Ch;
          end;
          if Length(S)+1<=MAX then X:=X+1
          else OkChars:=[]; { Line too Long }
          GotoX;
        end else
        if Length(S)+1<=Max then
          OkChars:= [' '..'}']; { Line ok again }
      end;
    end;
  until CH=^M;
end;


{.PA}


procedure  GetCell(FX: ScreenIndex;FY: Integer);
var
  S:             AnyString;
  NewStat:       Set of Attributes;
  ErrorPosition: Integer;
  I:             ScreenIndex;
  Result:        Real;
  Abort:         Boolean;
  IsForm:        Boolean;
  Cell:          CellRec;

{ Procedure ClearCells clears the current cell and its associated    }
{ cells. An associated cell is a cell overwritten by data from the   }
{ current cell. The data can be text in which case the cell has the  }
{ attribute "OverWritten". If the data is a result from an expression}
{ and the field with is larger tahn 11 then the cell is "Locked"     }

  procedure ClearCells;
  var Done: Boolean;
  begin
    I:=FX; Done:=false;
    repeat
      LoadCell(I,FY,Cell);
      GotoXY(XPos[I],FY+1);
      write('           ');
      if I=FXMax then Done:=true
      else
      begin
        I:=Succ(I);
        LoadCell(I,FY,Cell);
        if [OverWritten,Locked]*Cell.CellStatus=[] then Done:=true;
      end;
    until Done;
    { Cell is not OVerWritten not Locked }
  end;

{.CP20}
{ The new type of the cell is flashed at the bottom of the screen    }
{ Notice that a constant of type array is used to indicate the type  }

  procedure FlashType;
  begin
    HighVideo;
    GotoXY(5,23);
    LowVideo;
  end;

{.CP20}
{ Procedure GetFormula repeats calling the procedure GetLine and     }
{ Evaluate until the line read by GetLine contains a valid formula.  }
{ Evaluate returns an error position in the string evaluated. If     }
{ this position is non zero GetLine is called. If the user types     }
{ ESC in GetLine to abort the editing then the string returned from  }
{ Getline will contain $FF and te original value of the cell will    }
{ be restored later.                                                 }

  procedure GetFormula;
  begin
    FlashType;
    repeat
      GetLine(S,1,24,70,ErrorPosition,True);
      if S<>Chr($FF) then
      begin
        Evaluate(IsForm,S,Result,ErrorPosition);
        if ErrorPosition<>0 then
          Flash(15,'Error at cursor'+^G,false)
        else Flash(15,'               ',false);
      end;
    until (ErrorPosition=0) or (S=Chr($FF));
    if IsForm then NewStat:=NewStat+[Formula];
  end;

{.CP20}
{ Procedure GetText calls the procedure GetLine with the current     }
{ cells X,Y position as parameters. This means that text entering    }
{ takes place direcly at the cells posion on the screen.             }

  procedure GetText;
  begin
    FlashType;
    GetLine(S,XPos[FX],FY+1,70,ErrorPosition,False);
  end;

{.CP20}
{ Procedure EditCell loads a copy of the current cells contents in   }
{ in the variable S before calling either GetText or GetFormula. In  }
{ this way no changes are made to the current cell.                  }

  procedure EditCell;
  begin
    LoadCell(FX,FY,Cell);
    with Cell do
    begin
      S:=Contents;
      if Txt in CellStatus then GetText else GetFormula;
    end;
  end;

{.PA}
{ Procedure UpdateCells is a little more complicated. Basically it   }
{ makes sure to tag and untag cells which has been overwritten or    }
{ cleared from data from  another cell. It also updates the current  }
{ with the new type and the contents which still is in the temporaly }
{ variable "S".                                                      }


  procedure UpdateCells;
  var
    Flength: Integer;
    Done: Boolean;
    Redraw: Boolean;
  begin
    LoadCell(FX,FY,Cell);
    Cell.Contents:=S;
    StoreCell(FX,FY,Cell);
    if Txt in NewStat then
    begin
      I:=FX; FLength:=Length(S); Done:=false;
      while (I<FXMax) and not Done do
      begin
        I:=Succ(I);
        LoadCell(I,FY,Cell);
        Redraw:=false;
        with Cell do
        begin
          FLength:=Flength-11;
          if (Flength>0) then
          begin
            CellStatus:=[Overwritten,Txt];
            Contents:='';
          end else
          begin
            if OverWritten in CellStatus then
            begin
              CellStatus:=[Txt];
              Redraw:=true;
            end;
          end;
        end;
        StoreCell(I,FY,Cell);
        if Redraw then begin GotoCell(I,FY); LeaveCell(I,FY); end;
        if Cell.Contents<>'' then Done:=true;
      end;
      LoadCell(FX,FY,Cell);
      Cell.CellStatus:=[Txt];
      StoreCell(FX,FY,Cell);
    end else { string changed to formula or constant }
    begin { Event number two }
      I:=FX; Done:=false;
      repeat
        LoadCell(I,FY,Cell);
        with Cell do
        begin
          if OverWritten in CellStatus then
          begin
            CellStatus:=[Txt];
            Contents:='';
          end;
        end;
        StoreCell(I,FY,Cell);
        if I=FXMax then Done:=true
        else
        begin
          I:=Succ(I);
          LoadCell(I,FY,Cell);
          if not (OverWritten in Cell.CellStatus) then Done:=true;
        end;
      until Done;
      LoadCell(FX,FY,Cell);
      with Cell do
      begin
        CellStatus:=[Constant];
        if IsForm then CellStatus:=CellStatus+[Formula];
        Value:=Result;
      end;
      StoreCell(FX,FY,Cell);
    end;
  end;


{.PA}
{ Procedure GetCell finnaly starts here. This procedure uses all     }
{ all the above local procedures. First it initializes the temporaly }
{ variable "S" with the last read character. It then depending on    }
{ this character calls GetFormula, GetText, or EditCell.             }

begin { procedure GetCell }
  S:=Ch; ErrorPosition:=0; Abort:=false;
  NewStat:=[];
  LoadCell(FX,FY,Cell);
  if Ch in ['0'..'9','+','-','.','(',')'] then
  begin
    NewStat:=[Constant];
    if not (Formula in Cell.CellStatus) then
    begin
      GotoXY(11,24); ClrEol;
      ClearCells;
      GetFormula;
    end else
    begin
      Flash(15,'Edit formula Y/N?',true);
      repeat read(Kbd,Ch) until UpCase(CH) in ['Y','N'];
      Flash(15,'                 ',false);
      if UpCase(Ch)='Y' then EditCell Else Abort:=true;
    end;
  end else
  begin
    if Ch=^[ then
    begin
      NewStat:=(Cell.CellStatus)*[Txt,Constant];
      EditCell;
    end else
    begin
      if formula in Cell.CellStatus then
      begin
        Flash(15,'Edit formula Y/N?',true);
        repeat read(Kbd,Ch) until UpCase(CH) in ['Y','N'];
        Flash(15,'                 ',false);
        if UpCase(Ch)='Y' then EditCell Else Abort:=true;
      end else
      begin
        NewStat:=[Txt];
        ClearCells;
        GetText;
      end;
    end;
  end;
  if not Abort then
  begin
    if S<>Chr($FF) then UpDateCells;
    GotoCell(FX,FY);
    LoadCell(FX,FY,Cell);
    if AutoCalc and (Constant in Cell.CellStatus) then Recalculate;
    if Txt in NewStat then
    begin
      GotoXY(3,FY+1); Clreol;
      For I:='A' to FXMax do
      LeaveCell(I,FY);
    end;
  end;
  Flash(15,'                ',False);
  GotoCell(FX,FY);
end;

{.PA}
{ Procedure Format is used to }


procedure Format;
var
  J,FW,DEC,
  FromLine,ToLine: integer;
  Lock:            Boolean;
  Cell:            CellRec;


  procedure GetInt(var I: integer; Max: Integer);
  var
    S: string[8];
    Err: Integer;
    Ch: Char;
  begin
    S:='';
    repeat
      repeat Read(Kbd,Ch) until Ch in ['0'..'9','-',^M];
      if Ch<>^M then
      begin
        Write(Ch); S:=S+Ch;
        Val(S,I,Err);
      end;
    until (I>=Max) or (Ch=^M);
    if I>Max then I:=Max;
  end;

begin
  HighVideo;
  Msg('Format: Enter number of decimals (Max 11):  ');
  GetInt(DEC,11);
  Msg('Enter Cell whith remember if larger than 10 next column will lock: ');
  GetInt(FW,20);
  Msg('From which line in column '+FX+': ');
  GetInt(FromLine,FYMax);
  Msg('To which line in column '+FX+': ');
  GetInt(ToLine,FYMax);
  if FW>10 then Lock:=true else Lock:=False;
  for J:=FromLine to ToLine do
  begin
    LoadCell(FX,J,Cell);
    Cell.DEC:=DEC;
    Cell.FW:=FW;
    StoreCell(FX,J,Cell);
    if FX<FXMax then
    begin
      LoadCell(Succ(FX),J,Cell);
      with Cell do
      begin
        if Lock then
        begin
          CellStatus:=CellStatus+[Locked,Txt];
          Contents:='';
        end else CellStatus:=CellStatus-[Locked];
      end;
      StoreCell(Succ(FX),J,Cell);
    end;
  end;
  NormVideo;
  UpDate;
  GotoCell(FX,FY);
end;

Video;
  UpDate;
  GotoCell(FX,FY);
end;


