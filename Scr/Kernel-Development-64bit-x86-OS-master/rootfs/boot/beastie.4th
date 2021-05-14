\ Copyright (c) 2003 Scott Long <scottl@freebsd.org>
\ Copyright (c) 2003 Aleksander Fafula <alex@fafula.com>
\ Copyright (c) 2006-2011 Devin Teske <devinteske@hotmail.com>
\ All rights reserved.
\ 
\ Redistribution and use in source and binary forms, with or without
\ modification, are permitted provided that the following conditions
\ are met:
\ 1. Redistributions of source code must retain the above copyright
\    notice, this list of conditions and the following disclaimer.
\ 2. Redistributions in binary form must reproduce the above copyright
\    notice, this list of conditions and the following disclaimer in the
\    documentation and/or other materials provided with the distribution.
\ 
\ THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
\ ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
\ IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
\ ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
\ FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
\ DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
\ OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
\ HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
\ LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
\ OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
\ SUCH DAMAGE.
\ 
\ $FreeBSD: release/9.1.0/sys/boot/forth/beastie.4th 222417 2011-05-28 08:50:38Z julian $

marker task-beastie.4th

variable logoX
variable logoY

\ Initialize logo placement to defaults
4 logoX !
4 logoY !

: cse506-logo ( x y -- ) \ CSE506

2dup at-xy ."  " 1+
2dup at-xy ."  " 1+
2dup at-xy ."  " 1+
2dup at-xy ."  [34m" 1+
2dup at-xy ."           _________   ____________________" 1+
2dup at-xy ."           \_   ___ \ /   _____/\_   _____/" 1+
2dup at-xy ."           /    \  \/ \_____  \  |    __)_ " 1+
2dup at-xy ."           \     \____/        \ |        \" 1+
2dup at-xy ."            \______  /_______  //_______  /" 1+
2dup at-xy ."                   \/        \/         \/ " 1+
2dup at-xy ."             ._______________    ________" 1+
2dup at-xy ."             |   ____/\   _  \  /  _____/" 1+
2dup at-xy ."             |____  \ /  /_\  \/   __  \ " 1+
2dup at-xy ."             /       \\  \_/   \  |__\  \" 1+
2dup at-xy ."            /______  / \_____  /\_____  /" 1+
2dup at-xy ."                   \/        \/       \/ " 1+
     at-xy ."  [37m"

	\ Put the cursor back at the bottom
	0 22 at-xy
;

: beastie-logo ( x y -- ) \ color BSD mascot (19 rows x 34 columns)

2dup at-xy ."               [31m,        ," 1+
2dup at-xy ."              /(        )`" 1+
2dup at-xy ."              \ \___   / |" 1+
2dup at-xy ."              /- [37m_[31m  `-/  '" 1+
2dup at-xy ."             ([37m/\/ \[31m \   /\" 1+
2dup at-xy ."             [37m/ /   |[31m `    \" 1+
2dup at-xy ."             [34mO O   [37m) [31m/    |" 1+
2dup at-xy ."             [37m`-^--'[31m`<     '" 1+
2dup at-xy ."            (_.)  _  )   /" 1+
2dup at-xy ."             `.___/`    /" 1+
2dup at-xy ."               `-----' /" 1+
2dup at-xy ."  [33m<----.[31m     __ / __   \" 1+
2dup at-xy ."  [33m<----|====[31mO)))[33m==[31m) \) /[33m====|" 1+
2dup at-xy ."  [33m<----'[31m    `--' `.__,' \" 1+
2dup at-xy ."               |        |" 1+
2dup at-xy ."                \       /       /\" 1+
2dup at-xy ."           [36m______[31m( (_  / \______/" 1+
2dup at-xy ."         [36m,'  ,-----'   |" 1+
     at-xy ."         `--{__________)[37m"

	\ Put the cursor back at the bottom
	0 25 at-xy
;

: orb-logo ( x y -- ) \ color Orb mascot (15 rows x 30 columns)

	3 + \ beastie adjustment (see `fbsdbw-logo' comments above)

	2dup at-xy ."  [31m```                        [31;1m`[31m" 1+
	2dup at-xy ." s` `.....---...[31;1m....--.```   -/[31m" 1+
	2dup at-xy ." +o   .--`         [31;1m/y:`      +.[31m" 1+
	2dup at-xy ."  yo`:.            [31;1m:o      `+-[31m" 1+
	2dup at-xy ."   y/               [31;1m-/`   -o/[31m" 1+
	2dup at-xy ."  .-                  [31;1m::/sy+:.[31m" 1+
	2dup at-xy ."  /                     [31;1m`--  /[31m" 1+
	2dup at-xy ." `:                          [31;1m:`[31m" 1+
	2dup at-xy ." `:                          [31;1m:`[31m" 1+
	2dup at-xy ."  /                          [31;1m/[31m" 1+
	2dup at-xy ."  .-                        [31;1m-.[31m" 1+
	2dup at-xy ."   --                      [31;1m-.[31m" 1+
	2dup at-xy ."    `:`                  [31;1m`:`" 1+
	2dup at-xy ."      [31;1m.--             `--." 1+
	     at-xy ."         .---.....----.[37m"

 	\ Put the cursor back at the bottom
 	0 25 at-xy
;

\ This function draws any number of beastie logos at (loader_logo_x,
\ loader_logo_y) if defined, else (46,4) (to the right of the menu). To choose
\ your beastie, set the variable `loader_logo' to the respective logo name.
\ 
\ Currently available:
\ 
\ 	NAME        DESCRIPTION
\ 	beastie     Color ``Helper Daemon'' mascot (19 rows x 34 columns)
\ 	beastiebw   B/W ``Helper Daemon'' mascot (19 rows x 34 columns)
\ 	fbsdbw      "FreeBSD" logo in B/W (13 rows x 21 columns)
\ 	orb         Color ``Orb'' mascot (15 rows x 30 columns)
\ 	orbbw       B/W ``Orb'' mascot (15 rows x 32 columns) (default)
\ 
\ NOTE: Setting `loader_logo' to an undefined value (such as "none") will
\       prevent beastie from being drawn.
\ 
: draw-beastie ( -- ) \ at (loader_logo_x,loader_logo_y), else (46,4)

	s" loader_logo" getenv dup -1 = if
		drop exit
	then

	2dup s" cse506" compare-insensitive 0= if
		logoX @ logoY @ cse506-logo
		2drop exit
	then
	2dup s" beastie" compare-insensitive 0= if
		logoX @ logoY @ beastie-logo
		2drop exit
	then
	2dup s" orb" compare-insensitive 0= if
		logoX @ logoY @ orb-logo
		2drop exit
	then

	2drop
;
