program MicroCalc;
{
    MICROCALC DEMONSTRATION PROGRAM  Version 1.00A

  This program is Copyrighted by Borland International, Inc.
  1983, 1984, 1985 and is hereby donated to the public domain for
  non-commercial use only.  Dot commands are for the program
  lister:   LISTT.PAS  (available with  our TURBO TUTOR):

      .PA, .CP20, etc...

  INSTRUCTIONS
    1.  Compile this program using the TURBO.COM compiler.
        a.  Use the O command from the main menu to select Options.
        b.  Select the C option to generate a .COM file.
        c.  Select the Q option to Quit the Options menu.
        d.  Select the M option to specify the Main file
        e.  Type "MC" and hit <RETURN>
        f.  Type C to compile the program to disk
        g.  Type R to run the program

    2.  Exit the program by typing: /Q
}

{$R-,U-,V-,X-,A+,C-}

uses NovaMemory;


const
  FXMax: Char  = 'G';  { Maximum number of columns                   }
  FYMax        = 21;   { Maximum number of lines                     }
  CellBytes: Word = 78;
  ScreenBytes: Word = 11466;

type
  Anystring   = string[255];
  ScreenIndex = 'A'..'G';
  Attributes  = (Constant,Formula,Txt,OverWritten,Locked,Calculated);

{ The spreadsheet is made out of Cells every Cell is defined as      }
{ the following record:                                              }

  CellRec    = record
    CellStatus: set of Attributes; { Status of cell (see type def.)  }
    Contents:   String[70];        { Contains a formula or some text }
    Value:      Real;              { Last calculated cell value      }
    DEC,FW:     0..20;             { Decimals and Cell Whith         }
  end;

  Cells      =  array[ScreenIndex,1..FYMax] of CellRec;
  XRamBlock  = array[0..4] of Byte;

const
  XPOS: array[ScreenIndex] of integer = (3,14,25,36,47,58,68);

var
  ScreenData:    XRamBlock;         { Worksheet lives in managed XRAM }
  FX:            ScreenIndex;       { Culumn of current cell         }
  FY:            Integer;           { Line of current cell           }
  Ch:            Char;              { Last read character            }
  MCFile:        file of CellRec;   { File to store sheets in        }
  AutoCalc:      boolean;           { Recalculate after each entry?  }

procedure GetCellOffset(X: ScreenIndex; Y: Integer; var Offset: Word);
var Column: Word;
begin
  case X of
    'A': Column:=0;
    'B': Column:=1;
    'C': Column:=2;
    'D': Column:=3;
    'E': Column:=4;
    'F': Column:=5;
    'G': Column:=6;
  end;
  Offset:=(Column*FYMax+Y-1)*CellBytes;
end;

procedure MemoryFault;
begin
  if XRamAllocated(ScreenData) then XRamFree(ScreenData);
  Halt;
end;

procedure LoadCell(X: ScreenIndex; Y: Integer; var Cell: CellRec);
var Offset: Word;
begin
  GetCellOffset(X,Y,Offset);
  XRamRead(ScreenData,Offset,Cell,CellBytes);
  if XRamStatus()<>0 then MemoryFault;
end;

procedure StoreCell(X: ScreenIndex; Y: Integer; var Cell: CellRec);
var Offset: Word;
begin
  GetCellOffset(X,Y,Offset);
  XRamWrite(ScreenData,Offset,Cell,CellBytes);
  if XRamStatus()<>0 then MemoryFault;
end;


{ The following include files contain procedures used in MicroCalc.  }
{ In the following source code there is a reference after each       }
{ procedure call indicating in which module the procedure is located.}

{ If you want a printer listing of the following modules then you    }
{ must let the include directives start in column one and then use   }
{ the TLIST program to generate a listing.                           }

 {$I MC-MOD00.INC        Miscelaneous procedures                     }
 {$I MC-MOD01.INC        Initialization procedures                   }
 {$I MC-MOD02.INC        Commands to move between fields             }
 {$I MC-MOD03.INC        Commands to Load,Save,Print                 }
 {$I MC-MOD04.INC        Evaluating an expression in a cell          }
 {$I MC-MOD05.INC        Reading a cell definition and Format command}


{.PA}
{*********************************************************************}
{*                START OF MAIN PROGRAM PROCEDURES                   *}
{*********************************************************************}


{ Procedure Commands is activated from the main loop in this program }
{ when the user type a semicolon. Commands then activates a procedure}
{ which will execute the command. These procedures are located in the}
{ above modules.                                                     }
{ For easy reference the source code module number is shown in a     }
{ comment on the right following the procedure call.                 }

procedure Commands;
begin
  GotoXY(1,24);
  HighVideo;
  Write('/ restore, Quit, Load, Save, Recalculate, Print,  Format, AutoCalc, Help ');
  Read(Kbd,Ch);
  Ch:=UpCase(Ch);
  case Ch of                                             { In module }
    'Q': begin XRamFree(ScreenData); Halt; end;
    'F': Format;                                               {  04 }
    'S': Save;                                                 {  03 }
    'L': Load;                                                 {  03 }
    'H': Help;                                                 {  03 }
    'R': Recalculate;                                          {  05 }
    'A': Auto;                                                 {  00 }
    '/': Update;                                               {  01 }
    'C': Clear;                                                {  01 }
    'P': Print;                                                {  03 }
  end;
  Grid;                                                        {  01 }
  GotoCell(FX,FY);                                             {  02 }
end;

{ Procedure Hello says hello and activates the help procedure if the }
{ user presses anything but Return                                   }

procedure Wellcome;

  procedure Center(S: AnyString);
  var I: integer;
  begin
    for I:=1 to (80-Length(S)) div 2 do Write(' ');
    writeln(S);
  end;

begin { procedure Wellcome }
  ClrScr; GotoXY(1,9);
  Center('Welcome to MicroCalc.  A Turbo demonstation program');
  Center('Copyright 1983 by Borland International Inc. ');
  Center('Press any key for help or <RETURN> to start');
  GotoXY(40,12);
  Read(Kbd,Ch);
  if Ch<>^M then Help;
end;

{.PA}
{*********************************************************************}
{*          THIS IS WHERE THE PROGRAM STARTS EXECUTING               *}
{*********************************************************************}

begin
  XRamAlloc(ScreenData,ScreenBytes);
  if XRamStatus()<>0 then Halt;
  Init;                                                        {  01 }
  Wellcome;
  ClrScr; Grid;                                                {  01 }
  GotoCell(FX,FY);
  repeat
    Read(Kbd,Ch);
    case Ch of
      ^E:       MoveUp;                                        {  02 }
      ^X,^J:    MoveDown;                                      {  02 }
      ^D,^M,^F: MoveRight;                                     {  02 }
      ^S,^A:    MoveLeft;                                      {  02 }
      '/':      Commands;
      ^[:       GetCell(FX,FY);                                {  04 }
    else
      if Ch in [' '..'~'] then
      GetCell(FX,FY);                                          {  04 }
    end;
  until true=false;          { (program stops in procedure Commands) }
end.

