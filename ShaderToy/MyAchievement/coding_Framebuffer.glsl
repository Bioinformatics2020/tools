#iChannel0 "self"

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //坐标系归一化
    vec2 U = ( fragCoord*2.0 - iResolution.xy ) / iResolution.y; 

    vec4 fb = texelFetch(iChannel0, ivec2(fragCoord), 0);

    float tot = iTimeDelta * 1.0 / 10.0 + fb.x;

    // Output to screen
    fragColor = vec4(tot,0.0,0.0,1.0);
}