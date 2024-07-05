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

float ellipseLine(vec2 uv,vec2 scale)
{
    vec2 a = vec2(pow(scale.x,1.68),pow(scale.y,1.68));
    return abs(length(scale * uv) - 1.0) / length(uv * a);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //坐标系归一化
    vec2 U = ( fragCoord*2.0 - iResolution.xy ) / iResolution.y;       
    
    vec2 crossUV = abs( U + vec2(-U.y,U.x) );// 坐标系的旋转与缩放
    float cross = min(crossUV.x,crossUV.y) * 0.707;

    vec2 AbsUV = abs( U ); 
    vec2 CycleUV = abs(U);
    CycleUV.x -= 0.707;

    float alpha =   (AbsUV.x+AbsUV.y)>0.707
        ? abs(length(CycleUV)-0.5)
        : abs(cross);
    
    float tot = (alpha-0.15) * 100.0;//锐化

    fragColor = vec4(tot,0.0,coordStamp(fragCoord,2.0),1.0);
}