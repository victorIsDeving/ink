out vec4 C;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
uniform vec2 iResolution;
uniform vec4 iMouse;
uniform float iTime;
uniform int iFrame;


#define BRUSH_SIZE 10.0
#define STROKE_LENGTH 300.0

#define dt 0.20

float rand(vec2 n) {
        return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}
float noise(vec2 p){
        vec2 ip = floor(p);
        vec2 u = fract(p);
        u = u*u*(3.0-2.0*u);

        float res = mix(
        mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
        mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
        return res*res;
}

void main(  )
{
    vec2 uv = gl_FragCoord.xy/iResolution.xy;

    float brushStrength = texture(iChannel0, vec2(0.5, 0.0)).a;

    vec4 col = texture(iChannel0, uv);

    vec2 w = 1.0/iResolution.xy;

    vec4 tr = texture(iChannel0, uv + vec2(w.x , 0), 0.0);
    vec4 tl = texture(iChannel0, uv - vec2(w.x , 0), 0.0);
    vec4 tu = texture(iChannel0, uv + vec2(0 , w.y), 0.0);
    vec4 td = texture(iChannel0, uv - vec2(0 , w.y), 0.0);

    vec3 dx = (tr.xyz - tl.xyz)*0.5;
    vec3 dy = (tu.xyz - td.xyz)*0.5;
    vec2 densDif = vec2(dx.z ,dy.z);

    const float K = 0.2;
    const float v = 0.55;

    col.z -= dt*dot(vec3(densDif, dx.x + dy.y) ,col.xyz);
    vec2 laplacian = tu.xy + td.xy + tr.xy + tl.xy - 4.0*col.xy;
    vec2 viscForce = vec2(v)*laplacian;
    col.xyw = texture(iChannel0, uv - dt*col.xy*w, 0.).xyw;

    vec2 oldBrushPos = vec2(texture(iChannel0, vec2(0.0, 0.5)).a,
                            texture(iChannel0, vec2((iResolution.x-1)/iResolution.x, 0.5)).a);

    vec4 brushData = texture(iChannel0, vec2(0.0));
    //brushStrength=brushData.z;

    vec2 dirVec = normalize(iMouse.xy -  brushData.xy);
    vec4 brushColor = vec4(dirVec.x, dirVec.y, 0.0, 1.0);
    float brushStregthRamp = clamp(sin(brushStrength*3.1415926), 0.0, 1.0);
    float d;
    float bd;

    for(int i=0; i < BRUSH_SIZE; i++) {
        vec2 mPos = mix(iMouse.xy, brushData.xy, float(i)/STROKE_LENGTH);
        d = 1.0 - smoothstep(0.0, 1.0, distance(gl_FragCoord.xy, mPos) / BRUSH_SIZE / brushStregthRamp);
        d *= smoothstep(0.2, 1.0, (noise(gl_FragCoord.xy-mPos)+0.75)/5.0) * 10.0;
        bd = 1.0 - smoothstep(0.0, 1.0, distance(gl_FragCoord.xy, mPos) / BRUSH_SIZE / brushStregthRamp / 1.5);
        bd *= smoothstep(0.1, 1.0, (noise(vec2(brushStrength * 20.0, 0.0) + (gl_FragCoord.xy-mPos)/2.0)+0.0)/5.0) * 10.0;
        col = mix(col, brushColor, d + bd);
    }

    col.xyz *= 0.95;

    brushStrength -= distance(iMouse.xy, brushData.xy) / STROKE_LENGTH;
    float oldMouseClick = texture(iChannel0, vec2(0.5, (iResolution.y-1)/iResolution.y)).a;
    if(iMouse.z != brushData.w) {
        if(iMouse.w > 0.0) {
                brushStrength = 1.0;
        } else {
            brushStrength = 0.0;
        }
    }
    brushStrength = clamp(brushStrength, 0.0, 1.0);

    vec4 oldMouseXCol = vec4(col.xyz, iMouse.x);
    vec4 oldMouseYCol = vec4(col.xyz, iMouse.y);
    vec4 brushStrengthCol = vec4(col.xyz, brushStrength);
    vec4 mouseClickCol = vec4(col.xyz, iMouse.z);

    col = mix(oldMouseXCol, col, smoothstep(1.0, 2.0, gl_FragCoord.x));
    col = mix(col, oldMouseYCol, smoothstep(iResolution.x-2.0, iResolution.x, gl_FragCoord.x));
        col = mix(brushStrengthCol, col, smoothstep(1.0, 2.0, gl_FragCoord.y));
    col = mix(col, mouseClickCol, smoothstep(iResolution.y-2.0, iResolution.y, gl_FragCoord.y));
    vec4 newBrushData = vec4(iMouse.xy, brushStrength, iMouse.z);
    C = mix(newBrushData, col, smoothstep((iResolution.x-2)/iResolution.x, 1.0, gl_FragCoord.x));
}
