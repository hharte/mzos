# mzos - Vector Graphic MZOS Disk Utility

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/50fcbfcf3f634e5b90a8b979fda156fa)](https://www.codacy.com/gh/hharte/mzos/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=hharte/mzos&amp;utm_campaign=Badge_Grade)

Vector Graphic own operating system for use with their System B computer systems  These disks have a very simple file system, where every file has to be contiguous.

`mzos` can list and Extract Files from MZOS disk images in `.vgi` and `.img` format.

The filename in MZOS can be up to eight characters.  Each file has an associated file type, from 0-99, although only six are currently defined:


<table>
  <tr>
   <td>Type
   </td>
   <td>Description
   </td>
   <td>Metadata
   </td>
   <td>Output Filename
   </td>
  </tr>
  <tr>
   <td>0
   </td>
   <td>Default
   </td>
   <td>unused
   </td>
   <td>fname.DEFAULT
   </td>
  </tr>
  <tr>
   <td>1
   </td>
   <td>Executable
   </td>
   <td>Load Address
   </td>
   <td>fname.OBJECT_L&lt;load_address>
   </td>
  </tr>
  <tr>
   <td>2
   </td>
   <td>BASIC Program
   </td>
   <td>Actual File Size
   </td>
   <td>fname.BASIC
   </td>
  </tr>
  <tr>
   <td>3
   </td>
   <td>BASIC data file
   </td>
   <td>unused
   </td>
   <td>fname.BASIC_DATA
   </td>
  </tr>
  <tr>
   <td>4
   </td>
   <td>ASCII text
   </td>
   <td>unused
   </td>
   <td>fname.TXT
   </td>
  </tr>
  <tr>
   <td>8
   </td>
   <td>DEX Assembler source file
   </td>
   <td>unused
   </td>
   <td>fname.DEX
   </td>
  </tr>
  <tr>
   <td>nn
   </td>
   <td>Reserved
   </td>
   <td>unused
   </td>
   <td>fname.TYPE_nn
   </td>
  </tr>
</table>


In addition to the file name and type, the MZOS filesystem stores metadata about certain types of files.  For executable object code, the “load address” is stored in the metadata.  To preserve this important information, `mzos` appends the load address to the output filename extension.


# Usage


```
Vector Graphic MZOS File Utility (c) 2022 - Howard M. Harte
https://github.com/hharte/mzos

usage is: mzos <filename.vgi> [command] [<filename>|<path>] [-q]
        <filename.vgi> Vector Graphic Disk Image in .vgi or .img format.
        [command]      LI - List files
                       EX - Extract files to <path>
        Flags:
              -q       Quiet: Don't list file details during extraction.

        If no command is given, LIst is assumed.
```



## To list files in an MZOS disk image:


```
$ mzos <filename.vgi>
```


Or


```
$ mzos <filename.vgi> li
```


For example:


```
$ mzos Vector_Word_Processing_Demonstration_8-56.vgi
Filename  DA BLKS D TYP Type        Metadata
MZOSE      4   34   0 Default       0800,20
VBASIC    38   36   1 Object Code   Load addr: 2A00
MLPROG   161   50   3 BASIC Data    0000,00
TLIST    211   10   7 Reserved      0080,00
MEM.EDIT  84    9   4 ASCII Text    0000,04
MEM.FINL  93    9   4 ASCII Text    0000,07
COUNT    104    2   6 Reserved      0000,20
BROCHUR  106    8   4 ASCII Text    0000,05
LETTER   154    7   4 ASCII Text    0000,07
```



## To Extract All Files from the disk image:


```
$ mzos <filename.vgi> ex <path>
```


For example:


```
$ mzos Vector_Word_Processing_Demonstration_8-56.vgi ex files
   MZOSE -> files\MZOSE.DEFAULT (8704 bytes)
  VBASIC -> files\VBASIC.OBJECT_L2A00 (9216 bytes)
  MLPROG -> files\MLPROG.BASIC_DATA (12800 bytes)
   TLIST -> files\TLIST.TYPE_7 (2560 bytes)
MEM.EDIT -> files\MEM.EDIT.TXT (2304 bytes)
MEM.FINL -> files\MEM.FINL.TXT (2304 bytes)
   COUNT -> files\COUNT.TYPE_6 (512 bytes)
 BROCHUR -> files\BROCHUR.TXT (2048 bytes)
  LETTER -> files\LETTER.TXT (1792 bytes)
Extracted 9 files.
```



# References

[MZOS MZ Operating System v1.4 Manual](http://www.bitsavers.org/pdf/vectorGraphic/mzos/MZOS_1.4_Manual.pdf)

[MZOS 1.5 Disk Images and Guide](https://deramp.com/downloads/vector_graphic/software/disk_images/Micropolis%20controller/)
