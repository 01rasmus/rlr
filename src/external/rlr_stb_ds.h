#pragma once
/*
    rlr_render is supposed to be used as a library,
    therefore all the internal functions that are
    used by stb_ds.h need to be renamed to avoid
    collisions with users who also want to use stb_ds.h

    use the following to figure out what function names
    that need to be renamed:

    nm -g --defined-only path/to/stb_ds.c.o

    (make sure to run this command again when updating
    stb_ds.h so make sure the function names are renamed
    correctly)
*/
#define stbds_arrfreef       rlr_external_stbds_arrfreef
#define stbds_arrgrowf       rlr_external_stbds_arrgrowf
#define stbds_hash_bytes     rlr_external_stbds_hash_bytes
#define stbds_hash_string    rlr_external_stbds_hash_string
#define stbds_hmdel_key      rlr_external_stbds_hmdel_key
#define stbds_hmfree_func    rlr_external_stbds_hmfree_func
#define stbds_hmget_key      rlr_external_stbds_hmget_key
#define stbds_hmget_key_ts   rlr_external_stbds_hmget_key_ts
#define stbds_hmput_default  rlr_external_stbds_hmput_default
#define stbds_hmput_key      rlr_external_stbds_hmput_key
#define stbds_rand_seed      rlr_external_stbds_rand_seed
#define stbds_shmode_func    rlr_external_stbds_shmode_func
#define stbds_stralloc       rlr_external_stbds_stralloc
#define stbds_strreset       rlr_external_stbds_strreset
#include <stb_ds.h>