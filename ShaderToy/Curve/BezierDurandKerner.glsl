//The control points
vec2 P0 = vec2(-1.,-.7);
vec2 P1 = vec2(-.75,.0);
vec2 P2 = vec2( .75,1.);
vec2 P3 = vec2( 1.,0.);
//leading coefficient of the polynomial defining the curve. Used by the Durand-Kerner solver
float iLeadingCoef=0.;//initialized in main()

/////////////////////////////////////////////////////////////////////////////////////
//Utility function///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
vec2 Bezier(float t){
	float _t=1.-t;
	return _t*_t*(P0*_t + 3.*P1*t)+(3.*P2*_t + P3*t)*t*t;
}

/*vec2 DBezier(float t){
	float _t=1.-t;
	return 3.*((P1-P0)*_t*_t + 2.*(P2-P1)*t*_t + (P3-P2)*t*t);
}*/

float SegDist(vec2 p, vec2 a, vec2 b){//distance to line (a,b)
	b-=a;
	p-=a;
	p=p-b*clamp(dot(p,b)/(dot(b,b)),0.,1.);//Fortunately it still work well when a==b => division by 0
	return length(p);
}

float SegDistSQR(vec2 p, vec2 a, vec2 b){//distance to line (a,b)
	b-=a;
	p-=a;
	p=p-b*clamp(dot(p,b)/(dot(b,b)),0.,1.);//Fortunately it still work well when a==b => division by 0
	return dot(p,p);
}

float Bwidth(vec2 p0, vec2 p1, vec2 p2, vec2 p3){
	//width of the "fat line" going from p0 to p3 and containing the 
	//Bezier curve which control points are (p0..p3)
	return max(SegDistSQR(p1, p0, p3), SegDistSQR(p2, p0, p3));
}
/////////////////////////////////////////////////////////////////////////////////////
//Subdivision////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//Durand-Kerner//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
#define MAX_ITER 20
#define EPS 0.01
float uit=0.;

vec2 cmul(vec2 c1, vec2 c2){
	return vec2(c1.x*c2.x-c1.y*c2.y,c1.x*c2.y+c1.y*c2.x);
}

vec2 cinv(vec2 c){
	return vec2(c.x,-c.y)/dot(c,c);
}

vec4 CBezier(vec2 t){
	vec2 _t=vec2(1.,0.)-t;
	vec2 t2=cmul(t,t);
	vec2 _t2=cmul(_t,_t);
	return vec4(cmul(_t2,(P0.x*_t + 3.*P1.x*t))+cmul((3.*P2.x*_t + P3.x*t),t2),
				cmul(_t2,(P0.y*_t + 3.*P1.y*t))+cmul((3.*P2.y*_t + P3.y*t),t2)
				);
}

vec4 CDBezier(vec2 t){
	vec2 _t=vec2(1.,0.)-t;
	vec2 t2=cmul(t,t);
	return vec4(
				(cmul(((P1.x-P0.x)*_t + 2.*(P2.x-P1.x)*t),_t) + (P3.x-P2.x)*t2),
				(cmul(((P1.y-P0.y)*_t + 2.*(P2.y-P1.y)*t),_t) + (P3.y-P2.y)*t2)
				);
}

vec2 cBfun(vec2 c, vec2 p){
	vec4 CB=CBezier(c)-vec4(p.x,0.,p.y,0.);
	vec4 CDB=CDBezier(c);
	vec2 v=cmul(CB.xy,CDB.xy)+cmul(CB.zw,CDB.zw);
	return v;
}

float DKstep(inout vec2 c0, vec2 c1, vec2 c2, vec2 c3, vec2 c4, vec2 p){
	//return values:
	//c0: new value of c0;
	//returns: step "length" for epsilon comparison
	vec2 fc=cBfun(c0, p);
	fc=iLeadingCoef*cmul(fc,cinv(cmul(cmul(c0-c1,c0-c2),cmul(c0-c3,c0-c4))));
	c0-=fc;
	return max(abs(fc.x),abs(fc.y));
}

float Dist2(vec2 c, vec2 p){
	if(abs(c.y)<=EPS) {
		float t=clamp(c.x,0.,1.);
		vec2 q=Bezier(t)-p;
		return dot(q,q);
	}
	return 1.e20;
}

float BDistDK( in vec2 p ){
	vec2 c0=vec2(1.,0.);
	vec2 c1=vec2(0.,0.9);
	vec2 c2=cmul(c1,vec2(0.4,0.5));
	vec2 c3=cmul(c2,vec2(0.4,0.9));
	vec2 c4=cmul(c3,vec2(0.4,0.9));
	for(int i=0; i<MAX_ITER; i++){
		float e = DKstep(c0, c1, c2, c3, c4, p);
		e = max(e,DKstep(c1, c2, c3, c4, c0, p));
		e = max(e,DKstep(c2, c3, c4, c0, c1, p));
		e = max(e,DKstep(c3, c4, c0, c1, c2, p));
		e = max(e,DKstep(c4, c0, c1, c2, c3, p));
		if(e<EPS) break;
		uit+=1.;
	}
	//return length(c0-p);
	float d=1.e20;
	d=min(d,Dist2(c0,p));//if(abs(c0.y)<=EPS) {float t=clamp(c0.x,0.,1.);vec2 q=Bezier(t)-p; d=min(d,dot(q,q));}
	d=min(d,Dist2(c1,p));//if(abs(c1.y)<=EPS) {float t=clamp(c1.x,0.,1.);vec2 q=Bezier(t)-p; d=min(d,dot(q,q));}
	d=min(d,Dist2(c2,p));//if(abs(c2.y)<=EPS) {float t=clamp(c2.x,0.,1.);vec2 q=Bezier(t)-p; d=min(d,dot(q,q));}
	d=min(d,Dist2(c3,p));//if(abs(c3.y)<=EPS) {float t=clamp(c3.x,0.,1.);vec2 q=Bezier(t)-p; d=min(d,dot(q,q));}
	d=min(d,Dist2(c4,p));//if(abs(c4.y)<=EPS) {float t=clamp(c4.x,0.,1.);vec2 q=Bezier(t)-p; d=min(d,dot(q,q));}
	return sqrt(d);
}
/////////////////////////////////////////////////////////////////////////////////////
//direct method using 110 segments (same time)///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#define NBRPT 110
float BDist(vec2 p){
	float d=1.e20;
	for(int i=0;i<NBRPT;i++){
		vec2 q=p-Bezier(float(i)/float(NBRPT));
		d=min(d, dot(q,q));
	}
	return sqrt(d);
}

float BDist1(vec2 p){
	float d=1.e20;
	float t=0.;
	vec2 lq=Bezier(t);
	for(int i=0;i<NBRPT;i++){
		t+=1./float(NBRPT);
		vec2 nq=Bezier(t);
		d=min(d, SegDist(p,lq,nq));
		lq=nq;
	}
	return d;
}

float BDist2(vec2 p){
	float d=1.e20;
	float t=0.;
	vec2 lq=Bezier(t);
	for(int i=0;i<NBRPT;i++){
		t+=1./float(NBRPT);
		vec2 nq=Bezier(t);
		d=min(d, SegDistSQR(p,lq,nq));
		lq=nq;
	}
	return sqrt(d);
}
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 p = -P0+3.*(P1-P2)+P3;
	iLeadingCoef=1./dot(p,p);
	
	vec2 uv = (2.*fragCoord.xy - iResolution.xy)/ iResolution.y;
	//float f=BDistDK(uv);//
    float f=BDist2(uv);//
	f=1.0-smoothstep(0.0,0.005,abs(f));//
	fragColor = vec4(f);//
}