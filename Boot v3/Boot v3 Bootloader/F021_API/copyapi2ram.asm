;*****************************************************************************
;
; copyapi2ram.asm
; Author      : Kalpesh M.
; Date        : 26-03-2019
;
;*****************************************************************************


;-------------------------------------------------------------------------------
;
; Copy the Flash API from flash to SRAM.
;

    .def     _copyAPI2RAM_
    .asmfunc

_copyAPI2RAM_

   .ref    api_load
flash_load   .word api_load
   .ref    api_run
flash_run   .word api_run
    .ref    api_size
flash_size  .word api_size

     ldr    r0, flash_load
     ldr    r1, flash_run
     ldr    r2, flash_size
     add    r2, r1, r2
copy_loop1:
     ldr     r3, [r0], #4
     str     r3, [r1], #4
     cmp     r1, r2
     blt     copy_loop1
      bx     lr

    .endasmfunc

