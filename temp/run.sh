#!/bin/bash

# Set audio device variable
if [ "$(uname)" == "Darwin" ]; then
    audio="coreaudio"
else
    audio="pa"
fi

if [[ "$1" = "gdb" ]]; then
    qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev $audio,id=audio0 -machine pcspk-audiodev=audio0 -rtc base=localtime -d int -s -S
else
    qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev $audio,id=audio0 -machine pcspk-audiodev=audio0 -rtc base=localtime
fi
