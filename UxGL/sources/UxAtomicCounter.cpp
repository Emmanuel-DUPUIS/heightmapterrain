//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxAtomicCounter.h"

#include "UxError.h"

#include <cassert>

UxAtomicCounter::UxAtomicCounter(GLuint iBinding, GLuint iOffset)
{
  _Buffer  = 0;
  _Binding = iBinding;
  _Offset  = iOffset;

  glGenBuffers(1, &_Buffer);
  __CheckGLErrors;
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER , _Buffer);
  __CheckGLErrors;
  glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, _Binding, _Buffer);
  glBufferData(GL_ATOMIC_COUNTER_BUFFER , sizeof(GLuint), nullptr, GL_DYNAMIC_READ);
  __CheckGLErrors;
}

UxAtomicCounter::~UxAtomicCounter()
{
  glDeleteBuffers(1, &_Buffer);
  _Buffer = 0;
}

void UxAtomicCounter::set(uint32_t iValue)
{
  assert(_Buffer);
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER , _Buffer);
  __CheckGLErrors;
  glBufferSubData(GL_ATOMIC_COUNTER_BUFFER , _Offset, sizeof(GLuint), &iValue);
  __CheckGLErrors;
}

uint32_t UxAtomicCounter::get() const
{
  assert(_Buffer);
  GLuint rValue = -1;
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER , _Buffer);
  __CheckGLErrors;
  glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER , _Offset, sizeof(GLuint), &rValue);
  __CheckGLErrors;
  return rValue;
}
