//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once
  
#include "UxGL.h"

#include "gl/glew.h"
#include <stdint.h>

//========================================================================
//  Atomic Counter encapsulation:
//    Manages creation and set/get value. Might integrate automatic 
//    resource allocation.
//========================================================================

class UxAtomicCounter
{
private:
  GLuint _Buffer;
  GLuint _Binding;
  GLuint _Offset;

public:
  UxAtomicCounter(GLuint iBinding, GLuint iOffset);
  ~UxAtomicCounter();
  __DeclareDeletedCtorsAndAssignments(UxAtomicCounter)
  
  uint32_t get() const;
  void     set(uint32_t iValue);
};
