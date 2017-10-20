varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
varying vec2 v_blurTextureCoords[11];

void main()
{
	vec4 out_colour = vec4(0.0);
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[0]) * 0.07331;
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[1]) * 0.083048;
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[2]) * 0.091507;
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[3]) * 0.098071;
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[4]) * 0.102234;
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[5]) * 0.103661;
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[6]) * 0.102234;
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[7]) * 0.098071;
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[8]) * 0.091507;
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[9]) * 0.083048;
	out_colour += texture2D(CC_Texture0, v_blurTextureCoords[10]) * 0.07331;
	gl_FragColor = out_colour;
}