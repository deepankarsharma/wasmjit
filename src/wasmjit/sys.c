/* -*-mode:c; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
  Copyright (c) 2019 Rian Hunter et. al, see AUTHORS file.

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

#include <wasmjit/sys.h>
#include <wasmjit/tls.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

void *wasmjit_map_code_segment(size_t code_size)
{
	void *newcode;
	newcode = mmap(NULL, code_size, PROT_READ | PROT_WRITE,
		       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (newcode == MAP_FAILED)
		return NULL;
	return newcode;
}

int wasmjit_mark_code_segment_executable(void *code, size_t code_size)
{
	return !mprotect(code, code_size, PROT_READ | PROT_EXEC);
}


int wasmjit_unmap_code_segment(void *code, size_t code_size)
{
	return !munmap(code, code_size);
}

wasmjit_tls_key_t jmp_buf_key;

__attribute__((constructor))
static void _init_jmp_buf(void)
{
	wasmjit_init_tls_key(&jmp_buf_key, NULL);
}

wasmjit_thread_state *wasmjit_get_jmp_buf(void)
{
	wasmjit_thread_state *toret;
	int ret;
	ret = wasmjit_get_tls_key(jmp_buf_key, &toret);
	if (!ret) return NULL;
	return toret;
}

int wasmjit_set_jmp_buf(wasmjit_thread_state *jmpbuf)
{
	return wasmjit_set_tls_key(jmp_buf_key, jmpbuf);
}

wasmjit_tls_key_t stack_top_key;

__attribute__((constructor))
static void _init_stack_top(void)
{
	wasmjit_init_tls_key(&stack_top_key, NULL);
}

void *wasmjit_stack_top(void)
{
	void *toret;
	int ret;
	ret = wasmjit_get_tls_key(stack_top_key, &toret);
	if (!ret) return NULL;
	return toret;
}

int wasmjit_set_stack_top(void *stack_top)
{
	return wasmjit_set_tls_key(stack_top_key, stack_top);
}

char *wasmjit_load_file(const char *filename, size_t *size)
{
	FILE *f = NULL;
	char *input = NULL;
	int fd = -1, ret;
	struct stat st;
	size_t rets;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		goto error_exit;
	}

	ret = fstat(fd, &st);
	if (ret < 0) {
		goto error_exit;
	}

	f = fdopen(fd, "r");
	if (!f) {
		goto error_exit;
	}
	fd = -1;

	*size = st.st_size;
	input = malloc(st.st_size);
	if (!input) {
		goto error_exit;
	}

	rets = fread(input, sizeof(char), st.st_size, f);
	if (rets != (size_t) st.st_size) {
		goto error_exit;
	}

	goto success_exit;

 error_exit:
	if (input) {
		free(input);
	}

 success_exit:
	if (f) {
		fclose(f);
	}

	if (fd >= 0) {
		close(fd);
	}

	return input;
}

void wasmjit_unload_file(char *buf, size_t size)
{
	free(buf);
	(void) size;
}
