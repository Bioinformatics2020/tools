return 1;}

float sdCircle(float2 uv, in float2 p, in float r ) 
{
    return length(uv-p)-r;
}

float smoothSDF(float d ,float width,float smooth)
{
    return 1.0-smoothstep(width-smooth,width+smooth,abs(d));
}

float2 Bezier(float t,float2 P0,float2 P1,float2 P2,float2 P3){
	float _t=1.-t;
	return _t*_t*(P0*_t + 3.*P1*t)+(3.*P2*_t + P3*t)*t*t;
}

float2 BezierTangent(out float2 Tangent,float t,float2 p0,float2 p1,float2 p2,float2 p3)
{
    float2 f=Bezier(t,p0,p1,p2,p3);

    float2 f0=Bezier(t-0.01,p0,p1,p2,p3);
    float2 f1=Bezier(t+0.01,p0,p1,p2,p3);

    Tangent = normalize(f1-f0);
    return f;
}

float SegDist(float2 p, float2 a, float2 b){//distance to line (a,b)
	b-=a;
	p-=a;
	p=p-b*clamp(dot(p,b)/(dot(b,b)),0.,1.);//Fortunately it still work well when a==b => division by 0
	return length(p);
}

float2 BDist1(float2 p,int NBRPT,float2 P0,float2 P1,float2 P2,float2 P3){
	float d=1.e20;
	float t=0.;
    float out_t=1.;
	float2 lq=Bezier(t,P0,P1,P2,P3);
	for(int i=0;i<NBRPT;i++){
		t+=1./float(NBRPT);
		float2 nq=Bezier(t,P0,P1,P2,P3);
        float d0 = SegDist(p,lq,nq);
        if(d0<d)
        {
            out_t = t+1.0-1./float(NBRPT);
            //+dot(p-lq,nq-lq)/pow(length(nq-lq),2.0)/float(NBRPT);
            d=d0;
        }
		lq=nq;
	}
	return float2(d,out_t);




    
float2 uv = U;
float width = Width;
float smooth = Smooth;

float2 p0 = P0;
float2 p1 = P1;
float2 p2 = P2;
float2 p3 = P3;

float2 f=BDist1(uv,110,p0,p1,p2,p3);//
f.x=1.0-smoothstep(Width-smooth,Width+smooth,abs(f.x));//
T = f.y;
return f.x;