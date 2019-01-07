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

#include <wasmjit/high_level.h>

#include <wasmjit/runtime.h>
#include <wasmjit/sys.h>

#include <unistd.h>

int _wasmjit_high_emscripten_invoke_main(struct WasmJITHigh *self,
					 const char *module_name,
					 int argc, char **argv, char **envp,
					 uint32_t flags);

int wasmjit_high_emscripten_invoke_main(struct WasmJITHigh *self,
					const char *module_name,
					int argc, char **argv, char **envp,
					uint32_t flags)
{
	int ret;

	ret = _wasmjit_high_emscripten_invoke_main(self, module_name,
						   argc, argv, envp, flags);
	if (ret < 0)
		return ret;

	if (WASMJIT_IS_TRAP_ERROR(ret)) {
		/* This was a trap, and this trap could have happened during
		   a signal, so we can only call async-signal-safe functions */

		if (WASMJIT_DECODE_TRAP_ERROR(ret) == WASMJIT_TRAP_EXIT) {
			ret &= 0xff;
		} else {
			int trap_error = WASMJIT_DECODE_TRAP_ERROR(ret);
			const char *trap_reason = wasmjit_trap_reason_to_string(trap_error);
			size_t trap_reason_len;

			for (trap_reason_len = 0; trap_reason[trap_reason_len]; ++trap_reason_len);

			write(2, "TRAP: ", 6);
			write(2, trap_reason, trap_reason_len);
			write(2, "\n", 1);
			ret = 0x7f + trap_error;
		}
	}

	_exit(ret);
}
