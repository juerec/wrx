#ifndef WINDOWFUNC_H
#define WINDOWFUNC_H

// #include <complex.h>

unsigned int NumWindowFuncs();
const char ** WindowFuncNames();
const char *WindowFuncName(int whichFunction);
int WindowFuncId(const char * WindowFuncName);
void WindowFunc(unsigned int whichFunction, unsigned int NumSamples, float *in);
#ifdef COMPLEX_H
void WindowFuncComplex(int whichFunction, int NumSamples, float complex *in);
#endif

#endif /* WINDOWFUNC_H */
