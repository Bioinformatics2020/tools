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

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //坐标系归一化
    vec2 U = ( fragCoord*2.0 - iResolution.xy ) / iResolution.y; 

    //制作分段函数(3段)
    float X = floor((U.x + 1.0)  * 1.5) / 1.5;

    //计算高度(0.3-1.7)
    float high = sin(X * 3.14 - iTime * 1.75) * 0.7 + 1.0;

    //计算x轴波浪形函数
    float a = clamp(ceil(abs(fract(U.x * 1.75) - 0.5) - 0.4),0.0,1.0);

    //计算y轴缩放
    float tot = ceil(a - 2.0 * abs(U.y) / high);

    fragColor = vec4(tot,0.0,coordStamp(fragCoord,2.0),1.0);
}