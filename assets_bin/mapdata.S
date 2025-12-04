/* assets_bin/mapdata.S */
/* Helper file for mapdata xip linking */

.section .rodata

/* Need alignment for uint32 and efficiency of pico flash accessing */
.balign 4

/* C++ symbol */
.global map_data_xip_blob
.global map_data_xip_blob_end

map_data_xip_blob:
    /* Note: Path must be relative to where the compiler searches */
    .incbin "mapdata.xip"
map_data_xip_blob_end: