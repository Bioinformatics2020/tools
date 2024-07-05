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

#define PI 3.1415926

vec3 rotateVec3(vec3 InPoint,mat3 rotation)
{
    //InPoint = [x2,y2,z2]
    //z = [(z2/c1 - x2/a1)/(c2/c1 - a2/a1) - (y2/b1 - x2/a1)/(b2/b1 - a2/a1)] / 
    //    [(c3/c1 - a3/a1)/(c2/c1 - a2/a1) - (b3/b1 - a3/a1)/(b2/b1 - a2/a1)]
    //y = (y2/b1 - x2/a1)/(b2/b1 - a2/a1) - z * (b3/b1 - a3/a1)/(b2/b1 - a2/a1)
    //x = x2/a1 - y*a2/a1 - z*a3/a1

    float z2c1 = InPoint.z/rotation[2][0];
    float x2a1 = InPoint.x/rotation[0][0];
    float c2c1 = rotation[2][1]/rotation[2][0];
    float a2a1 = rotation[0][1]/rotation[0][0];
    float y2b1 = InPoint.y/rotation[1][0];
    float b2b1 = rotation[1][1]/rotation[1][0];
    float c3c1 = rotation[2][2]/rotation[2][0];
    float a3a1 = rotation[0][2]/rotation[0][0];
    float b3b1 = rotation[1][2]/rotation[1][0];

    float c2c1_a2a1 = c2c1-a2a1;
    float b2b1_a2a1 = b2b1-a2a1;

    float y2b1_x2a1_b2b1_a2a1 = (y2b1-x2a1)/b2b1_a2a1;
    float b3b1_a3a1_b2b1_a2a1 = (b3b1-a3a1)/b2b1_a2a1;

    float z = ((z2c1-x2a1)/c2c1_a2a1 - y2b1_x2a1_b2b1_a2a1) / 
              ((c3c1-a3a1)/c2c1_a2a1 - b3b1_a3a1_b2b1_a2a1);
    float y = y2b1_x2a1_b2b1_a2a1 - z*b3b1_a3a1_b2b1_a2a1;
    float x = x2a1 - y*a2a1 - z*a3a1;
    return vec3(x,y,z);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{	
    vec2 UV = ( fragCoord*2.0 - iResolution.xy ) / iResolution.y;
    vec2 mouse = (iMouse.xy*2.0 - iResolution.xy) / iResolution.y;

    float A = mouse.y * PI;
    float B = mouse.x * PI;
    float C = 0.0;//iTime * PI / 100.0;
    float SA = sin(A);
    float SB = sin(B);
    float SC = sin(C);
    float CA = cos(A);
    float CB = cos(B);
    float CC = cos(C);

    //XYZ 方向旋转
    mat3 rotation = mat3(
        CB*CC            , -CB*SC           , SB    ,
        CA*SC + CC*SA*SB , CA*CC - SA*SB*SC , -CB*SA,
        SA*SC - CA*CC*SB , CC*SA + CA*SB*SC , CA*CB
    );

    vec3 coord = vec3(UV.x,UV.y,0.0);
    vec3 transfromUV = ( rotation * coord);

    vec3 coord2 = abs(coord * coord / transfromUV);
    
    float cube = (-max(coord2.x, coord2.y) + 0.5) * 10.0;

    mat3 rotation3 = mat3(
        CB*CC            , -CB*SC           , SB    ,
        CA*SC + CC*SA*SB , CA*CC - SA*SB*SC , -CB*SA,
        SA*SC - CA*CC*SB , CC*SA + CA*SB*SC , CA*CB
    );

    vec3 coord3 = abs(rotateVec3(coord,inverse(rotation3)));
    float cube3 = (-max(coord3.x, coord3.y) + 0.5) * 10.0;

    
    vec3 tot = vec3(cube,cube3,0.0);
    fragColor = vec4( tot.xy,coordStamp(fragCoord,2.0), 1.0 );
}