#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

// Texture samplers
uniform sampler2D background;
uniform sampler2D alphaTexture;
uniform sampler2D colorTexture;
uniform sampler2D shineTexture;

// alpha-blends two colors
vec4 blend(vec4 bg,vec4 fg){
  vec3 bgm=bg.rgb*bg.a;
  vec3 fgm=fg.rgb*fg.a;
  float ia=1.0-fg.a;
  float a=(fg.a + bg.a * ia);
  vec3 rgb;
  if(a!=0.0){
    rgb=(fgm + bgm * ia) / a;
  }else{
    rgb=vec3(0.0,0.0,0.0);
  }
  return vec4(rgb,a);
}

void main()
{
  vec4 tmp_alpha = texture(alphaTexture, TexCoord);
  vec4 tmp_background = texture(background, TexCoord);
  vec4 tmp_color = texture(colorTexture, TexCoord);

  tmp_color.a = tmp_alpha.a;
  color = tmp_color;
}
