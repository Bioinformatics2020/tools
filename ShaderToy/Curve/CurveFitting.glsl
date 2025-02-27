//#define Use_Linear
//#define Use_Cosine
//#define Use_Smoothstep
//#define Use_Smoothstep2
//#define Use_Cubic
//#define Use_Hermite
//#define Use_ThirdOrderSpline
//#define Use_Catmull_Rom
//#define Centripetal_Catmull_Rom

// Most of these functions use the following format:

// v0----v1--x--v2----v3

// Where 'x' is the fractional diff between v1 and v2.

//--------------------------------------------------------------------------------
//  1 out, 1 in...
float Hash(float p)
{
	vec3 p3  = fract(vec3(p) * .1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

//--------------------------------------------------------------------------------
float Cubic(float x, float v0,float v1, float v2,float v3) 
{
	float p = (v3 - v2) - (v0 - v1);
	return p*(x*x*x) + ((v0 - v1) - p)*(x*x) + (v2 - v0)*x + v1;
}

//--------------------------------------------------------------------------------
float Catmull_Rom(float x, float v0,float v1, float v2,float v3) 
{
	float c2 = -.5 * v0	+ 0.5*v2;
	float c3 = v0		+ -2.5*v1 + 2.0*v2 + -.5*v3;
	float c4 = -.5 * v0	+ 1.5*v1 + -1.5*v2 + 0.5*v3;
	return(((c4 * x + c3) * x + c2) * x + v1);
	
//	Or, the same result with...
//	float x2 = x  * x;
//	float x3 = x2 * x;
//	return 0.5 * ( ( 2.0 * v1) + (-v0 + v2) * x +
//                  (2.0 * v0 - 5.0 *v1 + 4.0 * v2 - v3) * x2 +
//                  (-v0 + 3.0*v1 - 3.0 *v2 + v3) * x3);
}

float Centripetal_Catmull_Rom_Coefficient(float x, float p0,float p1, float p2,float p3) 
{
	float a = p1;
	float b = 0.5 * (p2 - p0);
	float c = p0 - 2.5 * p1 + 2.0 * p2 - 0.5 * p3;
        float d = 1.5 * (p1 - p2) + 0.5 * (p3 - p0);
	return(a + x*(b + x*(c + x*d)));
}


//--------------------------------------------------------------------------------
float ThirdOrderSpline(float x, float L1,float L0, float H0,float H1) 
{
	return 		  L0 +.5 *
			x * ( H0-L1 +
			x * ( H0 + L0 * -2.0 +  L1 +
			x * ((H0 - L0)* 9.0	 + (L1 - H1)*3.0 +
			x * ((L0 - H0)* 15.0 + (H1 - L1)*5.0 +
			x * ((H0 - L0)* 6.0	 + (L1 - H1)*2.0 )))));
}

//--------------------------------------------------------------------------------
float Cosine(float x, float v0, float v1) 
{
	x = (1.0-cos(x*3.1415927)) * .5;
	return (v1-v0)*x + v0;
}

//--------------------------------------------------------------------------------
float Linear(float x, float v0, float v1) 
{
	return (v1-v0)*x + v0;
}

//--------------------------------------------------------------------------------
float Smoothstep(float x, float v0, float v1) 
{
	x = x*x*(3.0-2.0*x);
	return (v1-v0)*x + v0;
}

//--------------------------------------------------------------------------------
float Smoothstep2(float x, float v0, float v1) 
{
    x = x*x*x*(x*(x*6.0-15.0)+10.0);
    return (v1-v0)*x + v0;
}

//--------------------------------------------------------------------------------
float Hermite(float x, float a, float b, float c, float d)
{
    float e = c-b;
    float f = a-d;
    return(b+.5*x*(e+b-a+x*(a-b+e+x*3.*(e*3.+f+x*5./3.*(-e*3.-f+x*.4*(e*3.+f))))));
}

//================================================================================
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.y;
	
	float pos = (iTime*.5 +uv.x) * 6.0;
	float x  = fract(pos);
	float v0 = Hash(floor(pos));
	float v1 = Hash(floor(pos)+1.0);
	float v2 = Hash(floor(pos)+2.0);
	float v3 = Hash(floor(pos)+3.0);
	float f;
	
#ifdef Use_Linear
	f = Linear(x, v1, v2);
#elif defined Use_Cosine
	f = Cosine(x, v1, v2);
#elif defined Use_Smoothstep
	f = Smoothstep(x, v1, v2);
#elif defined Use_Smoothstep2
	f = Smoothstep2(x, v1, v2);
#elif defined Use_Cubic
	f = Cubic(x, v0, v1, v2, v3);
#elif defined Use_Catmull_Rom
	f = Catmull_Rom(x, v0, v1, v2, v3);
#elif defined  Centripetal_Catmull_Rom
f = Centripetal_Catmull_Rom_Coefficient(x, v0, v1, v2, v3);
#elif defined Use_ThirdOrderSpline
	f = ThirdOrderSpline(x, v0, v1, v2, v3);
#elif defined Use_Hermite
	f = Hermite(x, v0, v1, v2, v3);
    
#endif
//--------------------------------------------------------------------------------

	// Blobs...
	f = .02 / abs(f-uv.y);
	float d = .03/length((vec2(((uv.x)/9.0*.25), uv.y)-vec2(x+.03, v1)) * vec2(.25,1.0));
	f = max(f, d*d);
	d = .03/length((vec2(((uv.x)/9.0*.25), uv.y)-vec2(x-.97, v2)) * vec2(.25,1.0));
	f = max(f, d*d);

	fragColor = vec4(vec3(1.0,.2, .05) * f, 1.0);
}