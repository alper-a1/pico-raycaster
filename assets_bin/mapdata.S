/* assets_bin/mapdata.S */
/* Helper file for mapdata xip linking */

.section .rodata

/* Need alignment for uint32 and efficiency of pico flash accessing */
.balign 4

/* C++ symbol */
.global mapdata_xip_blob
.global mapdata_xip_blob_end

textures_xip_blob:
    /* Note: Path must be relative to where the compiler searches */
    .incbin "mapdata.xip"
textures_xip_blob_end: