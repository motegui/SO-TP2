#!/bin/bash

export QEMU_AUDIO_DRV=none

# Set audio device variable

# if [ "$(uname)" == "Darwin" ]; then
#     audio="coreaudio"
# else
#     audio="pa"
# fi



# if [[ "$1" = "gdb" ]]; then
#     qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -rtc base=localtime -d int -s -S
# else
#     qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -rtc base=localtime
# fi

#!/bin/bash

# Desactiva el backend de audio de QEMU
export QEMU_AUDIO_DRV=none

if [[ "$1" = "gdb" ]]; then
    qemu-system-x86_64 \
        -hda Image/x64BareBonesImage.qcow2 \
        -m 512 \
        -rtc base=localtime \
        -nographic \
        -serial mon:stdio \
        -d int -s -S
else
    qemu-system-x86_64 \
        -hda Image/x64BareBonesImage.qcow2 \
        -m 512 \
        -rtc base=localtime \
        -nographic \
        -serial mon:stdio
fi