//这个曲线使用多条线段拼接而成，在UE4材质中绘制时会有抖动现象

//绘制坐标系的标记点
float coordStamp(vec2 fragCoord,float stampNum)
{
    //坐标系归一化
    vec2 U = ( fragCoord*2.0 - iResolution.xy ) / iResolution.y;    
    
    U = abs(U - 0.5 / stampNum) * stampNum;
    U %= 1.0;

    float stamp = 0.0;
    //正常情况下显示十字架标记，标记太多时显示圆点
    if(stampNum < 4.0)
    {
        U= 1.0 - abs(U-0.5);
        vec2 U1 = ceil(U - 1.0 + 0.05);
        vec2 U2 = ceil(U - 1.0 + stampNum*0.004);
        stamp = (U1.x + U1.y - 1.0) * (U2.x + U2.y);
    }
    else
    {
        stamp = -length(U-0.5) + 0.05;
        stamp *= 100.0;
    }
    return clamp(stamp,0.0,1.0);
}

//绘制两端不平行的线段
float drawLineFormFourPoint(vec2 UV, vec2 a,vec2 b,vec2 c,vec2 d)
{

    //计算两边的斜率
    vec2 line1 = b-a;
    vec2 line2 = c-b;
    vec2 line3 = d-c;

    float cos1 = dot(line2,line1)/length(line2)/length(line1);
    float k1 = sign(line2.x*line1.y - line2.y*line1.x)*pow(2.0/(1.0+cos1)-1.0,0.5);

    float cos2 = dot(line2,line3)/length(line2)/length(line3);
    
    float k2 = sign(line2.x*line3.y - line2.y*line3.x)*pow(2.0/(1.0+cos2)-1.0,0.5);
    
    //tan(atan(cos2)/2.0)
    //cosA=x -> cos(2B) = x -> (tanB2 -1)/(tanB2+1)=-x -> 1+x=2/(tanB2+1) -> tanB2=2/(1+x) - 1
    //tanB=y

    float tot = 0.0;
    //以b点为核心
    vec2 c2 = normalize(vec2( 0.8, -0.75)-c-b);

    c-=b;
    float len = length(c);
    float sin_r = c.y/len;
    float cos_r = c.x/len;
    mat2 Transfrom = mat2(cos_r,-sin_r,sin_r,cos_r);

    //绕b点旋转
    UV -= b;
    UV = Transfrom * UV;

    float width = 0.01;

    if(UV.x + k1*UV.y >= 0.0 && UV.x + k2*UV.y <= len)
    {
        tot = -abs(UV.y) + width;
        tot = tot / width * 1.5;
    }
    // UV += b; 还原UV坐标

    return clamp(tot,0.0,1.0);
}

//绘制线段
float drawLineFormPoint(vec2 UV, vec2 a,vec2 b)
{
    float tot = 0.0;

    b= b-a;
    float len = length(b);
    float sin_r = b.y/len;
    float cos_r = b.x/len;
    mat2 Transfrom = mat2(cos_r,-sin_r,sin_r,cos_r);

    //绕a点旋转
    UV -= a;
    UV = Transfrom * UV;

    float width = 0.01;
    
    //绘制线段
    if(UV.x >= 0.0 && UV.x <= len)
    {
        tot = -abs(UV.y) + width;
        tot = tot / width * 1.5;
    }
    // UV += a; 还原UV坐标

    return clamp(tot,0.0,1.0);
}

//绘制虚线线段
float drawDottedLineFormPoint(vec2 UV, vec2 a,vec2 b)
{
    float tot = 0.0;

    b= b-a;
    float len = length(b);
    float sin_r = b.y/len;
    float cos_r = b.x/len;
    mat2 Transfrom = mat2(cos_r,-sin_r,sin_r,cos_r);

    //绕a点旋转
    UV -= a;
    UV = Transfrom * UV;

    float width = 0.01;
    
    //绘制线段
    if(UV.x >= 0.0 && UV.x <= len)
    {
        tot = -abs(UV.y) + width;
        tot = tot / width * 1.5;

        float dotted = ceil(mod(UV.x + 0.1 ,0.2)-0.1);
        tot *= dotted;
    }
    // UV += a; 还原UV坐标

    return clamp(tot,0.0,1.0);
}

float drawClubPoint(vec2 UV, vec2 point)
{
    vec2 p = -abs(point - UV);
    p += 0.04;
    float tot = min(p.x,p.y) * 1000.0;
    return clamp(tot,0.0,1.0);
}

float drawCirclePoint(vec2 UV, vec2 point)
{
    vec2 p = point - UV;
    float tot = (-length(p) + 0.04) * 200.0;
    return clamp(tot,0.0,1.0);
}

#define POINTNUM 4
//绘制折线
float drawPolyline(vec2 UV,vec2 point[POINTNUM])
{
    vec2 bufPoint;

    float ctlLine = 0.0;
    float ctlPoint = 0.0;
    for(int i=0;i<POINTNUM;i++)
    {
        if(i>0)
        {
            ctlLine += drawDottedLineFormPoint(UV,bufPoint,point[i]);
        }
        bufPoint = point[i];

        if(i == 0 || i == POINTNUM-1)
        {
            ctlPoint += drawCirclePoint(UV,point[i]);
        }
        else
        {
            ctlPoint += drawClubPoint(UV,point[i]);
        }
        
    }

    float line1 = drawDottedLineFormPoint(UV,point[0],point[1]);
    float line2 = drawDottedLineFormPoint(UV,point[1],point[2]);
    float line3 = drawDottedLineFormPoint(UV,point[2],point[3]);
    
    return ctlPoint + ctlLine;
}

//控制点个数
#define CTLNUM 4
//绘制3次贝塞尔曲线
float drawBezierCurve(vec2 UV, vec2 controls[CTLNUM])
{
    //控制点个数,从0开始
    int ctlNum = CTLNUM -1;
    //步长，线段个数的倒数
    float stepSize = 0.01;

    float allCurveLine = 0.0;
    
    //预留4个点作为缓冲区，因为绘制一条线段需要4个点
    vec2 bFour[4];

    int count = 0;
    for(float i = -stepSize;i<=1.0 + stepSize*1.1; i+=stepSize)
    {
        float t1 = i;
        vec2 b = vec2(0.0);
        //计算贝塞尔曲线上的某一个点
        for(int j = 0 ;j<=ctlNum;j++)
        {
            //计算二项式系数
            int c = 1;
            int j2 = min(j,ctlNum-j);
            for(int k = 0;k<j2;k++)
            {
                c = c*(ctlNum - k)/(j2-k);
            }
            //多项式求和
            b = b + float(c) * pow(t1, float(j)) * pow(1.0-t1,float(ctlNum-j)) * controls[j];
        }

        bFour[0] = bFour[1];
        bFour[1] = bFour[2];
        bFour[2] = bFour[3];
        bFour[3] = b;
        if(count>3)
        {
            //drawClubPoint(UV,bFour[0]);
            float curvePoint1 = drawLineFormFourPoint(UV,bFour[0],bFour[1],bFour[2],bFour[3]);
            
            allCurveLine += curvePoint1;
        }
        count+=1;
    }
    return clamp(allCurveLine,0.0,1.0);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //坐标系归一化
    vec2 U = ( fragCoord*2.0 - iResolution.xy ) / iResolution.y; 

    //vec2 mouse = (iMouse.xy*2.0 - iResolution.xy) / iResolution.y;

    vec2 controls[4];
    controls[0] = vec2(-0.5,-0.5);
    controls[1] = vec2( -0.5, 0.5);
    controls[2] = vec2( 0.5, -0.5);
    controls[3] = vec2( 0.5, 0.5);

    float CurveLine = drawBezierCurve(U,controls);
    float ControlPoint = drawPolyline(U,controls);

    fragColor = vec4(ControlPoint,CurveLine,coordStamp(fragCoord,2.0),1.0);
}