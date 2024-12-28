
#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 colorMod;
uniform vec4 colorAdd;
uniform sampler2D _texture;

void main()
{
    FragColor = texture(_texture, TexCoord) * colorMod;
    FragColor += colorAdd;
}
