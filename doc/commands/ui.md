# User Interface

## EditorCopy

Copy the selected text to the clipboard.

## EditorCut

Cut the text selection to the clipboard.


## EditorPaste

Insert the clipboard contents on the command-line.

## EditorSearch

Incremental search in the text editor.


## Wait

Wait for a key press or a time lapse.

When the argument is greater than 0, interrupt the program for the given number
of seconds, which can be fractional.

When the argument is 0 or negative, wait indefinitely until a key is
pressed. The key code for the key that was pressed will be pushed in the
stack. If the argument is negative, the current menu will be displayed on the
screen during the wait.

For example, the following program will count for approximately 3 seconds:

```rpl
1 30 for i
   i 1 disp
   0.1 wait
next
```

## Prompt

Display a message in the header area, then stop the currently executing program
to wait for user input. The user should press the `Run` key to resume execution
once the user has performed the requested operations.

For example, the following program displays a message at the top of the screen,
then waits for the user to put a number on the stack and hit the `Run` key. The
program then deposits the next number on the stack.

```rpl
«
	"Enter number then press Run" Prompt
    1 +
    "Next number" →Tag
»
```

Once you run the program, you can type a value, then use the `Run` key left of
`+` to resume execution:

```rpl
123
```

## KEYEVAL
Simulate a keypress from within a program


## Key

Get instantaneous state of the keyboard


## DOFORM
Take a variable identifier with a form list


## EDINSERT
Insert given text into the editor


## EDREMOVE
Remove characters in the editor at the cursor position


## EDLEFT
Move cursor to the left in the editor


## EDRIGHT
Move cursor to the right in the editor


## EDUP
Move cursor up in the editor


## EDDOWN
Move cursor down in the editor


## EDSTART
Move cursor to the start of text in the editor


## EDEND
Move cursor to the end of text in the editor


## EDLSTART
Move cursor to the start of current line in the editor


## EDLEND
Move cursor to the end of current line in the editor


## EDTOKEN
Extract one full word at the cursor location in the editor


## EDACTOKEN
Extract one word at the left of cursor location (suitable for autocomplete)


## EDMODE
Change the cursor mode in the editor


## SETTHEME
Set system color theme


## GETTHEME
