float sdBox( in vec2 p, in vec2 b ) 
{
    p = abs(p)-b;
    if(p.x > 0.0 && p.y > 0.0)
        return length(p);
    else if(p.x < 0.0 && p.y > 0.0)
        return p.y;
    else if(p.x > 0.0 && p.y < 0.0)
        return p.x;
    else
    {
        return max(p.x,p.y);
    }    
}

//化简版1
float sdBox1( in vec2 p, in vec2 b ) 
{
    p = abs(p)-b;
    if(p.x > 0.0 && p.y > 0.0)
        return length(p);
    else
    {
        return max(p.x,p.y);
    }    
}

//化简版2
float sdBox2( in vec2 p, in vec2 b ) 
{
    p = abs(p)-b;
    float g = max(p.x,p.y);
    return (g>0.0)?length(max(p,0.0)):g;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
    vec2 m = (2.0*iMouse.xy-iResolution.xy)/iResolution.y;

	float d = sdBox1(p,vec2(0.5,0.2));
    
	// coloring
    vec3 col = (d>0.0) ? vec3(0.9,0.6,0.3) : vec3(0.65,0.85,1.0);
    col *= 1.0 - exp(-6.0*abs(d));
	col *= 0.8 + 0.2*cos(150.0*d);
	col = mix( col, vec3(1.0), 1.0-smoothstep(0.0,0.01,abs(d)) );
    //
    if( iMouse.z>0.001 )
    {
        d = sdBox1(m,vec2(0.5,0.2));
        col = mix(col, vec3(1.0,1.0,0.0), 1.0-smoothstep(0.0, 0.005, abs(length(p-m)-abs(d))-0.0025));
        col = mix(col, vec3(1.0,1.0,0.0), 1.0-smoothstep(0.0, 0.005, length(p-m)-0.015));
    }

	fragColor = vec4(col,1.0);
}