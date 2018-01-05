//========================================================================
//  Height Map Terrain Model
//  MIT License
//  Copyright (c) 2017 Emmanuel DUPUIS, emmanuel.dupuis@undecentum.com
//========================================================================

// Inputs resulting from GS computation (except HeightTextureUV, passed)
in GSO
{
  vec4  VertexColor;
  float Height;
  float ScreenHeightGradient;
} fsi;

// Resulting fragment's color
out vec4 fso_color;


void main(void)
{
  fso_color = fsi.VertexColor;

  // Optional use of a height color map to determine color
  if (u_HeightMap.colorMode == 1)
  {
    float u = clamp((fsi.Height - u_HeightMap.minHeightColorMap) / (u_HeightMap.maxHeightColorMap - u_HeightMap.minHeightColorMap), 0, 1);
    float v = 0.5;
    fso_color = texture(u_HeightMap.heightColorMap, vec2(u, v))*fsi.VertexColor;
  }

  // Optional isoline display
  if (u_HeightMap.isolineStep > 0)
  {
    if (fsi.ScreenHeightGradient > 0) // Avoid colouring flat zones
    {
      vec4 isolineColor;
      if (u_HeightMap.colorMode == 1)
        isolineColor = vec4(0, 0, 0, 1);
      else
        isolineColor = vec4(1, 0.463, 0.027, 1);

      float intpart = 0;
      float fract = modf(fsi.Height / u_HeightMap.isolineStep, intpart);

      // 5px or 2.5px width (2.5+2.5 or 1.25+1.25) and transition with the same width
      float width = (int(mod(fsi.Height / u_HeightMap.isolineStep + 0.5, 5)) == 0) ? 2.5 : 1.25;
      float limit = width / (fsi.ScreenHeightGradient * u_HeightMap.isolineStep);

      //if (fract < 2 * limit || fract > 1 - 2 * limit) //Systematic computation moreefficient than a conditionnal instruction?
      // isolineColor*fso_color inside [0,limit] and [1-limit, 1] and mix(isolineColor,white,alpha)*fso_color inside [limit,2*limit]
      // and [1-2*limit, 1-limit] with alpha=(min(fract, 1-fract)-limit)/limit
      fso_color = mix(isolineColor, vec4(1, 1, 1, 1), clamp((min(fract, 1 - fract) - limit) / limit, 0, 1))*fso_color;
    }
  }
}
