//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#pragma once

#include "UxGL.h"
#include "UxIndexBuffer.h"
#include "UxVertexInputAttribute.h"

//========================================================================
//  Vertex Array Object (VAO) encapsulation:
//    Manages buffer of vefrtex to configure elements to draw in 
//    conjonction with a Element Array Buffer or UxIndexBuffer.
//    Base class, the derive class are templates integrating the data
//    structure of a vertex (position, color, texture coordinates...).
//========================================================================

class UxVertexArrayBase
{
private:
  GLuint  _Array;        // GL vertex array id
  GLuint  _Buffer;       // GL buffer (vertices) id
  int32_t _BufferSize;   // Size of the buffer fed with the vector

public:

  UxVertexArrayBase();
  ~UxVertexArrayBase();
  __DeclareDeletedCtorsAndAssignments(UxVertexArrayBase)

  virtual void clearVector();                          // Removes all the values from the vector
  void store(GLenum iUsage);                           // Stores the vector content into the buffer
  void bind(const UxIndexBuffer& iIndexBuffer) const;  // Bind the buffer to the current context (prior glDrawElements)
  static void unbind();                                // Unbind any buffer to the current context 

  // Links a vertex attribute to the array
  void linkAttribute(std::shared_ptr<UxVertexInputAttribute> iInputAttribute, GLenum iDataAttributeType, uint32_t iNbComponents, 
                     uint32_t iAttributeSize, uint32_t iAttributeOffset, bool iNormalized);

protected:
  // To be implemented by derived class tempate
  virtual uint32_t     getElementNumber() const = 0;
  virtual uint32_t     getStructureSize() const = 0;
  virtual const void*  getData() const = 0;
};
