/*
 * Utility to list directory contents of Vector Graphic MZOS floppy disks,
 * and optionally extract the files.
 *
 * www.github.com/hharte/mzos
 *
 * Copyright (c) 2022, Howard M. Harte
 *
 * Reference: MZOS MZ Operating System 1.4 by Vector Graphic Inc.
 *
 */

#define _CRT_SECURE_NO_DEPRECATE

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "./mzos.h"

const char kPathSeparator =
#ifdef _WIN32
    '\\';
#else  /* ifdef _WIN32 */
    '/';
#endif /* ifdef _WIN32 */

const char *file_type_str[] = {
    "Default      ",
    "Object Code  ",
    "BASIC Program",
    "BASIC Data   ",
    "ASCII Text   ",
    "Reserved     ",
    "Reserved     ",
    "Reserved     ",
    "DEX Asm src  ",
    "Reserved     ",
};

int main(int argc, char *argv[]) {
    FILE *instream;
    mz_dir_entry_t *dir_entry_list;
    mzos_args_t     args;
    int positional_arg_cnt;
    int dir_entry_cnt;
    int i;
    int result;
    int extracted_file_count = 0;
    int status               = 0;

    positional_arg_cnt = parse_args(argc, argv, &args);

    args.vgi = 0;

    if (positional_arg_cnt == 0) {
        printf("Vector Graphic MZOS File Utility (c) 2022 - Howard M. Harte\n");
        printf("https://github.com/hharte/mzos\n\n");

        printf("usage is: %s <filename.vgi> [command] [<filename>|<path>] [-q]\n", argv[0]);
        printf("\t<filename.vgi> Vector Graphic Disk Image in .vgi or .img format.\n");
        printf("\t[command]      LI - List files\n");
        printf("\t               EX - Extract files to <path>\n");
        printf("\tFlags:\n");
        printf("\t      -q       Quiet: Don't list file details during extraction.\n");
        printf("\n\tIf no command is given, LIst is assumed.\n");
        return -1;
    }

    args.image_filename[sizeof(args.image_filename) - 1] = '\0';

    if (strncasecmp(&args.image_filename[strlen(args.image_filename) - 4], ".vgi", 4) == 0) {
        args.vgi = 1;
    }

    if (!(instream = fopen(args.image_filename, "rb"))) {
        fprintf(stderr, "Error Openening %s\n", argv[1]);
        return -ENOENT;
    }

    dir_entry_list = (mz_dir_entry_t *)calloc(DIR_ENTRIES_MAX, sizeof(mz_dir_entry_t));

    if (dir_entry_list == NULL) {
        fprintf(stderr, "Memory allocation of %d bytes failed\n", (int)(DIR_ENTRIES_MAX * sizeof(mz_dir_entry_t)));
        status = -ENOMEM;
        goto exit_main;
    }

    dir_entry_cnt = mz_read_sectors(instream, 0, 4, (uint8_t *)dir_entry_list, args.vgi);

    if (dir_entry_cnt == 0) {
        fprintf(stderr, "File not found\n");
        status = -ENOENT;
        goto exit_main;
    }

    /* Parse the command, and perform the requested action. */
    if ((positional_arg_cnt == 1) | (!strncasecmp(args.operation, "LI", 2))) {
        printf("Filename  DA BLKS D TYP Type        Metadata\n");

        for (i = 0; i < DIR_ENTRIES_MAX; i++) {
            mz_list_dir_entry(&dir_entry_list[i]);
        }
    } else {
        if (positional_arg_cnt < 2) {
            fprintf(stderr, "filename required.\n");
            status = -EBADF;
            goto exit_main;
        } else if (!strncasecmp(args.operation, "EX", 2)) {
            for (i = 0; i < DIR_ENTRIES_MAX; i++) {
                if (!strncmp(dir_entry_list[i].sname, "        ", SNAME_LEN)) {
                    continue;
                }

                if (dir_entry_list[i].disk_address > MZ_SECTORS_MAX) {
                    printf("Invalid disk address %d, skipping extraction.\n", dir_entry_list[i].disk_address);
                    continue;
                }

                if (dir_entry_list[i].block_count > MZ_SECTORS_MAX) {
                    printf("Invalid block count %d, skipping extraction.\n", dir_entry_list[i].disk_address);
                    continue;
                }

                result = mz_extract_file(&dir_entry_list[i], instream, args.output_path, args.quiet, args.vgi);

                if (result == 0) {
                    extracted_file_count++;
                }
            }
            printf("Extracted %d files.\n", extracted_file_count);
        }
    }

exit_main:

    if (dir_entry_list) free(dir_entry_list);

    if (instream != NULL) fclose(instream);

    return status;
}

int parse_args(int argc, char *argv[], mzos_args_t *args) {
    int positional_arg_cnt = 0;

    memset(args, 0, sizeof(mzos_args_t));

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            switch (positional_arg_cnt) {
                case 0:
                    snprintf(args->image_filename, sizeof(args->image_filename), "%s", argv[i]);
                    break;
                case 1:
                    snprintf(args->operation,      sizeof(args->operation),      "%s", argv[i]);
                    break;
                case 2:
                    snprintf(args->output_path,    sizeof(args->output_path),    "%s", argv[i]);
                    break;
            }
            positional_arg_cnt++;
        } else {
            char flag = argv[i][1];

            switch (flag) {
                case 'f':
                    args->force = 1;
                    break;
                case 'q':
                    args->quiet = 1;
                    break;
                default:
                    printf("Unknown option '-%c'\n", flag);
                    break;
            }
        }
    }
    return positional_arg_cnt;
}

void mz_list_dir_entry(mz_dir_entry_t *dir_entry) {
    char fname[SNAME_LEN + 1];

    snprintf(fname, sizeof(fname), "%s", dir_entry->sname);

    if (strncmp(fname, "        ", SNAME_LEN)) {
        uint8_t file_type;

        file_type = dir_entry->file_type & 0x7f;

        printf("%s %3d  %3d %3d ", fname, dir_entry->disk_address, dir_entry->block_count, file_type);

        if (file_type < (sizeof(file_type_str) / sizeof(file_type_str[0]))) {
            printf("%s", file_type_str[file_type]);
        } else {
            printf("(Unknown)    ");
        }

        switch (file_type) {
            case FILE_TYPE_OBJECT:
                /* Binary object file, loaded with "GO," type-dependent data
                   contains the load address */
                printf(" Load addr: %04X\n", dir_entry->start);
                break;
            case FILE_TYPE_BASIC_SRC:
                /* BASIC Source code, type-dependent data contains the actual
                   program size in blocks */
                printf(" Actual Size: %d\n", dir_entry->start);
                break;
            default:
                printf(" %04X,%02X\n",       dir_entry->start, dir_entry->special);
                break;
        }
    }
}

int mz_extract_file(mz_dir_entry_t *dir_entry, FILE *instream, char *path, int quiet, int vgi) {
    uint8_t file_type;
    char    dos_fname[SNAME_LEN + 1];
    char    fname[SNAME_LEN + 1];

    snprintf(dos_fname, sizeof(dos_fname), "%s", dir_entry->sname);

    if (!strncmp(dos_fname, "        ", SNAME_LEN)) {
        return -ENOENT;
    }

    /* Truncate the filename if a space is encountered. */
    for (unsigned int j = 0; j < strnlen(dos_fname, sizeof(dos_fname)); j++) {
        if (dos_fname[j] == ' ') dos_fname[j] = '\0';
    }

    snprintf(fname, sizeof(fname), "%s", dos_fname);

    /* Replace '/' with '-' in output filename. */
    char *current_pos = strchr(fname, '/');

    while (current_pos) {
        *current_pos = '-';
        current_pos  = strchr(current_pos, '/');
    }

    /* Replace '*' with 's' in output filename. */
    current_pos = strchr(fname, '*');

    while (current_pos) {
        *current_pos = 's';
        current_pos  = strchr(current_pos, '*');
    }

    file_type = dir_entry->file_type & 0x7f;

    FILE *ostream;
    uint8_t *file_buf;
    int  file_len;
    char output_filename[256];

    file_len = dir_entry->block_count * MZ_BLOCK_SIZE;

    switch (file_type) {
        case FILE_TYPE_DEFAULT:
            snprintf(output_filename, sizeof(output_filename), "%s%c%s.DEFAULT",      path, kPathSeparator, fname);
            break;
        case FILE_TYPE_OBJECT:
            snprintf(output_filename, sizeof(output_filename), "%s%c%s.OBJECT_L%04X", path, kPathSeparator, fname,
                     dir_entry->start);
            break;
        case FILE_TYPE_BASIC_SRC:
            file_len = dir_entry->start;
            snprintf(output_filename, sizeof(output_filename), "%s%c%s.BASIC",      path, kPathSeparator, fname);
            break;
        case FILE_TYPE_BASIC_DATA:
            snprintf(output_filename, sizeof(output_filename), "%s%c%s.BASIC_DATA", path, kPathSeparator, fname);
            break;
        case FILE_TYPE_ASCII_TEXT:
            snprintf(output_filename, sizeof(output_filename), "%s%c%s.TXT",        path, kPathSeparator, fname);
            break;
        case FILE_TYPE_DEX_ASM_SRC:
            snprintf(output_filename, sizeof(output_filename), "%s%c%s.DEX",        path, kPathSeparator, fname);
            break;
        case FILE_TYPE_RESERVED5:
        case FILE_TYPE_RESERVED6:
        case FILE_TYPE_RESERVED7:
        case FILE_TYPE_RESERVED9:
        default:
            snprintf(output_filename, sizeof(output_filename), "%s%c%s.TYPE_%d", path, kPathSeparator, fname,
                     file_type);
            break;
    }

    output_filename[sizeof(output_filename) - 1] = '\0';

    if (!(ostream = fopen(output_filename, "wb"))) {
        printf("Error Openening %s\n", output_filename);
        return -ENOENT;
    } else if ((file_buf = (uint8_t *)calloc(1, dir_entry->block_count * MZ_BLOCK_SIZE))) {
        if (!quiet) printf("%8s -> %s (%d bytes)\n", dos_fname, output_filename, file_len);

        mz_read_sectors(instream, dir_entry->disk_address, dir_entry->block_count, file_buf, vgi);
        fwrite(file_buf, file_len, 1, ostream);
        free(file_buf);
        fclose(ostream);
        return 0;
    }

    printf("Memory allocation of %d bytes failed\n", file_len);
    fclose(ostream);
    return -ENOMEM;
}

/* MZOS has a sector skew as follows: */
const uint8_t skew_map[MZ_SECTORS_PER_TRACK] = { 0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15 };

/* Read sectors from the disk image (either .img or .vgi) following the MZOS
   sector skew. */
int mz_read_sectors(FILE *stream, uint16_t start_sector, uint16_t count, uint8_t *buffer, uint8_t vgi) {
    uint8_t *bufptr     = buffer;
    int sector_size     = vgi ? VGI_SECTOR_LEN : MZ_BLOCK_SIZE;
    size_t sectors_read = 0;
    int    end_sector   = start_sector + count;

    if (end_sector > MZ_SECTORS_MAX) {
        printf("Error: Sector %d out of range (%d max.)\n", end_sector, MZ_SECTORS_MAX);
    }

    for (int sector = start_sector; sector < end_sector; sector++) {
        uint32_t file_offset = (sector / MZ_SECTORS_PER_TRACK) * (sector_size * MZ_SECTORS_PER_TRACK);
        file_offset += sector_size * skew_map[sector & 0x0f];

        if (sector_size == VGI_SECTOR_LEN) {
            file_offset += VGI_HEADER_LEN; /* Skip over VGI sector header */
        }

        if (0 != fseek(stream, file_offset, SEEK_SET)) {
            return 0;
        }

        sectors_read += fread(bufptr, MZ_BLOCK_SIZE, 1, stream);
        bufptr       += MZ_BLOCK_SIZE;
    }

    return (int)sectors_read;
}
