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

//绘制椭圆
float ellipseLine(vec2 uv,vec2 scale)
{
    vec2 a = vec2(pow(scale.x,1.68),pow(scale.y,1.68));
    return abs(length(scale * uv) - 1.0) / length(uv * a);
}

//使用旋转并截取的方法绘制线段
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

    float width = 0.02;
    
    //绘制线段
    if(UV.x >= 0.0 && UV.x <= len)
    {
        tot = -abs(UV.y) + width;
        tot = tot * 100.0;
    }
    // UV += a; 还原UV坐标

    return clamp(tot,0.0,1.0);
}