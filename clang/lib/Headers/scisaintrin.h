/*===---- scisaintrin.h - SCISA intrinsics ----------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===------------------------------------------------------------------------===
 */

#ifndef __SCISAINTRIN_H
#define __SCISAINTRIN_H

#ifndef __scisa__
#error "<scisaintrin.h> is for SCISA only"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read data from an I/O port
 * @param addr Pointer to buffer to read into
 * @param size Number of bytes to read
 * @return Number of bytes read
 */
unsigned int __builtin_scisa_in(void *addr, unsigned int size);

/**
 * @brief Write data to an I/O port
 * @param addr Pointer to buffer containing data to write
 * @param size Number of bytes to write
 */
void __builtin_scisa_out(void *addr, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif /* __SCISAINTRIN_H*/
