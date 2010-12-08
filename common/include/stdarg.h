#ifndef _STDARG_H_
#define _STDARG_H_

#define va_list __builtin_va_list
#define va_start(ap, last) __builtin_va_start((ap), (last))
#define va_arg(ap, type) __builtin_va_arg((ap), type)
#define __va_copy(dest, src) __builtin_va_copy((dest), (src))
#define va_end(ap) __builtin_va_end((ap))


#endif
