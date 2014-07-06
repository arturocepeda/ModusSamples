
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda Pérez
//  iOS Game Engine
//
//  Vertex shader (3D mesh with texture)
//
//  --- mesh_texture.vsh ---
//
//////////////////////////////////////////////////////////////////


attribute vec4 aPosition;
attribute vec3 aNormal;
attribute vec2 aTextCoord0;

uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uPointVisualPiano1Position;

varying vec3 varEyeSpaceVertexNormal;
varying vec4 varEyeSpaceVertexPosition;
varying vec4 varEyeSpacePointVisualPiano1Position;
varying vec2 varTextCoord0;

void main()
{
   // varyings for the fragment shader
   varEyeSpaceVertexNormal = normalize(uNormalMatrix * aNormal);
   varEyeSpaceVertexPosition = uModelViewMatrix * aPosition;
   varEyeSpacePointVisualPiano1Position = uModelViewMatrix * vec4(uPointVisualPiano1Position, 0.0);
   varTextCoord0 = aTextCoord0;
   
   // set vertex position
   gl_Position = uModelViewProjectionMatrix * aPosition;
}
