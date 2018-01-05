//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include <GL/glew.h>
#include <vector>

//========================================================================
//  Element Array Buffer encapsulation:
//    Manages index buffer to configure elements to draw in conjonction
//    with a Vertex Array Object (VAO) or UxVertexArray.
//========================================================================

class UxIndexBuffer
{
private:
  GLuint              _Buffer;                // GL buffer id
  std::vector<GLuint> _IndexVector;           // Vector of indices
  int32_t             _BufferSize;            // Size of the buffer fed with the vector
  uint32_t            _PrimitiveRestartIndex; // Primitive restart attribute for strips

public:

  UxIndexBuffer();
  ~UxIndexBuffer();
  __DeclareDeletedCtorsAndAssignments(UxIndexBuffer)

  void        clearVector();          // Removes all the values from the vector
  void        store(GLenum iUsage);   // Stores the vector content into the buffer
  void        bind() const;           // Bind the buffer to the current context (prior glDrawElements)
  static void unbind();               // Unbind any buffer to the current context 
  uint32_t    getBufferSize() const;  // Returns the size of the buffer

  // Mathods to feed the index vector according different browsing patterns

  void addElement(int32_t iOffset, uint32_t iIndex);
  void addLinearRow(int32_t iOffset, uint32_t iLength);
  void addLinearMatrix(int32_t iOffset, uint32_t iColNumber, uint32_t iRowNumber);
  void addPatchesFromMatrix(int32_t iOffset, bool iColumnMajorOrder, uint32_t iColNumber, uint32_t iRowNumber);
  void addRowLineStripsFromMatrix(int32_t iOffset, bool iColumnMajorOrder, uint32_t iColNumber, uint32_t iRowNumber, GLuint iPrimitiveRestartIndex);
  void addColumnLineStripsFromMatrix(int32_t iOffset, bool iColumnMajorOrder, uint32_t iColNumber, uint32_t iRowNumber, GLuint iPrimitiveRestartIndex);

private:
  void addColumnLineStrips(int32_t iOffset, bool iColumnMajorOrder, uint32_t iStrip, uint32_t iColNumber, uint32_t iRowNumber, GLuint iPrimitiveRestartIndex);
};
