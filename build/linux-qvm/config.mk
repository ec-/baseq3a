PK3 = pak8a.pk3

basedir = ../../code

QADIR = $(basedir)/game
CGDIR = $(basedir)/cgame
UIDIR = $(basedir)/q3_ui

Q3ASM = ./tools/q3asm -vq3 -r -m -v
Q3LCC = ./../../tools/q3lcc -DQ3_VM -S -Wf-g -I$(QADIR)
7Z = 7z u -tzip -mx=9 -mpass=8 -mfb=255 --

QA_CFLAGS = -DQAGAME
CG_CFLAGS = -DCGAME -I$(CGDIR)
UI_CFLAGS = -DQ3UI -I$(UIDIR)

include srcs.mk
