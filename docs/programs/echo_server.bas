10 REM === ECHO SERVER ===
20 REM Listens on port 8080, echoes back anything received.
30 REM Test with:  python3 docs/programs/nic_client.py
40 PRINT CHR$(12)
50 PRINT "=== ECHO SERVER ==="
60 PRINT
70 NLISTEN 0,8080
80 PRINT "Listening on port 8080..."
90 PRINT "Waiting for connection..."
100 IF NOT NREADY(0) THEN 100
110 NACCEPT 0
120 PRINT "Client connected!"
130 PRINT
140 REM --- receive loop ---
150 IF (NSTATUS(0) AND 1) = 0 THEN 250
160 IF NOT NREADY(0) THEN 150
170 A$=NRECV$(0)
180 L=NLEN
190 IF L=0 THEN 150
200 PRINT "Recv (";L;" bytes): ";A$
210 NSEND 0,A$
220 PRINT "Echo: ";A$
230 GOTO 150
240 REM --- disconnected ---
250 PRINT
260 PRINT "Client disconnected."
270 NCLOSE 0
280 PRINT
290 GOTO 70
