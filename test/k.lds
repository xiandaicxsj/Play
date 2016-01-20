SECTIONS  
{  
        .text 0x1000: { *(.text) }  
        .data 0x3000: AT(ADDR(.text) +SIZEOF(.text)) { *(.data) }  
}  

