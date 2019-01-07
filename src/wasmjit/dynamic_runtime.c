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

#include <wasmjit/runtime.h>

#include <wasmjit/sys.h>

/* platform specific */

__attribute__((noreturn))
void wasmjit_trap(int reason)
{
	assert(reason);
	wasmjit_restore_thread_state(*wasmjit_get_jmp_buf(), (reason << 8));
}

__attribute__((noreturn))
void wasmjit_exit(int status)
{
	wasmjit_restore_thread_state(*wasmjit_get_jmp_buf(), (WASMJIT_TRAP_EXIT << 8) | status);
}

int wasmjit_invoke_function(struct FuncInst *funcinst,
			    union ValueUnion *values,
			    union ValueUnion *out)
{
	union ValueUnion lout;
	int ret;
	wasmjit_thread_state jmpbuf;

	if (wasmjit_get_jmp_buf()) {
		lout = wasmjit_invoke_function_raw(funcinst, values);
		if (out)
			*out = lout;
		ret = 0;
	} else {
		wasmjit_set_jmp_buf(&jmpbuf);
		if (!(ret = wasmjit_save_thread_state(jmpbuf))) {
			lout = wasmjit_invoke_function_raw(funcinst, values);
			if (out)
				*out = lout;
			ret = 0;
		}
		wasmjit_set_jmp_buf(NULL);
	}

	return ret;
}

void wasmjit_free_func_inst(struct FuncInst *funcinst)
{
	if (funcinst->invoker)
		wasmjit_unmap_code_segment(funcinst->invoker,
					   funcinst->invoker_size);
	if (funcinst->compiled_code)
		wasmjit_unmap_code_segment(funcinst->compiled_code,
					   funcinst->compiled_code_size);
	free(funcinst);
}

void wasmjit_free_module_inst(struct ModuleInst *module)
{
	size_t i;
	if (module->free_private_data)
		module->free_private_data(module->private_data);
	free(module->types.elts);
	for (i = module->n_imported_funcs; i < module->funcs.n_elts; ++i) {
		wasmjit_free_func_inst(module->funcs.elts[i]);
	}
	free(module->funcs.elts);
	for (i = module->n_imported_tables; i < module->tables.n_elts; ++i) {
		free(module->tables.elts[i]->data);
		free(module->tables.elts[i]);
	}
	free(module->tables.elts);
	for (i = module->n_imported_mems; i < module->mems.n_elts; ++i) {
		free(module->mems.elts[i]->data);
		free(module->mems.elts[i]);
	}
	free(module->mems.elts);
	for (i = module->n_imported_globals; i < module->globals.n_elts; ++i) {
		free(module->globals.elts[i]);
	}
	free(module->globals.elts);
	for (i = 0; i < module->exports.n_elts; ++i) {
		if (module->exports.elts[i].name)
			free(module->exports.elts[i].name);
	}
	free(module->exports.elts);
	free(module);
}
