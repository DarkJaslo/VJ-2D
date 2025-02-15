#version 330

uniform vec4 color;
uniform sampler2D tex;
uniform float opacity = 1.0f;

in vec2 texCoordFrag;
out vec4 outColor;

void main()
{
	// Discard fragment if texture sample has alpha < 0.3
	// otherwise compose the texture sample with the fragment's interpolated color
	vec4 texColor = texture(tex, texCoordFrag);
	texColor.a *= opacity;
	if(texColor.a < 0.3f)
		discard;
	outColor = color * texColor;
}

