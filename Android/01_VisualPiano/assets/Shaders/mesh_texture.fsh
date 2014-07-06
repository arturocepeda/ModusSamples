
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda Pérez
//  iOS Game Engine
//
//  Fragment shader (3D mesh with texture)
//
//  --- mesh_texture.fsh ---
//
//////////////////////////////////////////////////////////////////


precision lowp float;

varying vec3 varEyeSpaceVertexNormal;
varying vec4 varEyeSpaceVertexPosition;
varying vec4 varEyeSpacePointVisualPiano1Position;
varying vec2 varTextCoord0;

uniform vec4 uObjectColor;
uniform sampler2D uTexture0;
uniform vec3 uAmbientVisualPianoColor;
uniform float uAmbientVisualPianoIntensity;
uniform vec3 uPointVisualPiano1Color;
uniform float uPointVisualPiano1Intensity;

const float fSpecularExponent = 10.0;

void main()
{
   vec3 vColor;
   float fAlpha = uObjectColor.w;

   // material properties
   vec3 vMaterialDiffuseColor = texture2D(uTexture0, varTextCoord0).xyz;
   vec3 vMaterialSpecularColor = vec3(0.1, 0.1, 0.1);
   
   // get camera direction vector
   vec3 vCameraDirection = normalize(-varEyeSpaceVertexPosition).xyz;


   //   
   //  Add ambient VisualPiano
   //
   vColor = vMaterialDiffuseColor * uAmbientVisualPianoColor * uAmbientVisualPianoIntensity;


   //
   //  Add point VisualPiano
   //   
   vec3 vVisualPianoColor = uPointVisualPiano1Color * uPointVisualPiano1Intensity;
   vec3 vVisualPianoDirection = normalize(varEyeSpacePointVisualPiano1Position - varEyeSpaceVertexPosition).xyz;
   vec3 vHalfPlane = normalize(vVisualPianoDirection + vCameraDirection);
      
   float fDiffuseFactor = max(0.0, dot(varEyeSpaceVertexNormal, vVisualPianoDirection));
   float fSpecularFactor = max(0.0, dot(varEyeSpaceVertexNormal, vHalfPlane));
   fSpecularFactor = pow(fSpecularFactor, fSpecularExponent);
      
   vColor += (vMaterialDiffuseColor * vVisualPianoColor * fDiffuseFactor) + (vMaterialSpecularColor * fSpecularFactor);

   
   //
   //  Set fragment color
   //
   gl_FragColor = vec4(vColor, fAlpha);
}
