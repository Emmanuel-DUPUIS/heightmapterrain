//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxVertexArrayBase.h"

UxVertexArrayBase::UxVertexArrayBase()
{
  _Array     = -1; // Avoid use if "VAO object 0 a default object"
  _Buffer    =  0;
  _BufferSize = -1;

  glGenVertexArrays(1, &_Array);
  __CheckGLErrors;
  
  glGenBuffers(1, &_Buffer);
  __CheckGLErrors;
   
  glBindBuffer(GL_ARRAY_BUFFER, _Buffer);
  __CheckGLErrors;
}

UxVertexArrayBase::~UxVertexArrayBase()
{
  if (_Array != -1)
  {
    glDeleteVertexArrays(1, &_Array);
    _Array = -1;
  }

  if (_Buffer)
  {
    glDeleteBuffers(1, &_Buffer);
    _Buffer = 0;
  }
}

void UxVertexArrayBase::clearVector()
{
  _BufferSize = -1;
}

void UxVertexArrayBase::store(GLenum iUsage)
{
  assert(_BufferSize == -1);

  glBindBuffer(GL_ARRAY_BUFFER, _Buffer);
  __CheckGLErrors;

  _BufferSize = getElementNumber();
  assert(_BufferSize > -1);

  glBufferData(GL_ARRAY_BUFFER, getStructureSize()*_BufferSize, getData(), GL_STREAM_DRAW);
  __CheckGLErrors;

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  __CheckGLErrors;
}

void UxVertexArrayBase::bind(const UxIndexBuffer& iIndexBuffer) const
{
  assert(_Array != -1 && _Buffer != 0 && _BufferSize != -1);
  glBindVertexArray(_Array);
  glBindBuffer(GL_ARRAY_BUFFER, _Buffer);
  iIndexBuffer.bind();
}

void UxVertexArrayBase::unbind()
{
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  UxIndexBuffer::unbind();
}

void UxVertexArrayBase::linkAttribute(std::shared_ptr<UxVertexInputAttribute> iInputAttribute, GLenum iDataAttributeType, uint32_t iNbComponents, uint32_t iVertexSize, uint32_t iAttributeOffset, bool iNormalized)
{
  assert(iInputAttribute);

  int32_t location = iInputAttribute->getLocation();
  assert(location >= 0);

  glBindVertexArray(_Array);
  __CheckGLErrors;
  glBindBuffer(GL_ARRAY_BUFFER, _Buffer);
  __CheckGLErrors;
  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, iNbComponents, iDataAttributeType, iNormalized, iVertexSize, reinterpret_cast<const GLvoid*>(iAttributeOffset));
  __CheckGLErrors;
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
