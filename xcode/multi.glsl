#version 120

uniform sampler2D Tex1;
uniform sampler2D Tex2;

void main() {
    vec4 color1 = texture2D(Tex1, gl_TexCoord[0].st);
    vec4 color2 = texture2D(Tex2, gl_TexCoord[0].st);
    
    float newR = clamp( color1.r + color2.r , 0.0 , 1.0);
    float newG = clamp( color1.g + color2.g , 0.0 , 1.0);
    float newB = clamp( color1.b + color2.b , 0.0 , 1.0);
    gl_FragColor = vec4( newR, newG, newB, 1.0);
}