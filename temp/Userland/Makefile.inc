GCC=gcc
GPP=g++
LD=ld
AR=ar
ASM=nasm

GCCFLAGS=-m64 -mno-red-zone -fno-exceptions -std=c99 -Wall -ffreestanding -nostdlib -fno-common   -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc -g -fno-pie

ARFLAGS=rvs
ASMFLAGS=-felf64
