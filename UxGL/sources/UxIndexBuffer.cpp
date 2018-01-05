//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "UxIndexBuffer.h"
#include "UxError.h"

#include <cassert>

UxIndexBuffer::UxIndexBuffer()
{
  _Buffer                =  0;
  _BufferSize            = -1;
  _PrimitiveRestartIndex =  0;
}

UxIndexBuffer::~UxIndexBuffer()
{
  if (_Buffer)
  {
    glDeleteBuffers(1, &_Buffer);
    _Buffer = 0;
  }
}

void UxIndexBuffer::clearVector()
{
  _BufferSize = -1;
  _IndexVector.clear();
}

void UxIndexBuffer::store(GLenum iUsage)
{
  assert(_BufferSize == -1);

  if (_Buffer == 0)
  {
    glGenBuffers(1, &_Buffer);
    __CheckGLErrors;
  }

  _BufferSize = _IndexVector.size(); 
  assert(_BufferSize > -1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _Buffer);
  __CheckGLErrors;

  GLuint* pData = _IndexVector.data();  
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _BufferSize*sizeof(pData[0]), (void*)pData, iUsage);
  __CheckGLErrors;

  _IndexVector.clear();
}

void UxIndexBuffer::bind() const
{
  assert(_BufferSize > -1);
  
  if (_PrimitiveRestartIndex)
  {
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(_PrimitiveRestartIndex);
  }
  else
    glDisable(GL_PRIMITIVE_RESTART);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _Buffer);
}

void UxIndexBuffer::unbind()
{
  glDisable(GL_PRIMITIVE_RESTART);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t UxIndexBuffer::getBufferSize() const
{
  assert(_BufferSize > -1);
  return _BufferSize;
}

void UxIndexBuffer::addElement(int32_t iOffset, uint32_t iIndex)
{
  assert(_BufferSize == -1);
  _IndexVector.push_back(iOffset+iIndex);
}

void UxIndexBuffer::addLinearRow(int32_t iOffset, uint32_t iLength)
{
  assert(_BufferSize == -1);
  
  uint32_t size = _IndexVector.size();
  _IndexVector.reserve(size + iLength);

  uint32_t indexNb = iOffset + iLength;
  for (uint32_t index = iOffset; index < indexNb; index++)
    _IndexVector.push_back(index);
}

void UxIndexBuffer::addLinearMatrix(int32_t iOffset, uint32_t iColNumber, uint32_t iRowNumber)
{
  assert(_BufferSize == -1);
  
  uint32_t size = _IndexVector.size();
  uint32_t length = iColNumber*iRowNumber; 
  _IndexVector.reserve(size + length);

  uint32_t indexNb = iOffset + length; 
  for (uint32_t index = iOffset; index < indexNb; index++)
    _IndexVector.push_back(index);
}

void UxIndexBuffer::addPatchesFromMatrix(int32_t iOffset, bool iColumnMajorOrder, uint32_t iColNumber, uint32_t iRowNumber)
{
  assert(_BufferSize == -1);
  
  uint32_t size   = _IndexVector.size();
  uint32_t length = (iColNumber-1)*(iRowNumber-1);
  _IndexVector.reserve(size + 4 * length);

  uint32_t pos   = iOffset;
  uint32_t shift = iColumnMajorOrder ? iRowNumber : iColNumber;
  for (uint32_t iX = 0; iX < iColNumber - 1; iX++)
  {
    for (uint32_t iY = 0; iY < iRowNumber - 1; iY++)
    {
      _IndexVector.push_back(pos);
      _IndexVector.push_back(pos + shift);
      _IndexVector.push_back(pos + shift + 1);
      _IndexVector.push_back(pos + 1);
      pos++;
    }
    pos++;
  }
}

void UxIndexBuffer::addColumnLineStripsFromMatrix(int32_t iOffset, bool iColumnMajorOrder, uint32_t iColNumber, uint32_t iRowNumber, GLuint iPrimitiveRestartIndex)
{
  addColumnLineStrips(iOffset, iColumnMajorOrder, iRowNumber, iColNumber, iRowNumber, iPrimitiveRestartIndex);
}

void UxIndexBuffer::addRowLineStripsFromMatrix(int32_t iOffset, bool iColumnMajorOrder, uint32_t iColNumber, uint32_t iRowNumber, GLuint iPrimitiveRestartIndex)
{
  addColumnLineStrips(iOffset, !iColumnMajorOrder, iColNumber, iRowNumber, iColNumber, iPrimitiveRestartIndex);
}

void UxIndexBuffer::addColumnLineStrips(int32_t iOffset, bool iColumnMajorOrder, uint32_t iStrip, uint32_t iColNumber, uint32_t iRowNumber, GLuint iPrimitiveRestartIndex)
{
  assert(_BufferSize == -1 && (_PrimitiveRestartIndex == 0 || iPrimitiveRestartIndex == _PrimitiveRestartIndex));
  
  uint32_t size     = _IndexVector.size();
  uint32_t strip    = iRowNumber;
  uint32_t vertexNb = iColNumber*iRowNumber;
  uint32_t length   = vertexNb + strip;

  _IndexVector.reserve(size + length);

  uint32_t base    = 0; 
  uint32_t modulo  = 0;
  uint32_t delta = iColNumber + iRowNumber - strip;
  uint32_t indexNb = iOffset + vertexNb;
  for (uint32_t index = iOffset; index < indexNb; index++)
  {
    if (iColumnMajorOrder)
      _IndexVector.push_back(index);
    else
      _IndexVector.push_back(base + modulo*delta);
    modulo++;
    if (modulo == strip)
    {
      _IndexVector.push_back(iPrimitiveRestartIndex);
      modulo = 0;
      base++;
    }
  }

  _PrimitiveRestartIndex = iPrimitiveRestartIndex;
}

