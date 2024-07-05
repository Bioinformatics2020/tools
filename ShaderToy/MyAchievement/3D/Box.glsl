//
float sdCylinder(vec3 p, vec3 a, vec3 b, float r)
{
    vec3  ba = b - a;
    vec3  pa = p - a;
    float baba = dot(ba,ba);
    float paba = dot(pa,ba);
    float x = length(pa*baba-ba*paba) - r*baba;
    float y = abs(paba-baba*0.5)-baba*0.5;
    float x2 = x*x;
    float y2 = y*y*baba;
    
    float d = (max(x,y)<0.0)?-min(x2,y2):(((x>0.0)?x2:0.0)+((y>0.0)?y2:0.0));
    
    return sign(d)*sqrt(abs(d))/baba;
}

//求线与面的交点
//设平面为A(x-x0)+B(y-y0)+C(z-z0)=0   设射线参数方程为x=x1+t·m，y=y1+t·n，z=z1+t·p
//将射线参数方程代入平面方程，解出t=(A(x0-x1)+B(y0-y1)+C(z0-z1)) / (A·m+B·n+C·p)
//将t代入射线方程，解出交点
vec3 lineAndPlane(vec3 pointLocation, vec3 pointDirection,vec3 planeLocation, vec3 planeDirection, out float t)
{
    if(planeDirection.x*pointDirection.x+planeDirection.y*pointDirection.y+planeDirection.z*pointDirection.z == 0.0)
    {
        return vec3(999999.0);
    }
    t = (planeDirection.x*(planeLocation.x-pointLocation.x)+planeDirection.y*(planeLocation.y-pointLocation.y)+planeDirection.z*(planeLocation.z-pointLocation.z))
        / (planeDirection.x*pointDirection.x+planeDirection.y*pointDirection.y+planeDirection.z*pointDirection.z);
    return pointLocation+pointDirection*t;
}

//计算点到是否在四边形内部，注意：平面坐标需要按照顺时针或逆时针输入
bool pointToBox2D(vec3 point,vec3 boxPoint0,vec3 boxPoint1,vec3 boxPoint2,vec3 boxPoint3)
{
    float a = dot(point-boxPoint0,boxPoint1-boxPoint0);
    float b = dot(point-boxPoint1,boxPoint2-boxPoint1);
    float c = dot(point-boxPoint2,boxPoint3-boxPoint2);
    float d = dot(point-boxPoint3,boxPoint0-boxPoint3);

    if(a>0.0 && b>0.0 && c>0.0 && d>0.0)
        return true;
    else if(a<0.0 && b<0.0 && c<0.0 && d<0.0)
        return true;
    else
        return false;
}

//计算射线与平面内四边形交点，返回射线长度
float lineAndBox(vec3 pointLocation, vec3 pointDirection, vec3 boxPoint0,vec3 boxPoint1,vec3 boxPoint2,vec3 boxPoint3)
{
    float t = 99999.0;
    vec3 intersection = lineAndPlane(pointLocation,pointDirection,boxPoint0,cross(boxPoint0,boxPoint1),t);
    if(pointToBox2D(intersection,boxPoint0,boxPoint1,boxPoint2,boxPoint3))
    {
        return t;
    }
    return 99999.0;
}

//求射线到原点处边长为1的立方体的距离
float lineToCube(vec3 pos, vec3 dir)
{
    vec3 point0 = vec3( 0.5, 0.5,0.5);
    vec3 point1 =   vec3( 0.5,-0.5,0.5);
    vec3 point2 =   vec3(-0.5,-0.5,0.5);
    vec3 point3 =   vec3(-0.5, 0.5,0.5);
    vec3 point4 =   vec3( 0.5, 0.5,-0.5);
    vec3 point5 =   vec3( 0.5,-0.5,-0.5);
    vec3 point6 =   vec3(-0.5,-0.5,-0.5);
    vec3 point7 =   vec3(-0.5, 0.5,-0.5);

    float t = 99999.0;
    t = min(lineAndBox(pos,dir,point0,point1,point2,point3),t);
    t = min(lineAndBox(pos,dir,point0,point4,point7,point3),t);
    t = min(lineAndBox(pos,dir,point1,point2,point6,point5),t);
    t = min(lineAndBox(pos,dir,point5,point6,point7,point4),t);
    t = min(lineAndBox(pos,dir,point3,point2,point6,point7),t);
    t = min(lineAndBox(pos,dir,point0,point1,point5,point4),t);
    return t;
}


float sdBox( vec3 p, vec3 b )
{
    vec3 d = abs(p) - b;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
    vec2 m = (2.0*iMouse.xy-iResolution.xy)/iResolution.y;

    vec3 cameraLocation = vec3(2.0,2.0,2.0);


    vec3 ro = normalize( vec3(0.4,-0.3,-1.0) );
    vec3 rd = vec3(p.x,p.y+1.0,3.0);

    vec3 tot;
    tot = vec3(1.0/lineToCube(rd,ro));

    fragColor = vec4(tot,1.0);
}