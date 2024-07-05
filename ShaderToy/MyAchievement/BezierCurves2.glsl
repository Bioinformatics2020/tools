//这个曲线只有一个自由度，x轴按固定时间水平移动

#define EDGE   0.005
#define SMOOTH 0.0025

#define FREQUENCY_SCALE 1000.0
#define TIME_SCALE 20.0


#define A 0.1
#define B 1.9
#define C 0.1
#define D 0.9

//垂直方向与曲线相差的距离
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

// signed distance function for F(x,y)
float SDF( in vec2 coords )
{
    float v = F(coords);
    vec2 h = vec2( 0.01, 0.0 );

    //求斜率
    vec2 g = vec2( F(coords+h.xy) - F(coords-h.xy),
                 F(coords+h.yx) - F(coords-h.yx) ) / (2.0*h.x);
    return abs(v)/length(g);
}

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

//-----------------------------------------------------------------------------
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 texCoord = fragCoord / iResolution.y;
    
    // show the ball moving across the curve
    float color = 1.0;
    float dist = SDF(texCoord);
    if (dist < EDGE + SMOOTH)
    {
        color = smoothstep(EDGE - SMOOTH,EDGE + SMOOTH,dist);
    }
    
	fragColor = vec4(color/3.0,0.0,coordStamp(fragCoord,2.0),1.0);
}
