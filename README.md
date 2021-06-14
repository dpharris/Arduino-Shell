# Arduino-Shell

## Changes/Extensions
Please refer to the original **Arduino-Shell**.  *Mikael Patek* has done 
an amazing tour de force.  

I have made some changes and extensions to allow multiple scripts to be run pseudo-simultaneously.  This required implementing local-variables in a struct and reserving and freeing from the heap.   

Changed are indicated by **bolding**.

## Intro

This library provides a Forth/PostScript style shell for Arduino
sketches. Shell is a virtual stack machine with a byte token threaded
instruction set. The tokens, characters, are chosen so that it is
possible to write small scripts directly without a token compiler. As
Forth scripts are written in Reverse Polish Notation (RPN), and
executed in the given order.

Shell has built-in instruction level trace to aid script debugging and
performance tuning. It prints the instruction cycle count, script
address, opcode (or full operation name), stack depth and
contents. Typical output in the Serial Monitor above.

Shell can be configured to trace full operation names or tokens.

The classical Arduino Blink sketch in the shell script language is
```
 13O{13H1000D13L1000DT}w
```
And with some extra spacing to make the operations easier to read.
```
 13 O { 13 H 1000 D 13 L 1000 D T } w
```
And with full instruction names and some formatting:
```
 13 output
 {
   13 high 1000 delay
   13 low  1000 delay
   true
 } while
```
A further compressed version (shorter):
```
 13 output
 {
   1000 13 over over high delay
   low delay
   true
 } while

 13O{1000,13ooHDLDT}w
```
And a slightly faster version:
```
 1000 13 dup output
 {
   over over high delay
   over over low delay
   true
 } while

 1000,13uO{ooHDooLDT}w
```
And an even shorter version:
```
 13 output
 {
   13 digitalToggle 1000 delay
   true
 } while

13O{13X1000DT}w
```
Scripts can be defined and executed from both Random Access Memory
(SRAM), Electrically Erasable Programmable Read-Only Memory (EEPROM),
and Program Memory (PROGMEM). The supporting data type `script_t` and
macro `SCRIPT` can be used to define an application dictionary with
program memory based scripts. All named scripts are persistent.
Variables may be written to EEPROM with the operation `z`. They are
automatically restored at startup together with defined scripts.

## Install

Download and unzip the Arduino-Shell library into your sketchbook
libraries directory. Rename from Arduino-Shell-master to Arduino-Shell.

The Shell library and examples should be found in the Arduino IDE
File>Examples menu.

## Instruction Set

Opcode | Parameters | Description | Forth
-------|:-----------|:------------|:-----
' | 'c | literal character |
**"..."** | **"text" --** | **comment** | **(...)**
(...) | (chars) -- | print chars |
[...] | [ d0 ... dn ] -- d0 ... dn n | stack block
{...} | {code} -- addr | code block 
, | -- | no operation |
&#43; | x y -- x+y | addition | +
&#45; | x y -- x-y | subtraction | -
&#42; | x y -- x*y | multiplication | *
/ | x y -- x/y | division | /
% | x y -- x%y | modulo | MOD
&#35; | x y -- x!=y | not equal |
= | x y -- x==y | equal | =
< | x y -- x<y | less than | <
&#62; | x y -- x>y | greater than | >
~ | x -- ~x | bitwise not | NOT
& | x y -- x&y | bitwise and | AND
&#124; | x y -- x&#124;y | bitwise or | OR
^ | x y -- x^y | bitwise xor | XOR
@ | addr -- val | read variable | @
! | val addr -- | write variable | !
. | x -- | print number followed by one space | .
? | addr -- | print variable | ?
; | addr block -- | copy block and assign variable (function) |
\ | x1..xn n -- x1..xn | n > 0: mark stack frame with n-elements |
\ | x1..xn y1..ym n -- y1..ym | n < 0: remove stack frame with n-elements |
$ | n -- addr | address of n-element in frame |
a | -- bytes entries | allocated eeprom |
b | base -- | number print base | BASE
c | xn..x1 n -- | drop n stack elements |
d | x -- | drop | DROP
e | flag if-block else-block -- | execute block on flag | IF ELSE THEN
f | addr -- | forget variable |
g | xn..x1 n -- xn-1..x1 xn | rotate n-elements | ROLL
h | x y z -- (x*y)/z | scale | */
i | flag block -- | execute block if flag is true | IF THEN
j | xn..x1 -- xn..x1 n | stack depth | DEPTH
k | -- char | read character from input stream  | KEY
**l** | low high block( index -- ) -- | execute block from low to high | DO LOOP
&#32; | **high low block( index -- ) --** | **execute block from high to low** | 
m | -- | write new line to output stream | CR
n | x -- -x | negate | NEGATE
o | x y -- x y x | over | OVER
p | xm..x1 n -- xm..x1 xn | pick | PICK
q | x -- [x x] or 0 | duplicate if not zero | ?DUP
r | x y z --- y z x | rotate | ROT
s | x y -- y x | swap | SWAP
t | addr -- bool | write variable name to output stream | .NAME
u | x -- x x | duplicate | DUP
v | char -- | write character to output stream | EMIT
w | block( -- flag) -- | execute block while flag is true | BEGIN UNTIL
x | block -- | execute block | EXECUTE
y | -- | yield for multi-tasking scheduler |
z | addr -- | write variable to eeprom memory |
A | pin -- sample | analogRead(pin) |
**B** | **bit# -- bitmask** | **bitmask(bit)** |
C | xn..x1 -- | clear | ABORT
D | ms -- | delay |
E | period addr -- bool | check if timer variable has expired |
F | -- false | false | FALSE
H | pin -- | digitalWrite(pin, HIGH) |
I | pin -- | pinMode(pin, INPUT) |
K | -- [char true] or false | non-blocking read character from input stream | ?KEY
L | pin -- | digitalWrite(pin, LOW)  |
M | -- ms | millis() |
N | -- | no operation |
O | pin -- | pinMode(pin, OUTPUT) |
P | value pin -- | analogWrite(pin, value) |
**Q** | **Qx###** | **period/time literal, x = u,s,m,h,t for us, s, min, h, time** |
R | pin --  bool | digitalRead(pin) |
S | -- | print stack contents | .S
T | -- true | true | TRUE
U | pin -- | pinMode(pin, INPUT_PULLUP) |
**V** | **range -- random** | **random(range)** |
W | value pin -- | digitalWrite(pin, value) |
X | pin -- | digitalToggle(pin)  |
Y | -- | list **words** | WORDS
Z | -- | toggle trace mode |

### Extensions (subject to change)
Opcode | Parameters | Description 
-------|:-----------|:------------
**_c** | -- ch | current **c**hannel
**_p** | ch -- pin# | **p**in associated with this channel
**_I** | data n addr -- | **I**2C write: Wire.send(addr,n,data)
**_i** | data n addr -- data n | **i**2c send with reply: Wire.requestFrom(addr, n, data)
**_j** | -- data n addr | **j**2c async read: Wire.onreceive(addr,n,&data)
**_F** | -- fastclock | returns **F**astClock value
**_T** | -- realtime  | returns Real**T**imeClock value
**_P** | ch -- | **P**roduce the event for the channel
**_R** | offset ch -- | Produce a **R**ange-eventid by adding offset to ch's eid
**_r** | -- range | value in last **r**ange received
**_E** | -- | print the script definitions in **E**eprom
**_Y** | n -- | print the script associated with eventid-n
**_Y** | 0 -- | print all scripts associated with eventids
**_Z** | n -- | write a block to the script associated with eventid-n
**_G** | group n -- | assign **G**roup to the script associated with eventid-n
**_M** | -- mem | print free-**M**emory value
**_!** | -- | stop all active scripts

## Special forms

The shell script language allows several special forms such as literal
values, blocks, control structures and argument frames.

### Comments (extension)
**Comments are indicated by surrounding them with "'s, eg "a comment".
They are skipped over in the scanning.  Do _not_ place them inside code 
defintions, as that will slow down interpretation.  Rather, leave 
them outside of definitions.  Eg: ***
```
:double{2*}; "(n -- 2*n) twice"
```

### Boolean

Boolean values are true(-1) and false(0). Mapping to boolean may be
done with testing non-zero (0#). Defining true(-1) allows Boolean
values and relation operations to be directly combined with bitwise
operations.
```
 10 0 # . m
 T F ~ = . m
```
The instructions to push the boolean values are _T_ and _F_.

### Literal Numbers

Integer numbers (decimal, binary and hexadecimal) may be used directly
in scripts. When the script is executed the value of the number is
pushed on the parameter stack. The statement:
```
 println((3 + (-5)) * 6);
```
may be written as the following script expression:
```
 3 -5 + 6 * . m
```
and compressed to:
```
 3,-5+6*.m
```
Binary literal numbers are prefixed with `0b`, and hexadecimal with
`0x` as in C.
```
 10 . 0b10 . 0x10 .
```
**Time literal numbers are prefixed with `Q`:**
```
 10   - 10 ms
 Qu10 - 10 microseconds
 Qs10 - 10 seconds
 Qm10 - 10 minutes
 Qh10 - 10 hours
 Qt8:30.3 - specific time: 08:60:18
```
**Note; these are implemented as ranges:**

Unit | Value Range | Time Range |Note 
-------|:----------|:--------------|:---------
 ms | 0-9,999 | 0ms-9,999ms, ie 0-9.999s | for compatibiliy
 us | 10,000-19,999 | 0us-9,999us or 0-9.999ms 
 sec | 20,000-29,999 | 0s-9,999s or 0-167min or 0-2.78h 
 min | 30,000-39,999 | 0min-9,999min or 0-167h or 0-7d 
 hour | 40,000-49,999 | 0h-9,999h or 0-416d or 0-1year 
 time | 50,000-64400 | 00:00.0-23:59.9 
 
 **Note: be careful with comparisons.**  

### Literal Characters

Quote (apostrophe) a character to push it on the parameter stack.
```
 'A .
```

### Printing items

  - `2b` sets base to binary`;
```
   `10.` prints as `0b1010 `.
```
  - `8b` sets base to octal`;
```
   `19.` prints as `0o12 `.
```
  - `16b` sets base to hexadecimal`:
```
   `10.` prints as `0xa `.
```
  - **`1b`** sets base to time:
```     
 `10.` prints as `10ms `
 `Qu10.` prints as `10us `
 `Qs10.` prints as `10s `
 `Qm10.` prints as `10min `
 `Qh10.` prints as `10h `
 `Qt10:30:15.` prints as `10:30:15h `;
```

### Variables

Variables are defined with `:name`. The operator will push the
address of the variable on the parameter stack. It may be accessed
using the operators fetch `@` and store `!`.
```
 42:x!
 :x@
```
The operator `?` can be used to print the value of a variable.
```
 :x?
```
It is short hand for:
```
 :x@.
```

### Blocks

Code blocks have the following form `{ code-block }`. When the script
is executed the address of the block is pushed on the parameter
stack. The block can be executed with the instruction _x_.
```
 { code-block } x
```
The operation `;` will copy a block to the eeprom and assign a
variable. Used in the form:
```
 :fun { code-block };
 :fun@x
```
The short hand for executing a function is `` `name ``.
```
 `fun
```
The instruction _f_ may be used to forget variable(s). The eeprom
allocation point is reset accordingly.
```
 :fun,f
```

** Pseudo-simultanality is achieved by returning after each ``block`` executes, and then returning to complete the operations, ie** **w**hile, **l**oop, **i**f, if**e**lse, e**X**ecute, and **D**elay.  

### Control Structures

Control structures follow the same format at PostScript. They are also
in Reverse Polish Notation (RPN). The blocks are pushed on the
parameter stack before the control structure instruction. Below are
the control structures with full instruction names.
```
 bool { if-block } if
 bool { if-block } { else-block } ifelse

 low high { index loop-block } loop

 { while-block bool } while
```
The `loop-block` will recieve the current index on top of parameter
stack. The `while-block` should push a non-zero value to continue the
while loop otherwise zero (false) to terminate the loop.

The instructions are _i_ for `if`, _e_ for `ifelse`, _l_ for `loop`
and _w_ for `while`.

### Output Strings

Output strings have the following form `( output-string )`. When executed the
string within the parenthesis is written to the output stream. The
instruction _m_ will print a new-line (corresponds to forth cr).

### Stack Marker

A stack marker has the following form `[ code-block ]`. When executed the
number of stack elements generated by the code block is pushed on the
parameter stack.

### Frame Marker

A frame marker has the following form `n\ ... -n\` where _n_ is the
number of elements (parameters and locals) in the frame. Positive _n_
marks the frame and negative _n_ removes the frame stack elements
leaving any return values. Elements within the frame can be accessed
with `m$` where _m_ is the element index (1..n). The element address
is pushed on the parameter stack and the value may be accessed with
fetch `@` and store `!`.

Swap could be defined as:
```
 :swap {2\2$@1$@-2\};
```
which will mark a frame with two arguments, copy the second and then
the first argument, and last remove the frame, leaving the two return
values.

### Extended Instructions

Shell allows application extension with a virtual member function,
`trap()`. The function is called when the current instruction could not
be handled. The `trap()` function may parse any number of
instructions. Underscore `_` is used as the escape operation code.

## Example Scripts

### Blink

Turn board LED, pin 13, on/off with 1000 ms period.
```
 13 output
 {
   13 high 1000 delay
   13 low 1000 delay
   true
 } while
```
Script:
```
 13O{13H1000D13L1000DT}w
```

### Blink with digitalToggle

Toggle board LED, pin 13, on/off with 1000 ms period.
```
 13 output
 {
   13 digitalToggle 1000 delay
   true
 } while
```
Script:
```
 13O{13X1000DT}w
```

### Blink without delay

Turn board LED, pin 13, on/off without using delay. Use timer expired
instruction.
```
 13 output
 {
   1000 `timer ?expired
   { 13 digitalToggle } if
   true
 } while
```
Script:
```
 13O{1000:timer,E{13X}iT}w
```

### Blink controlled by on/off button

Turn board LED, pin 13, on/off with 1000 ms period if pin 2 is low.
```
 2 inputPullup
 13 output
 {
   2 digitalRead not
   {
     13 high 1000 delay
     13 low 1000 delay
   } if
   true
 } while
```
Script:
```
 2U13O{2R~{13H1000D13L1000D}iT}w
```

### Read Analog Pins

Read analog pins and print value in format "An = value".
```
 0 4 { ." A" dup . ." =" analogRead . cr } loop
```
Script:
```
 0,4{(A)u.(= )A.m}l
```
### Continously Read Analog Pins

Read analog pins (0..4) and print value continuously with 100 ms delay.
```
 {
   0 4 { analogRead . } loop
   cr 100 delay true
 } while
```
Script:
```
 {0,4{A.}lm100DT}w
```

### Termostat

Read analog pin 0, turn board LED on if value is within 100..200 else off.
```
 13 output
 {
   0 analogRead
   dup 100 < swap 200 > or not
   13 digitalWrite
   true
 } while
```
Script:
```
 13O{0Au100<s200>|~13WT}w
```

### Range check function

Check that a given parameter is within a range low to high.
```
 : within ( x low high -- bool )
   rot swap over swap > swap rot < or not ;

  10 5 100 within .
 -10 5 100 within .
 110 5 100 within .
```
Script:
```
 :within{rsos>sr<|~};
  10,5,100`within.
 -10,5,100`within.
 110,5,100`within.
```

### Range check function with stack frame

Check that a given parameter is within a range low to high. Use a
stack frame for the three parameters.
```
 : within { x low high -- bool }
   x @ high @ > x @ low @ < or not ;

  10 5 100 within .
 -10 5 100 within .
 110 5 100 within .
```
Script:
```
 :within{3\1$@3$@>1$@2$@<|~-3\};
  10,5,100`within.
 -10,5,100`within.
 110,5,100`within.
```

### Iterative Factorial (while)

Calculate factorial number of given parameter.
```
 : fac ( n -- n! )
   1 swap
   {
     dup 0>
       { swap over * swap 1- true }
       { drop false }
     ifelse
   } while ;

 5 fac .
```
Script:
```
 :fac{1s{u0>{so*s1-T}{dF}e}w};
 5`fac.
```

### Iterative Factorial (loop)

Calculate factorial number of given parameter.
```
 : fac ( n -- n! | n > 0 )
   1 2 rot { * } loop ;

 5 fac .
```
Script:
```
 :fac{1,2r{*}l};
 5`fac.
```

### Recursive Factorial

Calculate factorial number of given parameter.
```
 : fac ( n -- n! )
   dup 0>
     { dup 1- fac * }
     { drop 1 }
   ifelse ;

 5 fac .
```
Script:
```
 :fac{u0>{u1-`fac*}{d1}e};
 5`fac.
```

### Stack vector sum

Sum a vector of integers on stack. Use that stack marker to get number
of elements in vector.
```
 [ 1 2 3 4 5 6 7 8 ] 0 1 rot { drop + } loop
```
Script:
```
 [1,2,3,4,5,6,7,8]0,1r{d+}l
```

## Extension examples:

### Bitmask

Query bit #6 from reg, and report:
```
 : reg@ 6B & (bit6 is ) { (present)m }{ (absent)m }e
```
 
### Random

Roll a dice, and report:
```
 6V u2={(Three)}i u0={(One)}i u1={(Two)}i u3={(Four)}i u4={(Five)}i 5={(Six)}i
   "Mimics a case-statement, but no default-case."
 or
 [ {(Six)}, {(Five)}, {(Four)}, {(Three)}, {(Two)}, {(One)} ] 6V1+px dddddd
   "Leaves six blocks on stack, picks a random one and executes it, drops the original blocks"
   "Mimics a (seqential) case-statement."
```

### Time

Delay 100 microseconds, note that this is a blocking delay:
```
Qu100D
```
Delay 50 milliseconds, seconds, minutes, hours, respectively,
note that these are all non-blocking delays.
```
50D
Qs50D
Qm50D
Qh50D
```
Print "Bong" at noon, real-time, once:
```
 { Qt12_T= u{ (Bong)m }i ~}w
```

### I2C (subject to change)

Send to MCP23017 I/O Expander, addressed as 0x40:
```
 [0x08,0x12,0x40]_I or 8,0x12,0x40,3_I   -- write 8 to GPIOA(0x12) on chip at I2C-address 0x40
```
Read from MCP23017, register GPIO(0x12):
```
 [0x12,0x40]_I 1,0x40_i   -- first address the chip and the register address, then read 1 byte from it.  
```

### Ranges

_r and _R let you use eventid-ranges.  One of the eventids will be set with
n-zero bits, and indicated in the 'range' field in the event by inserting the 
number of bits to be used.  

In this example, the two lower bits are used to represent aspects: 
 * 0 = Stop
 * 1 = Caution
 * 2 = Advanced caution
 * 3 = Proceed
 
 To consume a eventid-range: 
 ```
 _r0={4_pH} "if stop-aspect, turn on red"
 ```
 And to send a range member: 
 ```
 3,12_R "report proceed-aspect on channel 12"
 ```

## Under the Hood

### Changes to original Shell lib

The Shell was extended so that several scripts could be operating to implement effects on multiple pins in (pseudo-) parallel.  For example, one pin might be flashing, while another fades.  This modification allows these scripts to execute independently to achieve their effects.  

#### Yielding
Since scripts are executed in (pseudo-)parallel, each script needs to be able to yield back to the main code to allow the other scripts to be executed.  This is done automatically by yielding at each **Delay**, and at each **loop**, **while**, **if**, or **else** block, using the re-entry code described below which returns the active context and location in the code. 

#### Re-entry
Each script uses the same execute() code, and therefore execute() needs to be re-entrant.  To allow this, a context is maintained and a switch-statement is used to return to its previous location in the code.  Since the original shell-code used a switch statement, this was replaced with if-then-else statements.  

The guts of the re-entry method are contained in the following macro **exec()**, which is called whereever the code needs to yield, and saves its context for the next re-entry.  The macro is used for example in the 'while' code: 
```
   } else if (ctx->op == 'w') {    // block( -- flag) -- | execute block while
     ctx->sp = (const char*) pop();
     do {
       **exec(ctx->sp);**
     } while (pop());
     continue;
```

The macro is;
```
#define exec(x) \
    ctx->sctx=(void*)1;\
    do {\
      if (execute(x, &(ctx->sctx)) != NULL) goto error;\
      ctx->ccrLine = __LINE__;\
      return(NULL);\
  case __LINE__:;\
    } while(ctx->sctx);
```
This intitializes a new context to the value 1, calls the shell with this new context via its execute() routine, records the next linenumber into ctx->ccrLine, and defines a new case-statement with that line-number as its label.  

On entry to, or reentry to, the execute() routine, the following code uses the switch-statement to switch to the approprite case.  On first entry, it switches to case 0.  On a re-entrant call, control switched the case-statememt previously saved in the exec() macro above.  The code looks like this:
```
   switch (ctx->ccrLine) {
     case 0:; // first time through
   ...
     case 716:;     // auto-generated in exec() macro
   ...
     case 1123:;    // auto-generated in exec() macro
   ...
```
Where the other cases, such as 716 qnd 1123, are defined in the macro above.  Note that on the first call to execute(), ctx->ccrLine is defined as 1.  This indicates the need for execute() to alloc a new context into ctx and to initialize it, including initializing ctx->ccrLine to 0, as mentioned above, so that the 'switch (ctx->ccrLine)' selects case 0.  On subsequent calls, that intialized context is re-passed into execute(), including the updated ctx->ccrLine to allow the 'switch (ctx->ccrLine)' to jump to the correct case-statement.  

#### Implementing Script Groups
Since some effects will use infinite while-blocks to produce effects on a pin, there has to be a way to stop these effects, or substitute another effect for that pin.  This is done by associating a group-variable, with a group of eventids, that keeps track of which of their associated scripts is active for that pin.  When another eventid in that group is received, it overrides the current script/effect value in its associated group-variable.  


 
