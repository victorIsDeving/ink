uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform float iTime;
uniform vec2 iResolution;
uniform vec4 iMouse;
#define EPS  .01
#define COL0 vec3(.2, .35, .55)
#define COL1 vec3(.9, .43, .34)
#define COL2 vec3(.96, .66, .13)
#define COL3 vec3(0.0)
#define COL4 vec3(0.99,0.1,0.09)



void main(  )
{
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    gl_FragColor = vec4(vec3(1.0)-texture(iChannel0, uv).xyz, 1.0);//vec4(mix(vec3(0.80, 0.78, 0.75), vec3(0.027, 0.239, 0.411), texture(iChannel0, uv).r), 1.0);
}
