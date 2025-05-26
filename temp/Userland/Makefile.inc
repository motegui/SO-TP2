GCC=gcc
GPP=g++
LD=ld
AR=ar
ASM=nasm

GCCFLAGS= -fno-exceptions -std=c99 -Wall -ffreestanding -nostdlib -fno-common   -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc -g
ARFLAGS=rvs
ASMFLAGS=-felf64
