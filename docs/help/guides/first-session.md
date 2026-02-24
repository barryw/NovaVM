---
title: "Your First Session"
type: guide
category: General
---

> Tell me and I forget. Teach me and I remember. Involve me and I learn. -- *Benjamin Franklin*

The fastest way to get comfortable with NovaBASIC is to use it. This chapter
walks you through everything you need for a productive first session: entering
programs, editing them, saving and loading files, and building the habits that
will serve you on every project after this one.

## The Edit-Run Cycle

NovaBASIC stores your program as an ordered list of numbered lines. When you
type a line beginning with a number and press Return, that line is added to
the program in the correct position. When you type `RUN`, execution begins
at the lowest line number and proceeds in order.

This is the fundamental loop:

```
TYPE A LINE -> LIST ->
RUN -> FIX -> RUN
```

Here is a small program to enter right now:

```basic
10 PRINT "NOVABASIC IS READY"
20 X = 7
30 PRINT "SEVEN SQUARED IS "; X*X
```

Type `LIST` to see the program back. Type `RUN` to execute it. Now
change line 20 by retyping it with a new value:

```basic
20 X = 12
```

The old line 20 is replaced. Type `RUN` again and the result changes.
To delete a line entirely, type its number alone and press Return:

```basic
30
```

Line 30 is gone. `LIST` confirms it.

## Essential Editing Commands

| **Command** | **What It Does** |
| --- | --- |
| LIST | Display the entire program currently in memory. |
| LIST 20-40 | Display only lines 20 through 40 (inclusive). |
| NEW | Clear the program from memory. Cannot be undone. |
| RUN | Execute the program starting from the lowest line number. |
| CONT | Continue execution after a `STOP` statement. |

::: warning
`NEW` erases everything in memory immediately. Save your work with
`SAVE` before typing `NEW` if you want to keep it.
:::

## Saving and Loading

Programs are saved to and loaded from the virtual file system using four
commands:

- **`SAVE "name"`**
Write the current program to disk under the given name. The
`.bas` extension is added automatically.
- **`LOAD "name"`**
Load a saved program from disk into memory, replacing anything
currently there.
- **`DIR`**
List all saved BASIC programs.
- **`DEL "name"`**
Delete a saved program. This cannot be undone.

::: note
Filenames may contain letters (`A--Z`, `a--z`), digits
(`0--9`), underscores, hyphens, and dots. Maximum length is
63 characters. Names are case-sensitive on disk.
:::

Here is a complete save-and-reload sequence:

```basic
SAVE "MYPROG"
DIR
NEW
LOAD "MYPROG"
RUN
```

`DIR` shows `MYPROG.bas` in the listing. After `LOAD` and
`RUN`, the program executes as if you had just typed it in.

## Working Efficiently

Good habits established early make everything easier later.

::: tip
- **Number lines by tens.** Use line numbers 10, 20, 30...
rather than 1, 2, 3. This leaves room to insert new lines between
existing ones without renumbering.
- **Use `REM` freely while learning.** A comment line
costs a little memory but saves a lot of confusion. You can remove
them once the code is stable.
- **Build and run in small steps.** Add a few lines, `RUN`,
verify the output, add more. Chasing bugs through a hundred lines you
typed without testing is no fun.
- **Group lines by function.** A common convention: 100s for
initialisation, 200s for input, 300s for game logic, 800s for output
routines, 900s for cleanup and quit. Any scheme that makes sense to
you is fine; the important thing is to pick one and use it.

:::

## The Round-Trip Exercise

The following exercise takes you through writing, saving, clearing, reloading,
and running a program. Every step matters -- do not skip any of them.

::: tryit
```basic
10 PRINT "ROUND TRIP COMPLETE"
```

Then, at the prompt (no line number):
```basic
SAVE "ROUNDTRIP"
NEW
DIR
LOAD "ROUNDTRIP"
RUN
```

**Expected:**

- After `SAVE`: the prompt returns immediately with no error.
- After `NEW`: `LIST` shows nothing.
- After `DIR`: `ROUNDTRIP.bas` appears in the listing.
- After `LOAD` and `RUN`: the screen prints
`ROUND TRIP COMPLETE`.

:::

If you see `ROUND TRIP COMPLETE` at the end, you have mastered the
complete NovaBASIC workflow. Everything else is built on top of exactly this.
