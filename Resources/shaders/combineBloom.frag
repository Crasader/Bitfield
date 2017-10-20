varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D Atex;
uniform sampler2D Btex;

void main()
{
   gl_FragColor = texture2D(Atex, v_texCoord) + texture2D(Btex, v_texCoord) * 1.5;
}