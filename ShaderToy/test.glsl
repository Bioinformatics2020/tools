float wave_amp=0.03;
float wave_freq=3.0;
float wave_speed=5.0;

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 v = fragCoord.xy / iResolution.xy;
    vec2 uv=v*2.-vec2(1.);
    uv.x*=iResolution.x/iResolution.y;

    float d=-uv.x+0.3*uv.y - exp(-1.0+uv.x) +  exp(-uv.y);
    float a=mod(wave_speed*iTime+d*wave_freq,4.0*3.14159265359);

    float shadow = .75 - 0.25*(sin(a-0.31));
    fragColor = vec4(shadow);
}