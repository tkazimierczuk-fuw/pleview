// Contents of DLLDefines.h
#ifndef _Pleview_DLLDEFINES_H_
#define _Pleview_DLLDEFINES_H_

/* Cmake will define MyLibrary_EXPORTS on Windows when it
configures to build a shared library. If you are going to use
another build system on windows or create the visual studio
projects by hand you need to define MyLibrary_EXPORTS when
building a DLL on windows.
*/
// We are using the Visual Studio Compiler and building Shared libraries

#if defined (_OWIN32) 
  #if defined(pleview_EXPORTS)
    #define  PLEVIEW_EXPORT __declspec(dllexport)
  #else
    #define  PLEVIEW_EXPORT __declspec(dllimport)
  #endif /* pleview_EXPORTS */
#else /* defined (_WIN32) */
 #define PLEVIEW_EXPORT
#endif

#endif /* _Pleview_DLLDEFINES_H_ */
