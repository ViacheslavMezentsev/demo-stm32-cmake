#Generates binary file and copies it with name <TARGET>.elf.bin
function(stm32_generate_elf_bin TARGET)
    stm32_generate_binary_file(${TARGET})
    file(COPY_FILE "${CMAKE_BINARY_DIR}/${TARGET}.bin" "${CMAKE_BINARY_DIR}/${TARGET}.elf.bin" RESULT COPY_ERROR)
    if(NOT COPY_ERROR EQUAL 0)
        message("Error copying .bin file: ${COPY_ERROR}")
    endif()
endfunction()

#Generates hex file and copies it with name <TARGET>.elf.hex
function(stm32_generate_elf_hex TARGET)
    stm32_generate_hex_file(${TARGET})
    file(COPY_FILE "${CMAKE_BINARY_DIR}/${TARGET}.hex" "${CMAKE_BINARY_DIR}/${TARGET}.elf.hex" RESULT COPY_ERROR)
    if(NOT COPY_ERROR EQUAL 0)
        message("Error copying .bin file: ${COPY_ERROR}")
    endif()
endfunction()

if(NOT (TARGET STM32::Semihosting))
    add_library(STM32::Semihosting INTERFACE IMPORTED)
    target_link_options(STM32::Semihosting INTERFACE -lrdimon $<$<C_COMPILER_ID:GNU>:--specs=rdimon.specs>)
endif()
