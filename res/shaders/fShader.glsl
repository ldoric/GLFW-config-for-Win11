#version 330 core

in vec3 vertexColor; // ulaz iz vertex shadera
in vec2 TexCord;     // teksturne koordinate

out vec4 fColor;

uniform sampler2D tex;

void main()
{
    // Kombiniramo boju temeljenu na poziciji s teksturom
    vec4 textureColor = texture(tex, TexCord);
	
    //fColor = vec4(vertexColor, 1.0) * textureColor; // multipliciramo boje
	//comment this out for rgb
	fColor = textureColor;
}