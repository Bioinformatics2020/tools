float pi = 3.141592653589793;
// 线性同余生成元
float fun1(int time, int seed )
{
    uint m = uint(0xffffffff);
    uint a = uint(16087);
    uint b = uint(0);
    
    uint num = uint(seed);
    for(int i=0;i<=time;i+=1)
    {
        num = (a*num+b)%m;
    }
    float outNum = float(num)/float(m);
    return outNum;
}

// 随机正态分布
vec2 fun2(float x,float y)
{
    vec2 outNum;
    outNum.x = cos(2.0*pi*x) * pow(-2.0*log(1.0-y),0.5);
    outNum.y = sin(2.0*pi*x) * pow(-2.0*log(1.0-y),0.5);
    return outNum;
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //坐标系归一化(0-1)
    vec2 U = fragCoord / iResolution.y; 
    vec2 U2 = ( fragCoord*2.0 - iResolution.xy ) / iResolution.y; 
    int x = int(U.x*1000.0);
    int seedx = 1;
    int y = int(U.y*1000.0);
    int seedy = 2;

    float RandomX = fun1(x,seedx);
    float RandomY = fun1(y,seedy);

    vec2 RandomGaussian = fun2(RandomX,RandomY);
    fragColor = vec4(RandomGaussian,0.0,1.0);
}