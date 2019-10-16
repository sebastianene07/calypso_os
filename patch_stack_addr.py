#!/usr/bin/env python

import os
import sys

READELF=sys.argv[1]

# Determine the STACK TOP address

get_symbol_cmd = "nm build.elf | grep \"_estack\""
symbol_dictionary = os.popen(get_symbol_cmd).read().split(" ")
print("Found _estack at: 0x" + symbol_dictionary[0])
stack_top_str = "0x"+symbol_dictionary[0]
stack_top = int(stack_top_str, 16)

# Determine the locatiion where we patch the stack address in the isr_vector section

find_isr_section_addr_cmd = READELF + " -S build.elf | grep \"isr_vector\""
section_addr_dictionary = os.popen(find_isr_section_addr_cmd).read().split("PROGBITS")[1].strip().split(" ")
print("Found .isr_vector address at:0x" + section_addr_dictionary[1] + " offset in elf")

# Patch the stack address in the first 4 bytes found from the isr_vector

elf_file = open("build.elf", "r+b")

seek_offset_str = "0x"+section_addr_dictionary[1];
seek_offset = int(seek_offset_str, 16)
#elf_file.seek(seek_offset, 0)
#old_stack_bytes = elf_file.read(4)

#old_stack = int.from_bytes(old_stack_bytes, byteorder='little', signed=False)
#print("Seek :" + hex(seek_offset))
elf_file.seek(seek_offset, 0)
elf_file.write(stack_top.to_bytes(4, byteorder='little', signed=False))
elf_file.close()

print("Patching the elf done (^ - ^) \n");
