#version 120
uniform sampler2D GlowFboTex;

void main() {
    
    gl_FragColor = texture2D(glowFboTex, gl_TexCoord[0].st);
}