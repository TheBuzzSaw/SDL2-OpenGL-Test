#version 120

uniform mat4 theMatrix;
attribute vec3 position;
attribute vec3 color;
attribute vec2 textureCoordinates;
varying vec3 _color;
varying vec2 _textureCoordinates;

void main()
{
    _color = color;
    _textureCoordinates = textureCoordinates;
    gl_Position = theMatrix * vec4(position, 1.0);
}

