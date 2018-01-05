//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

#include "MxGLObjects.h"

void MxGLObjects::init()
{
  createInputAttributes();
}

void MxGLObjects::createInputAttributes()
{
  addInputAttribute("PositionCoordinates4f");
  addInputAttribute("TextureCoordinates4f");
  addInputAttribute("HeightTextureCoordinates2f");
  addInputAttribute("ColorComponents4f");
  addInputAttribute("PositionOnPlaneCoordinates2f");
  addInputAttribute("PixelCoordinates2i");
}
