/* tools/textures.S */
/* Helper file for texture xip linking */

.section .rodata

/* Need alignment for uint32 and efficiency of pico flash accessing */
.align 4

/* C++ symbol */
.global textures_xip_blob

textures_xip_blob:
    /* Note: Path must be relative to where the compiler searches */
    .incbin "textures.xip"
