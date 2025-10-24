out vec4 C;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
uniform float iTime;
uniform vec4 iMouse;
uniform vec2 iResolution;


void main( ){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    C = vec4(vec3(1.0-texture(iChannel0, uv).a), 1.0);
}

