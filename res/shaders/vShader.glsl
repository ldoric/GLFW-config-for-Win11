#version 330 core
layout (location = 0) in vec3 aPos;   
layout (location = 1) in vec3 aNorm;  
layout (location = 2) in vec2 aTexCord; 

out vec3 vertexColor;  // šaljemo boju prema fragment shaderu
out vec2 TexCord;

uniform vec2 offset;

void main()
{ 
    vec3 newPos = aPos + vec3(offset, 0.0);
    gl_Position = vec4(newPos, 1.0f);

    vertexColor = vec3(aPos.x * 0.5 + 0.5, aPos.y * 0.5 + 0.5, aPos.z * 0.5 + 0.5);
    

    TexCord = aTexCord;
}
