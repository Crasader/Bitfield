attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
varying vec2 v_blurTextureCoords[11];

void main()
{
    gl_Position = CC_MVPMatrix * a_position;
	v_fragmentColor = a_color;
	v_texCoord = a_texCoord;

	float pixelSize = 1.0 / 1080.0;
	for (int i = -5; i <= 5; i++) {
		v_blurTextureCoords[i+5] = a_texCoord + vec2(pixelSize * float(i), 0);
	}
}