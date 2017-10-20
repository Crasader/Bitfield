varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
   //gl_FragColor = vec4(1,1,0,1);

	vec4 out_colour = texture2D(CC_Texture0, v_texCoord);
	float brightness = (out_colour.r * 0.2126) + (out_colour.g * 0.7152) + (out_colour.b * 0.0722);
	if (brightness < 0.7) {
		out_colour = vec4(0,0,0,1);
	}
	gl_FragColor = out_colour;
}