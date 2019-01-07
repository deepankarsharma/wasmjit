/* -*-mode:c; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
  Copyright (c) 2018,2019 Rian Hunter et. al, see AUTHORS file.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
 */

#ifndef __WASMJIT__SYS_H__
#define __WASMJIT__SYS_H__

#include <stddef.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <setjmp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdatomic.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef sigjmp_buf wasmjit_thread_state;
#define wasmjit_save_thread_state(s) setjmp((s))
#define wasmjit_restore_thread_state(s, d) longjmp((s), (d))

#define RESTRICT_ATTR restrict

#define wasmjit_warning(fmt, ...) fprintf(stderr, "warning: wasmjit: " fmt "\n", __VA_ARGS__)

typedef atomic_ulong wasmjit_atomic_unsigned_long;

__attribute__((unused))
static bool test_bit(long nr, volatile wasmjit_atomic_unsigned_long *addr)
{
	unsigned long val;
	val = atomic_load_explicit(addr, memory_order_acquire);
	return !!(val & (((unsigned long) 1) << nr));
}

__attribute__((unused))
static void set_bit(long nr, volatile wasmjit_atomic_unsigned_long *addr)
{
	unsigned long val, newval;

	do {
		val = atomic_load_explicit(addr, memory_order_acquire);
		newval = val | (((unsigned long) 1) << nr);
	} while (!atomic_compare_exchange_weak_explicit(addr, &val, newval,
							memory_order_release,
							memory_order_relaxed));
}

__attribute__((unused))
static void clear_bit(long nr, volatile wasmjit_atomic_unsigned_long *addr)
{
	unsigned long val, newval;

	do {
		val = atomic_load_explicit(addr, memory_order_acquire);
		newval = val & (~((unsigned long) 1) << nr);
	} while (!atomic_compare_exchange_weak_explicit(addr, &val, newval,
							memory_order_release,
							memory_order_relaxed));
}

void *wasmjit_map_code_segment(size_t code_size);
int wasmjit_mark_code_segment_executable(void *code, size_t code_size);
int wasmjit_unmap_code_segment(void *code, size_t code_size);

char *wasmjit_load_file(const char *filename, size_t *size);
void wasmjit_unload_file(char *buf, size_t size);

int wasmjit_set_stack_top(void *stack_top);
void *wasmjit_stack_top(void);

int wasmjit_set_jmp_buf(wasmjit_thread_state *jmpbuf);
wasmjit_thread_state *wasmjit_get_jmp_buf(void);

#ifdef __cplusplus
}
#endif

#endif
