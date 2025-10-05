#version 330 core

out vec4 FragColor;

//in vec2 texCoord;
in vec3 pos;
//uniform sampler2D ourTexture;

void main()
{
   //FragColor = texture(ourTexture, texCoord);
   FragColor = vec4(0.2, pos.y, 0.2, 1.0);
}