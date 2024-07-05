#iChannel0 "file://C:/Users/Administrator/Desktop/ZXTools/CppProject/GLSL/MyAchievement/Image/1.png"

float step = 0.001;
int Radius = 20;

//不模糊
vec4 noBlur(vec2 texCoord)
{
    return texture(iChannel0, texCoord);
}

//均值模糊
vec4 averageBlur(vec2 texCoord)
{
    float radius = float(Radius) - 1.0;
    float weight = 1.0/pow(radius*2.0+1.0,2.0);
    vec4 Color = vec4(0);

    for(float i=-radius;i<=radius;i++)
    {        
        for(float j=-radius;j<=radius;j++)
        {
            vec2 offsetTexCoord = texCoord;
            offsetTexCoord.x = offsetTexCoord.x + step*i;
            offsetTexCoord.y = offsetTexCoord.y + step*j;
            Color += texture(iChannel0, offsetTexCoord) * weight;
        }
    }
    return Color;
}

vec4 gaussianBlur(vec2 texCoord)
{
    float radius = float(Radius) - 1.0;
    float sigma = 1.0;
    float pi = 3.141592653589793;
    float e  = 2.718281828459045;
    vec4 Color = vec4(0);
    float allWeight=0.0;
    
    for(float i=-radius;i<=radius;i++)
    {        
        for(float j=-radius;j<=radius;j++)
        {
            float x=i/20.0;
            float y=j/20.0;

            float weight = 1.0 / (2.0 * pi * pow(sigma,2.0) * pow(e,- (pow(x,2.0) + pow(y,2.0)) / (2.0 * pow(sigma,2.0))));
            weight *= pow(1.0/40.0,2.0);
            allWeight += weight;
            vec2 offsetTexCoord = texCoord;
            offsetTexCoord.x = clamp(offsetTexCoord.x + step*i,0.0,1.0);
            offsetTexCoord.y = clamp(offsetTexCoord.y + step*j,0.0,1.0);
            Color += texture(iChannel0, offsetTexCoord) * weight;
        }
    }

    //return Color / allWeight;
    return Color;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //坐标系归一化
    vec2 texCoord = fragCoord / iResolution.y; 

    vec2 U = ( fragCoord*2.0 - iResolution.xy ) / iResolution.y; 

    //纹素点采样(像素点)
    //vec4 originalColor = texelFetch(iChannel0, ivec2(texCoord*vec2(1067,1157)), 0);
    //纹理采样
    //vec4 originalColor = texture(iChannel0, texCoord);



    // Output to screen
    //fragColor = noBlur(texCoord);
    //fragColor = averageBlur(texCoord);
    fragColor = gaussianBlur(texCoord);
    
}