program Jukebox;

{ Nova sound demo.

  Everything here goes through the canonical NDK audio module: MML sequences
  are queued per voice, played, looped, and re-tempoed, and fire-and-forget
  sound effects use a Pascal-defined SID instrument.  A short inline-assembly
  block hands the sequencer the address of the staging buffer, which is the
  one thing the typed byte ABI cannot express.

  Keys: 1-3 pick a tune, S stops, L loops, +/- change tempo,
        space fires a sound effect, Esc leaves.

  Key input uses NovaInput.PollKey rather than Crt.KeyPressed, which the
  Crt unit currently declares as a routine while the runtime publishes it
  as byte storage. }

uses Crt, NovaInput, NovaAudio;

const
  { AUDIO.VOICE is one-based: 1..6 are SID voices, 7..14 are WTS voices. }
  Melody = 1;
  Bass = 2;
  Counter = 3;
  Effect = 1;

var
  Mml: string[120];
  Status, Key: Byte;
  Tempo: Byte;
  Looping: Boolean;
  Ch: Char;

procedure Queue(Voice: Byte);
begin
  AUDIO_VOICE := Voice;
  AUDIO_STRLEN := Length(Mml);
  asm
    ; The sequencer takes a pointer; MML is the Pascal string's length byte.
    lda #<(MML+1)
    sta AUDIO_STRL
    lda #>(MML+1)
    sta AUDIO_STRH
  end;
  Status := AUDIO_MUSIC_SEQUENCE()
end;

procedure UseTempo(Bpm: Byte);
begin
  AUDIO_NOTE := Bpm;
  AUDIO_DURATION := 0;
  Status := AUDIO_MUSIC_TEMPO()
end;

procedure UseLoop(Enabled: Boolean);
begin
  if Enabled then
    AUDIO_NOTE := 1
  else
    AUDIO_NOTE := 0;
  Status := AUDIO_MUSIC_LOOP()
end;

procedure DefineEffect;
begin
  AUDIO_INST_ID := Effect;
  AUDIO_WAVEFORM := $40;             { pulse }
  AUDIO_ATTACK := 0;
  AUDIO_DECAY := 6;
  AUDIO_SUSTAIN := 8;
  AUDIO_RELEASE := 6;
  Status := AUDIO_INSTRUMENT()
end;

procedure Blip(Note, Frames: Byte);
begin
  AUDIO_NOTE := Note;
  AUDIO_DURATION := Frames;
  AUDIO_SOUND_INSTRUMENT := Effect;
  Status := AUDIO_SOUND_ASYNC()
end;

procedure Sweep;
var
  Note: Byte;
begin
  Note := 48;
  while Note < 84 do
  begin
    Blip(Note, 3);
    Delay(45);
    Note := Note + 4
  end
end;

procedure Play(Choice: Byte);
begin
  Status := AUDIO_MUSIC_STOP();
  case Choice of
    1:
      begin
        Mml := 'I1 O5 L8 CEGE CEGE DFAF DFAF EGBG EGBG >C<GEC C1';
        Queue(Melody);
        Mml := 'I2 O2 L4 CC GG AA GG CC GG C1';
        Queue(Bass)
      end;
    2:
      begin
        Mml := 'I3 O5 L16 ~4 EFGAB>C<BAGFED CDEFGAB>C4';
        Queue(Melody);
        Mml := 'I1 O4 L8 {CEG}4 {DFA}4 {EGB}4 {CEG}4';
        Queue(Counter);
        Mml := 'I2 O2 L8 C R C R G R G R A R A R G R G R';
        Queue(Bass)
      end;
    3:
      begin
        Mml := 'I1 O4 L4 V12 C E G >C< G E C R';
        Queue(Melody);
        Mml := 'I2 O3 L8 V10 EGEG FAFA GBGB >C<GEC';
        Queue(Counter);
        Mml := 'I3 O1 L4 V15 C C F F G G C1';
        Queue(Bass)
      end
  end;
  UseTempo(Tempo);
  UseLoop(Looping);
  Status := AUDIO_MUSIC_PLAY_ASYNC()
end;

procedure Menu;
begin
  ClrScr;
  TextColor(Yellow);
  writeln('NovaPascal jukebox');
  TextColor(LightGray);
  writeln('------------------');
  writeln;
  writeln('1  arpeggio study      (melody + bass)');
  writeln('2  chiptune            (lead, arpeggios, pulse bass)');
  writeln('3  three-voice march');
  writeln;
  writeln('S  stop        L  loop on/off');
  writeln('+  faster      -  slower');
  writeln('Space  sound effect sweep');
  writeln('Esc  quit');
  writeln
end;

procedure ShowStatus;
begin
  GotoXY(1, 15);
  TextColor(LightGreen);
  write('Tempo ');
  write(Tempo);
  if Looping then
    write(' bpm, loop on ')
  else
    write(' bpm, loop off');
  if AUDIO_MUSIC_PLAYING() <> 0 then
    writeln('   playing  ')
  else
    writeln('   idle     ');
  TextColor(LightGray)
end;

begin
  Ch := ' ';
  Tempo := 140;
  Looping := true;
  DefineEffect;
  Menu;
  ShowStatus;
  repeat
    Key := PollKey();
    if Key <> 0 then
    begin
      Ch := Chr(Key);
      case Ch of
        '1':
          Play(1);
        '2':
          Play(2);
        '3':
          Play(3);
        's', 'S':
          Status := AUDIO_MUSIC_STOP();
        'l', 'L':
          begin
            Looping := not Looping;
            UseLoop(Looping)
          end;
        '+', '=':
          begin
            if Tempo < 240 then
              Tempo := Tempo + 20;
            UseTempo(Tempo)
          end;
        '-', '_':
          begin
            if Tempo > 60 then
              Tempo := Tempo - 20;
            UseTempo(Tempo)
          end;
        ' ':
          Sweep
      end
    end;
    Delay(60);
    ShowStatus
  until Key = 27;
  Status := AUDIO_MUSIC_STOP();
  GotoXY(1, 20);
  writeln('Jukebox finished.')
end.
