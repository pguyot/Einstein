#
# Build the .ntkc module that allows NewtonScript apps to call functions inside Einstein
# 
# You will need mosrun and related tools available in your $PATH. mosrun can be found
# at https://github.com/MatthiasWM/mosrun
#

all: EinsteinGlue.ntkc

clean:
	rm Glue.o
	rm Glue.aif
	rm EinsteinGlue.ntkc

Glue.o: Glue.a
	ARM6asm -g -o Glue.o Glue.a

Glue.aif: Glue.o
	ARMLink -debug -aif -bin -dsuppress -rel -o Glue.aif Glue.o

EinsteinGlue.ntkc: Glue.aif Glue.sym
	AIFtoNTK -via Glue.sym Glue.aif -o EinsteinGlue.ntkc

