/*
  Written by Alan Wolfe
  http://demofox.org/
  http://blog.demofox.org/

  1D bezier curves:
  http://blog.demofox.org/2014/08/28/one-dimensional-bezier-curves/

  helpful table of frequencies:
  http://www.phy.mtu.edu/~suits/notefreqs.html
*/

#define EDGE   0.005
#define SMOOTH 0.0025

#define FREQUENCY_SCALE 1000.0
#define TIME_SCALE 1.0

float noise( in vec2 x )
{
    vec2 p = floor(x);
    vec2 f = fract(x);
	vec2 uv = p.xy + f.xy*f.xy*(3.0-2.0*f.xy);
    //todo 噪点图未导入
	return texture( iChannel0, (uv+118.4)/256.0, -100.0 ).x;
}

float RandomControlPointValue (float seed)
{
    return noise (vec2(seed, floor(iTime / TIME_SCALE)));
}

#define A RandomControlPointValue(1.3)
#define B RandomControlPointValue(2.7)
#define C RandomControlPointValue(3.6)
#define D RandomControlPointValue(4.1)

// F(x,y) = F(x) - y
float F ( in vec2 coords )
{
    float T = coords.x;
    return
        (A * (1.0-T) * (1.0-T) * (1.0-T)) + 
        (B * 3.0 * (1.0-T) * (1.0-T) * T) +
        (C * 3.0 * (1.0-T) * T * T) +
        (D * T * T * T) -
        coords.y;
}

// gradiant function for finding G for a generic function when you can't
// get it analytically using partial derivatives.  We could do
// partial derivatives of F above, but I'm being lazy.
vec2 Grad( in vec2 coords )
{
    vec2 h = vec2( 0.01, 0.0 );
    return vec2( F(coords+h.xy) - F(coords-h.xy),
                 F(coords+h.yx) - F(coords-h.yx) ) / (2.0*h.x);
}

// signed distance function for F(x,y)
float SDF( in vec2 coords )
{
    float v = F(coords);
    vec2  g = Grad(coords);
    return abs(v)/length(g);
}

// signed distance function for Circle, for control points
float SDFCircle( in vec2 coords, in vec2 offset )
{
    coords -= offset;
    float v = coords.x * coords.x + coords.y * coords.y - EDGE*EDGE;
    vec2  g = vec2(2.0 * coords.x, 2.0 * coords.y);
    return v/length(g); 
}

//-----------------------------------------------------------------------------
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{	
    float aspectRatio = iResolution.x / iResolution.y;
    vec2 percent = ((fragCoord.xy / iResolution.xy) - vec2(0.25,0.0));
    percent.x *= aspectRatio;
    
    // show the ball moving across the curve
    vec3 color = vec3(1.0,1.0,1.0);
    float ballX = fract(iTime/TIME_SCALE);
    float ballY = max(0.0,min(1.0,F(vec2(ballX,0.0))));
    float dist = SDFCircle(percent,vec2(ballX,ballY));
	if (dist < 2.0*EDGE + SMOOTH)
    {
        dist = max(dist, 0.0);
        dist = smoothstep(2.0*EDGE,2.0*EDGE + SMOOTH,dist);
        color *= mix(vec3(0.0,1.0,1.0),vec3(1.0,1.0,1.0),dist);
    }    

    dist = SDFCircle(percent, vec2(0.0,A));
	if (dist < EDGE + SMOOTH)
    {
        dist = max(dist, 0.0);
        dist = smoothstep(EDGE,EDGE + SMOOTH,dist);
        color *= mix(vec3(1.0,0.0,0.0),vec3(1.0,1.0,1.0),dist);
    }
    
    dist = SDFCircle(percent, vec2(0.33,B));
	if (dist < EDGE + SMOOTH)
    {
        dist = max(dist, 0.0);
        dist = smoothstep(EDGE,EDGE + SMOOTH,dist);
        color *= mix(vec3(0.0,1.0,0.0),vec3(1.0,1.0,1.0),dist);
    }    
    
    dist = SDFCircle(percent, vec2(0.66,C));
	if (dist < EDGE + SMOOTH)
    {
        dist = max(dist, 0.0);
        dist = smoothstep(EDGE,EDGE + SMOOTH,dist);
        color *= mix(vec3(1.0,1.0,0.0),vec3(1.0,1.0,1.0),dist);
    }    
    
    dist = SDFCircle(percent, vec2(1.0,D));
	if (dist < EDGE + SMOOTH)
    {
        dist = max(dist, 0.0);
        dist = smoothstep(EDGE,EDGE + SMOOTH,dist);
        color *= mix(vec3(0.0,0.0,1.0),vec3(1.0,1.0,1.0),dist);
    }      
    
    dist = SDF(percent);
    if (dist < EDGE + SMOOTH)
    {
        dist = smoothstep(EDGE - SMOOTH,EDGE + SMOOTH,dist);
        color *= (percent.x >= 0.0 && percent.x <= 1.0) ? vec3(dist) : vec3(0.95);
    }
    
	fragColor = vec4(color,1.0);
}
