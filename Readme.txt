------------------------------------
            DisPel v1.0
       65816/SMC Disassembler
by James Churchill of Naruto (C)2001
------------------------------------
         10th of July, 2001.

pelrun@gmail.com

Latest windows binaries available to download from:
https://drone.io/github.com/pelrun/Dispel/files

This is DisPel, my 65816 disassembler. It's not nearly as
"full-featured" as Tracer (for instance, it's not really for use with NES
roms), but it does everything I need it to. And since it only took a day to
write (most of which was working on the commandline parser, not the
disassembler itself!) I wonder why I didn't do it ages ago.

Features
--------

Able to disassemble a section of a rom, or a whole bank. Or all of it :)
Correct REP/SEP instruction handling.
Correct bank-boundary handling.
Automatic (but overridable) HiROM and LoROM support.
Shadow ROM support.
User-specifiable listing origin (see below.)
True SNES addressing - no need to worry about LoROM-offset conversion or
   headers.
Control-C *will* stop it!


True SNES addressing
--------------------

Now you no longer need to worry about converting to-from LoROM addresses to
disassemble the code you want - just enter the SNES bank/addresses you want,
and DisPel does the conversion automatically. If the ROM has an SMC header
then skip it with the "-n" option.

And now that you can select any section of the rom to disassemble, you don't
have to worry about a 20MB+ file containing "disassembled" graphics data,
which is just a waste of space.

Addresses/banks are specified using plain hex - no $ or 0x prefixes added.

HiROM addressing is switched on using the "-h" option. Use inSNESt or a
similar utility to determine whether it's needed.

v0.91 update: I've replaced the half-assed HiROM support with a newer half-assed
 version. Specifying HiROM addresses (bank $40-$6F and $C0-$EF) will switch HiROM on
 automatically, and HiROM listings use the correct banks now (instead of bank $00+).

v0.95 update: HiROM is now detected automatically. If DisPel gets it wrong, you can use
 the -h/-l options to force HiROM/LoROM modeS respectively.


You can disassemble a single bank using the "-b" option.

e.g.

dispel -b 1D rom.bin
 Will disassemble from $1D8000 to $1DFFFF.

dispel -b 1D -h rom.bin
 Will disassemble from $1D0000 to $1DFFFF.

Address ranges can be specified using the "-r" option. If the end address is
omitted, the disassembly will proceed from the supplied address to the end
of the file.

e.g.

dispel -r 58600-79700 rom.bin
 Will disassemble the range $58600 to $79700... only the valid LoROM banks,
 of course :)

dispel -r 58600 rom.bin
 Will disassemble from $58600 onwards.


Correct REP/SEP state parsing
-----------------------------

As a byproduct of the need to maintain backward compatibility with the 6502,
certain opcodes take either a 1-byte or a 2-byte operand. Which is used is
dictated by processor state flags, and so is software selectable. This is a
problem with disassembly because normally the disassembler does not know
which version of the instruction to use. This leads to the instruction
alignment being lost and gibberish instructions being output.

Tracer tried to address the problem by maintaining similar state flags, and
updating them when an instruction that would normally alter them is
disassembled. Unfortunately, it didn't do it correctly for all the
instructions that are affected. DisPel however should produce a correct
listing for a subroutine when the state is set properly at the start.

Since most 65816 code I've seen explicitly sets the size flags at the
beginning of each subroutine, you needn't worry about it a lot of the time.
However, if you are disassembling a small block where the flags are set
differently at the beginning, you can affect the initial state of the flags
using the "-a" and "-x" options.

"-a" forces 8-bit accumulator mode.
"-x" forces 8-bit X/Y register mode.

DisPel defaults to 16-bit accumulator and X/Y register mode.

Of course, this only applies at the beginning of the listing. Subsequent REP
and SEP commands will alter the states appropriately.


Bank-boundary handling
----------------------

SNES code doesn't run across bank boundaries. It might in a HiROM (though 
I doubt it), but never ever in a LoROM image. So the disassembler shouldn't
place instructions that straddle the bank boundary. If that happens, then the
instruction alignment is definitely compromised, as one or more bytes are
"eaten" at the start of the next bank that shouldn't be. This results in an
incorrect listing at the start of the bank, which continues until the
disassembler (through pure luck) re-establishes the alignment.

This was of great concern in Tracer, which had woeful support for
disassembling sections of a ROM. If it didn't get it right (and you were
bound to have some banks that started like this) then replacing them with
a correct listing was a real pain.

So in DisPel I enforced the bank boundaries. If an disassembled instruction
would go over a boundary then it is ignored. The surplus bytes up to the
boundary are displayed without an accompanying instruction, and disassembly
continues at the boundary onwards.

Just in case I'm completely wrong about the boundary-crossing on HiROMs, you
can disable the enforcement using the -d option. But I suggest leaving it on
to start with.

Shadow ROM support
------------------

Shadow ROM is a feature of the SNES hardware - the entire rom image is
mirrored at bank 80 onwards. When game code executes in those banks the
hardware runs at a higher clock-rate than when it's running at the lower
copy. (I think it's called FastROM and SlowROM in other documentation - for
obvious reasons. Note I'm not talking about Fast/SlowROM *protection* here.)

65816 code is largely relocatable. However, long absolute addressing modes
mean that most code *must* be run where it was assembled to run - it's
origin. Therefore FastROM code won't work properly if attempted to be
executed in the SlowROM banks and vice versa. Unfortunately, when you
disassemble FastROM game code with the other tools, you get a listing based
in the SlowROM banks, which gives you a very inconsistent view of the code -
relative addresses point at expected places, but long jumps and the like all
go somewhere completely unexpected!

If you know what's going on, there's no problem. But it's still a hassle to
have to worry about it. Therefore DisPel has the ability to produce a listing
using the FastROM addresses. You'll know if you need it if you see JMP's
going to addresses above $800000.

For HiROMs, the SlowROM code begins at bank $40, and the FastROM copy at $C0.

To turn it on, either specify the address range/bank in the $80xxxx+ area
directly, or use the -s option to convert the specified addresses to FastROM
ones.

v0.95 update: FastROM is detected automatically. You can force enable/disable it
 by using the -s and -i options.

e.g.

dispel -b 02 -s rom.bin
 Will disassemble bank 02 as bank 82.

dispel -b 82 rom.bin
 Identical to above.

dispel -r 20000-2FFFF -s rom.bin
 Will disassemble the region $20000-$2FFFF as $820000-$82FFFF.

dispel -r 820000-82FFFF rom.bin
 Same as above.


User-specified origin support
-----------------------------

This is what I first meant to use to implement Shadow ROM support. I did it
better above, but I decided to leave this in in case someone might find it
useful.

Not all SNES code is run where it is in the rom. Some of it is copied
elsewhere then executed at the new location (the sound code does this, but
that's a different discussion.) Such won't work at it's original location -
the code is assembled to run somewhere else, and all absolute addresses will
point to incorrect places. If you ever encounter something like this, the
"-g" option will force DisPel to assume the code is assembled somewhere other
than it's actual location. Relatively addressed operands will be altered to
fit.

You shouldn't need this. If you do, you'll probably understand what this
does anyway. Otherwise, don't worry about it.

e.g.

dispel -b 0 -g 30000 rom.bin

Disassemble LoROM bank 0 ($8000-$FFFF) as if it was really at $30000-$37FFF.


Miscellaneous
-------------

You can output only the instructions (no address/hexdump fields) using the -t option.

You can also place blank lines after RTS,RTI,RTL instructions using the -p option.
This will help show where the subroutines start/end in the code.


Usage
-----

dispel [-n] [-t] [-h] [-l] [-s] [-i] [-a] [-x] [-e] [-p]
              [-b <bank>|-r <startaddr>-<endaddr>] [-g <origin>]
              [-d <width>] [-o <outfile>] <infile>
Options: (numbers are hex-only, no prefixes)
 -n                Skip $200 byte SMC header
 -t                Don't output addresses/hex dump.
 -h/-l             Force HiROM/LoROM memory mapping.
 -s/-i             Force enable/disable shadow ROM addresses (see readme.)
 -a                Start in 8-bit accumulator mode. Default is 16-bit.
 -x                Start in 8-bit X/Y mode. Default is 16-bit.
 -e                Turn off bank-boundary enforcement. (see readme.)
 -p                Split subroutines by placing blank lines after RTS,RTL,RTI
 -b <bank>         Disassemble bank <bank> only. Overrides -r.
 -r <start>-<end>  Disassemble block from <start> to <end>.
                     Omit -<end> to disassemble to end of file.
 -g <origin>       Set origin of disassembled code (see readme.)
 -d <width>        No disassembly - produce a hexdump with <width> bytes/line.
 -o <outfile>      Set file to redirect output to. Default is stdout.
 <infile>          File to disassemble.


Release history
---------------

v1.0001 - 5/4/2011
No code changes; put the source into roughly the code style I use nowadays
before making the source public, and changed from a VC5 project to GCC/Make.

v1.00 - 10/7/2002
Fixed a stupid bug that made DisPel crash if the input file didn't exist.
Lowercased the mnemonics.
Changed the parameter format for BRK and COP



v0.99 - 10/7/2001
Fixed a couple of errors that prevented the 8-bit register options from working,
as well as the shadow/hirom support.
Also fixed a small error in this document; -b overrides -r, not -a.

v0.96 - 3/2/2001
Turns out I was erroneously outputting ",X" for the
"Direct Page Indirect Indexed, Y" and
"Direct Page Indirect Long Indexed, Y" opcodes.
That's 16 opcodes with incorrect output - Ouch.
Thanks to Skeud for pointing it out.

v0.95 - 19/11/2000
Autodetects HiROM/LoROM and FastROM/SlowROM modes.
Added option to put blank lines after RTS,RTL,RTI instructions
Added option to output raw assembly (minus address/hex data fields)

v0.91 - 25/09/2000
Fixed HiROM addressing to start from bank $40 instead of $00.
Serves me right for not looking up the HiROM docs...

v0.9 - 24/09/2000
Initial Release
