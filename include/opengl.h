#pragma once

#ifdef WIN32
#include <gl/gl.h>
// #include "third_party/opengl/glext.h"
#define GL_GET_PROC_ADDRESS wglGetProcAddress
#else
#include <GL/gl.h>
#include <GL/glext.h>
#define GL_GET_PROC_ADDRESS glfwGetProcAddress
#endif

#define FOR_OPENGL_FUNCTIONS(DO)                                      \
    DO(::PFNGLCREATESHADERPROC, glCreateShader)                       \
    DO(::PFNGLDELETESHADERPROC, glDeleteShader)                       \
    DO(::PFNGLSHADERSOURCEPROC, glShaderSource)                       \
    DO(::PFNGLCOMPILESHADERARBPROC, glCompileShader)                  \
    DO(::PFNGLGETSHADERIVPROC, glGetShaderiv)                         \
    DO(::PFNGLCREATEPROGRAMPROC, glCreateProgram)                     \
    DO(::PFNGLDELETEPROGRAMPROC, glDeleteProgram)                     \
    DO(::PFNGLATTACHSHADERPROC, glAttachShader)                       \
    DO(::PFNGLLINKPROGRAMPROC, glLinkProgram)                         \
    DO(::PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)           \
    DO(::PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)                 \
    DO(::PFNGLGENBUFFERSPROC, glGenBuffers)                           \
    DO(::PFNGLDELETEBUFFERSPROC, glDeleteBuffers)                     \
    DO(::PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)                 \
    DO(::PFNGLBINDBUFFERPROC, glBindBuffer)                           \
    DO(::PFNGLBUFFERDATAPROC, glBufferData)                           \
    DO(::PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)         \
    DO(::PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray) \
    DO(::PFNGLUSEPROGRAMPROC, glUseProgram)                           \
    DO(::PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)               \
    DO(::PFNGLDRAWARRAYSEXTPROC, glDrawArraysExt)

#define DO_INLINE(TYPE, NAME) inline TYPE NAME;
FOR_OPENGL_FUNCTIONS(DO_INLINE)
