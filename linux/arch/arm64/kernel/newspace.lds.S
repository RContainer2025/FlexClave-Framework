/*
 * Linker script for the custom 'newspace' segment.
 *
 * VMA: to be done
 * LMA: Calculated relative to the physical start of the kernel.
 */

. = ALIGN(PAGE_SIZE);

.newspace : AT(ADDR(_end))
{
    __newspace_start = .;
    *(.newspace)
    . = . + UDF_SEGMENT_LENGTH; // Adjust the size as needed
    __newspace_end = .;
}