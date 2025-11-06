out vec4 C;
uniform sampler2D iChannel0; // from BufferA (current ink)
uniform sampler2D iChannel1; // previous age map
uniform vec2 iResolution;

#define DECAY_RATE 0.98
#define MAX_AGE 1.0

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;

    float currentInk = texture(iChannel0, uv).a;
    float oldAge = texture(iChannel1, uv).r;

    // If there's fresh ink, reset age to 0; otherwise slowly increase toward 1
    float newAge = (currentInk > 0.05)
        ? 0.0
        : min(oldAge * DECAY_RATE + (1.0 - DECAY_RATE), MAX_AGE);

    // Store current ink + the age map
    C = vec4(currentInk, newAge, 0.0, 1.0);
}
