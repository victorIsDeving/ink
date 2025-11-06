out vec4 C;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform vec2 iResolution;

// difusão

#define DIFFUSION_RATE 0.15

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 w = 1.0 / iResolution.xy;

    float c = texture(iChannel0, uv).a;
    float l = texture(iChannel0, uv - vec2(w.x, 0)).a;
    float r = texture(iChannel0, uv + vec2(w.x, 0)).a;
    float u = texture(iChannel0, uv + vec2(0, w.y)).a;
    float d = texture(iChannel0, uv - vec2(0, w.y)).a;

    float age = texture(iChannel1, uv).r;
    float diffFactor = 1.0 - smoothstep(0.2, 0.8, age);
    float laplacian = (l + r + u + d - 4.0 * c);
    float diffCoeff = DIFFUSION_RATE * (1.0 - c) * diffFactor;
    float diffused = c + diffCoeff * laplacian;

    diffused = clamp(diffused, 0.0, 1.0);

    C = vec4(vec3(diffused), diffused);
}
