10 REM ======================================
20 REM  ST. ELMO'S FIRE (MAN IN MOTION)
30 REM  SIDPLAY VERSION (PSID CONVERSION)
40 REM ======================================
50 PRINT CHR$(12)
60 COLOR 14,0
70 PRINT " ST. ELMO'S FIRE - SIDPLAY"
80 PRINT
90 PRINT " FILE: st_elmos_fire_man_in_motion.sid"
100 PRINT " PRESS RUN/STOP TO END"
110 SIDSTOP
120 SIDPLAY "st_elmos_fire_man_in_motion",1
130 VSYNC
140 GOTO 130
