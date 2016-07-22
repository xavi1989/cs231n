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
    color.a = alpha;
    return color;
}

vec2 parallax(float v) {
    return vec2(v/800.0, v/600.0*1.5);
}

vec2 pixel() {
    return vec2(1.0/800.0, 1.0/600.0);
}

void main()
{
  // Get the alpha value
  float alpha = texture(alphaTexture, TexCoord).a;
  //alpha = clamp(alpha * 20 - 5, 0.0, 1.0);

  // Get Coord in the background
  vec2 bgCoord = vec2(gl_FragCoord.x, 600.0 - gl_FragCoord.y)/800.0;

  // Get the bgTexture
  vec4 bgColor = texture(bgTexture, bgCoord + parallax(20.0));

  // Refraction
  vec4 location = texture(colorTexture, TexCoord);
  float x = location.g;
  float y = location.r;
  vec2 refraction = (vec2(x, y) - 0.5) * 2.0; // convert to [-1, 1]
  vec2 refractionPos = bgCoord + pixel() * refraction * 100 + parallax(20 - 5);

  // Get the fgTexture
  vec4 fgColor = texture(fgTexture, refractionPos);

  fgColor.a = alpha;

  //shine
  //vec2 shinePos = vec2(0.5, 0.5) + refraction * pixel() * (-50);
  //vec4 shineColor = texture(shineTexture, shinePos);
  //vec4 tex = blend(fgColor, shineColor);


  color = blend(fgColor, bgColor);
  //color = vec4(1.0, 0, 0, 1.0);
  //color = shineColor;
}
