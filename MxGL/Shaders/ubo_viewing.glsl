//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================
//  Uniform Buffer Oject definition for scene viewing (camera).
//========================================================================

layout(std140) uniform u_ViewingBlock
{
  mat4  view;        // From world coordinates to view coordinates (camera)
  mat4  projection;  // From view coordinates to clipping coordinates
  ivec2 viewport;    // Width and height (pixels) of the viewport
} u_Viewing;
