/* sys_io.c - ARM CORTEX-M Series memory mapped register I/O operations  */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _CORTEX_M_SYS_IO_H_
#define _CORTEX_M_SYS_IO_H_

#if !defined(_ASMLANGUAGE)

#include <sys_io.h>

/* Memory mapped registers I/O functions */

/**
 * @fn static inline uint32_t sys_read32(mem_addr_t addr);
 * @brief Read 32 bits from a memory mapped register
 *
 * This function reads 32 bits from the given memory mapped register.
 *
 * @param addr the memory mapped register address from where to read
 *        the 32 bits
 *
 * @return the 32 bits read
 */

static inline uint32_t sys_read32(mem_addr_t addr)
{
	return *(volatile uint32_t *)addr;
}


/**
 * @fn static inline void sys_write32(uint32_t data, mem_addr_t addr);
 * @brief Write 32 bits to a memory mapped register
 *
 * This function writes 32 bits to the given memory mapped register.
 *
 * @param data the 32 bits to write
 * @param addr the memory mapped register address where to write the 32 bits
 */

static inline void sys_write32(uint32_t data, mem_addr_t addr)
{
	*(volatile uint32_t *)addr = data;
}


/* Memory bit manipulation functions */

/**
 * @fn static inline void sys_set_bit(uint32_t addr, uint32_t bit)
 * @brief Set the designated bit to 1
 *
 * This functions takes the designated bit starting from addr and sets it to 1.
 *
 * @param addr the memory address from where to look for the bit
 * @param bit the designated bit to set (from 0 to 31)
 */

static inline void sys_set_bit(mem_addr_t addr, uint32_t bit)
{
    uint32_t temp = *(volatile uint32_t *)addr;

    /* prevent out of range writes */

    if (bit < 32) {
        *(volatile uint32_t *)addr = temp | (1 << bit);
    }
}

/**
 * @fn static inline void sys_clear_bit(mem_addr_t addr, int bit)
 * @brief Clear the designated bit from addr to 0
 *
 * This functions takes the designated bit starting from addr and sets it to 0.
 *
 * @param addr the memory address from where to look for the bit
 * @param bit the designated bit to clear (from 0 to 31)
 */

static inline void sys_clear_bit(mem_addr_t addr, uint32_t bit)
{
    uint32_t temp = *(volatile uint32_t *)addr;

    /* prevent out of range writes */

    if (bit < 32) {
       *(volatile uint32_t *)addr = temp & ~(1 << bit);
    }
}

#endif /* !_ASMLANGUAGE */

#endif /* _CORTEX_M_SYS_IO_H_ */
