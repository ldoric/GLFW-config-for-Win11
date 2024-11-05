#version 330 core

in vec3 vertexColor; 
in vec2 TexCord;    

out vec4 fColor;

uniform sampler2D tex;

void main()
{
    vec4 textureColor = texture(tex, TexCord);
	
    //for rgb 
    fColor = vec4(vertexColor, 1.0) * textureColor; // multipliciramo boje
	//for textrue
	//fColor = textureColor;

    //if rgb go to vShader also
}