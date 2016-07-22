#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

// Texture samplers
uniform sampler2D fgTexture;
uniform sampler2D bgTexture;
uniform sampler2D alphaTexture;
uniform sampler2D colorTexture;
uniform sampler2D shineTexture;

//alpha-blendings two colors
vec4 blend(vec4 fgColor, vec4 bgColor) {
    float alpha = fgColor.a;
    vec4 color = fgColor * alpha + bgColor*(1 - alpha);
    color.a = alpha + (1 - alpha) * bgColor.a;
    return color;
}


void main()
{
  // Get the alpha value
  float alpha = texture(alphaTexture, TexCoord);

  // Get Coord in the background
  vec2 bgCoord = vec2(gl_FragCoord.x, 600.0 - gl_FragCoord.y)/800.0;

  // Get the bgTexture
  vec4 bgColor = texture(bgTexture, bgCoord);

  // Get the fgTexture
  vec4 fgColor = texture(fgTexture, vec2(TexCoord.x, 1 - TexCoord.y));

  fgColor.a = alpha;

  color = blend(fgColor, bgColor);
}
