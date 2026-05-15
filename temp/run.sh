#!/bin/bash

export QEMU_AUDIO_DRV=none

IMAGE="Image/x64BareBonesImage.qcow2"

if [[ "$1" = "serial" ]]; then
    qemu-system-x86_64 \
        -hda "$IMAGE" \
        -m 512 \
        -rtc base=localtime \
        -nographic \
        -serial mon:stdio
elif [[ "$1" = "gdb" ]]; then
    qemu-system-x86_64 \
        -hda "$IMAGE" \
        -m 512 \
        -rtc base=localtime \
        -d int -s -S
else
    qemu-system-x86_64 \
        -hda "$IMAGE" \
        -m 512 \
        -rtc base=localtime
fi
