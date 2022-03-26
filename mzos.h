/*
 * Vector Graphic MZOS filesystem definitions.
 *
 * www.github.com/hharte/mzos
 *
 * Copyright (c) 2022, Howard M. Harte
 *
 * Reference: MZOS MZ Operating System 1.4 by Vector Graphic Inc.
 *
 */

#ifndef MZOS_H_
#define MZOS_H_

#include <stdio.h>
#include <stdint.h>

#define MZ_TRACKS               (77)
#define MZ_SECTORS_PER_TRACK    (16)
#define MZ_SECTORS_MAX          (MZ_TRACKS * MZ_SECTORS_PER_TRACK)
#define MZ_BLOCK_SIZE           (256)
#define VGI_SECTOR_LEN          (275)
#define VGI_HEADER_LEN          (1 + 2 + 10)
#define DIR_ENTRIES_PER_BLK     (16)
#define DIR_ENTRIES_MAX         (DIR_ENTRIES_PER_BLK * 4)
#define SNAME_LEN               (8)

#define FILE_TYPE_DEFAULT       (0)
#define FILE_TYPE_OBJECT        (1)
#define FILE_TYPE_BASIC_SRC     (2)
#define FILE_TYPE_BASIC_DATA    (3)
#define FILE_TYPE_ASCII_TEXT    (4)
#define FILE_TYPE_RESERVED5     (5)
#define FILE_TYPE_RESERVED6     (6)
#define FILE_TYPE_RESERVED7     (7)
#define FILE_TYPE_DEX_ASM_SRC   (8)
#define FILE_TYPE_RESERVED9     (9)

#pragma pack(push, 1)

/* MZOS Directory Entry */
typedef struct mz_dir_entry {
    char     sname[SNAME_LEN];
    uint16_t disk_address;
    uint16_t block_count;
    uint8_t  file_type;
    uint16_t start;
    uint8_t  special;
} mz_dir_entry_t;
#pragma pack(pop)

typedef struct mzos_args {
    char image_filename[256];
    char output_path[256];
    char operation[8];
    int  force;
    int  quiet;
    int  vgi;
} mzos_args_t;

/* Function prototypes */
int  parse_args(int argc, char *argv[], mzos_args_t *args);
void mz_list_dir_entry(mz_dir_entry_t *dir_entry);
int  mz_extract_file(mz_dir_entry_t *dir_entry, FILE *instream, char *path, int quiet, int vgi);
int  mz_read_sectors(FILE *stream, uint16_t start_sector, uint16_t count, uint8_t *buffer, uint8_t vgi);

#if defined(_WIN32)
# define strncasecmp(x, y, z) _strnicmp(x, y, z)
#endif /* if defined(_WIN32) */

#endif /* MZOS_H_ */
