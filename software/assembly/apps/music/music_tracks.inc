; Embedded curated track table (55-byte entries): 32 name + 1 type + 16 artist + 6 model.
.segment "RODATA"

TRACK_SID = $01
TRACK_MID = $03

.macro track filename, file_type, artist, model
    .assert .strlen(filename) < 32, error, "track filename exceeds 31 characters"
    .assert .strlen(artist) < 16, error, "track artist exceeds 15 characters"
    .assert .strlen(model) < 6, error, "track model exceeds 5 characters"
    .byte filename
    .repeat 32 - .strlen(filename)
        .byte 0
    .endrepeat
    .byte file_type
    .byte artist
    .repeat 16 - .strlen(artist)
        .byte 0
    .endrepeat
    .byte model
    .repeat 6 - .strlen(model)
        .byte 0
    .endrepeat
.endmacro

tracks_featured:
    track "champagne-supernova.mid", TRACK_MID, "OASIS", "WTS"
    track "Love_Fileosophy_2SID.sid", TRACK_SID, "LINUS", "2SID"
    track "Fratres_2SID.sid", TRACK_SID, "THOMAS/CRT", "2SID"
    track "commando.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "monty-on-the-run.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "tubular-airwolf.sid", TRACK_SID, "MIKE OLDFIELD", "6581"
    track "sousa-stars-stripes.mid", TRACK_MID, "J.P. SOUSA", "WTS"
    track "bach-toccata-dm.mid", TRACK_MID, "J.S. BACH", "WTS"
    track "joplin-entertainer.mid", TRACK_MID, "SCOTT JOPLIN", "WTS"
    track "miami-vice.mid", TRACK_MID, "JAN HAMMER", "WTS"
    track "tetris-theme.mid", TRACK_MID, "TRADITIONAL", "WTS"
    track "castlevania-bloody-tears.mid", TRACK_MID, "KONAMI", "WTS"
    track "star-wars.mid", TRACK_MID, "J. WILLIAMS", "WTS"
    track "washington-post-march.mid", TRACK_MID, "J.P. SOUSA", "WTS"
    track "semper-fidelis.mid", TRACK_MID, "J.P. SOUSA", "WTS"

tracks_2sid:
    track "Love_Fileosophy_2SID.sid", TRACK_SID, "LINUS", "2SID"
    track "Fratres_2SID.sid", TRACK_SID, "THOMAS/CRT", "2SID"
    track "Popel_Premiere_2SID.sid", TRACK_SID, "PVCF", "2SID"

tracks_sid:
    track "commando.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "monty-on-the-run.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "parallax.sid", TRACK_SID, "MARTIN GALWAY", "6581"
    track "wizball.sid", TRACK_SID, "MARTIN GALWAY", "6581"
    track "crazy-comets.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "delta.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "cybernoid.sid", TRACK_SID, "JEROEN TEL", "6581"
    track "sanxion.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "lightforce.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "master-of-magic.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "intl-karate.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "cybernoid-2.sid", TRACK_SID, "JEROEN TEL", "6581"
    track "zoids.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "thing-on-a-spring.sid", TRACK_SID, "ROB HUBBARD", "6581"
    track "ocean-loader-2.sid", TRACK_SID, "MARTIN GALWAY", "6581"

tracks_wts:
    track "champagne-supernova.mid", TRACK_MID, "OASIS", "WTS"
    track "sousa-stars-stripes.mid", TRACK_MID, "J.P. SOUSA", "WTS"
    track "bach-toccata-dm.mid", TRACK_MID, "J.S. BACH", "WTS"
    track "joplin-entertainer.mid", TRACK_MID, "SCOTT JOPLIN", "WTS"
    track "debussy-clair-de-lune.mid", TRACK_MID, "C. DEBUSSY", "WTS"
    track "miami-vice.mid", TRACK_MID, "JAN HAMMER", "WTS"
    track "tetris-theme.mid", TRACK_MID, "TRADITIONAL", "WTS"
    track "castlevania-bloody-tears.mid", TRACK_MID, "KONAMI", "WTS"
    track "star-wars.mid", TRACK_MID, "J. WILLIAMS", "WTS"
    track "grieg-mountain-king.mid", TRACK_MID, "E. GRIEG", "WTS"
    track "pink-panther.mid", TRACK_MID, "H. MANCINI", "WTS"
    track "washington-post-march.mid", TRACK_MID, "J.P. SOUSA", "WTS"
    track "semper-fidelis.mid", TRACK_MID, "J.P. SOUSA", "WTS"
    track "vivaldi-spring.mid", TRACK_MID, "A. VIVALDI", "WTS"
    track "mozart-eine-kleine.mid", TRACK_MID, "W.A. MOZART", "WTS"
    track "beethoven-fur-elise.mid", TRACK_MID, "L. BEETHOVEN", "WTS"
    track "chopin-nocturne-op9n2.mid", TRACK_MID, "F. CHOPIN", "WTS"
    track "pachelbel-canon.mid", TRACK_MID, "J. PACHELBEL", "WTS"
    track "joplin-maple-leaf.mid", TRACK_MID, "SCOTT JOPLIN", "WTS"
    track "grieg-morning-mood.mid", TRACK_MID, "E. GRIEG", "WTS"

tracks_arcade:
    track "sonic-green-hill.mid", TRACK_MID, "M. NAKAMURA", "WTS"
    track "super-mario-bros.mid", TRACK_MID, "K. KONDO", "WTS"
    track "zelda-overworld.mid", TRACK_MID, "K. KONDO", "WTS"
    track "megaman2-wily.mid", TRACK_MID, "T. TAKASHI", "WTS"
    track "street-fighter-2.mid", TRACK_MID, "Y. SHIMOMURA", "WTS"
    track "contra-theme.mid", TRACK_MID, "KONAMI", "WTS"
    track "mission-impossible.mid", TRACK_MID, "L. SCHIFRIN", "WTS"
    track "x-files.mid", TRACK_MID, "MARK SNOW", "WTS"
    track "ghostbusters.mid", TRACK_MID, "R. PARKER JR", "WTS"
    track "hawaii-five-o.mid", TRACK_MID, "M. STEVENS", "WTS"

track_ptrs_lo:  .byte <tracks_featured, <tracks_2sid, <tracks_sid, <tracks_wts, <tracks_arcade
track_ptrs_hi:  .byte >tracks_featured, >tracks_2sid, >tracks_sid, >tracks_wts, >tracks_arcade
track_counts:   .byte 15, 3, 15, 20, 10
