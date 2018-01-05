//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxUniformBlockBase.h"
#include "UxProgram.h"
#include "UxError.h"

#include <cassert>


UxUniformBlockBase::UxUniformBlockBase(const std::string& iName, GLenum iUsage, int32_t iBinding, const std::string& iStructureName, size_t iBufferSize)
{
  _Name          = iName;
  _Buffer        = 0;
  _Binding       = allocator::allocate(this, _Name, iBinding);
  _StructureName = iStructureName;
  _BufferSize    = iBufferSize;
  _IsMapped      = false;

  glGenBuffers(1, &_Buffer);
  __CheckGLErrors;
  glBindBuffer(GL_UNIFORM_BUFFER, _Buffer);
  __CheckGLErrors;

  glBufferData(GL_UNIFORM_BUFFER, _BufferSize, nullptr, iUsage);
  __CheckGLErrors;
  glBindBufferRange(GL_UNIFORM_BUFFER, _Buffer, _Binding, 0, _BufferSize);
  __CheckGLErrors;
}

UxUniformBlockBase::~UxUniformBlockBase()
{
  glDeleteBuffers(1, &_Buffer);
  _Buffer = 0;
}

void UxUniformBlockBase::map()
{
  assert(_Buffer && !_IsMapped);
  glBindBufferBase(GL_UNIFORM_BUFFER, _Binding, _Buffer);
  __CheckGLErrors;
  _IsMapped = true;
}

void UxUniformBlockBase::unmap()
{
  assert(_Buffer && _IsMapped);
  glBindBufferBase(GL_UNIFORM_BUFFER, _Binding, _Buffer);
  __CheckGLErrors;
  glUnmapBuffer(GL_UNIFORM_BUFFER);
  __CheckGLErrors;
  _IsMapped = false;
}

void UxUniformBlockBase::bindToProgram(const UxProgram* iProgram, const std::string& iUniformName)
{
  // "layout(std140) uniform xxxBlock" with xxx as uniform name
  std::string blockName = iUniformName + "Block";

  GLuint unifBlockIndex = glGetUniformBlockIndex(iProgram->id(), blockName.c_str());
  
  if (unifBlockIndex == -1)
  {
    UxError::error(__FILE__, __LINE__) << "Undefined Uniform Block \"" << blockName.c_str() << "\" in program \"" << iProgram->getName() << ".\n";
    UxError::UxError::exit(-1);
  }

  // Verifies that the buffer size identical to the block uniform's size
  GLint size;
  glGetActiveUniformBlockiv(iProgram->id(), unifBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
  if (_BufferSize != size)
  {
    UxError::error(__FILE__,__LINE__) << "Inconsistent definition between structure associated to the UBO \"" << _StructureName << "\" (size=" << _BufferSize
      << ") and the Uniform Block \"" << blockName.c_str() << "\" in program \"" << iProgram->getName() << "\" (size=" << size << ").\n";
    UxError::UxError::exit(-1);
  }

  glUniformBlockBinding(iProgram->id(), unifBlockIndex, _Binding);
  __CheckGLErrors;
}
