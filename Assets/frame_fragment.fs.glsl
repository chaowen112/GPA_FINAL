#version 410 core
uniform sampler2D tex;
out vec4 color;
in VS_OUT
{
    vec2 texcoord;
} fs_in;

uniform int bar_on;
uniform int state;
uniform float offset;
//varying vec2 v_texCoord; 
vec2 img_size=vec2(600,600);
vec4 blur()
{
    int half_size = 2; 
	vec4 color_sum = vec4(0);
	for (int i = -half_size; i <= half_size ; ++i) 
	{        
	    for (int j = -half_size; j <= half_size ; ++j) 
		{ 
		    ivec2 coord = ivec2(gl_FragCoord.xy) + ivec2(i, j); 
			color_sum += texelFetch(tex, coord, 0); 
		} 
	} 
	int sample_count = (half_size * 2 + 1) * (half_size * 2 + 1); 
	color = color_sum / sample_count;
    return color;
}
vec4 quantization()
{
    float nbins = 8.0; 
	vec3 tex_color = texelFetch(tex, ivec2(gl_FragCoord.xy), 0).rgb; 
	tex_color = floor(tex_color * nbins) / nbins; 
	vec4 finalcolor = vec4(tex_color, 1.0);
    return finalcolor;
}
vec4 dog()
{   
    float sigma_e = 2.0f; 
	float sigma_r = 2.8f; 
	float phi = 3.4f;
	float tau = 0.99f;
	float twoSigmaESquared = 2.0 * sigma_e * sigma_e; 
	float twoSigmaRSquared = 2.0 * sigma_r * sigma_r;
	int halfWidth = int(ceil( 2.0 * sigma_r ));

    vec2 sum = vec2(0.0); 
	vec2 norm = vec2(0.0);
    for ( int i = -halfWidth; i <= halfWidth; ++i ) 
	{ 
	    for ( int j = -halfWidth; j <= halfWidth; ++j ) 
		{ 
		    float d = length(vec2(i,j)); 
			vec2 kernel= vec2( exp( -d * d / twoSigmaESquared ), exp( -d * d / twoSigmaRSquared ));
            vec4 c= texture(tex,fs_in.texcoord+vec2(i,j)/img_size); 
			vec2 L= vec2(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
            norm += 2.0 * kernel; 
			sum += kernel * L; 
		}
    }
    sum /= norm; 
	float H = 100.0 * (sum.x - tau * sum.y); 
	float edge =( H > 0.0 )?1.0:2.0 *smoothstep(-2.0, 2.0, phi * H );
    vec4 finalcolor = vec4(edge,edge,edge,1.0 );
	return finalcolor;
}
vec4 red_blue()
{
    vec4 texture_color_Left = texture(tex,fs_in.texcoord-vec2(0.005,0.0));
	vec4 texture_color_Right = texture(tex,fs_in.texcoord+vec2(0.005,0.0));
	vec4 texture_color = vec4(texture_color_Left.r*0.29+texture_color_Left.g*0.58+texture_color_Left.b*0.114,texture_color_Right.g,texture_color_Right.b,0.0);
	return texture_color;
}


vec4 pixelation()
{
    float Pixels = 600.0;
    float dx = 15.0 * (1.0 / Pixels);
    float dy = 15.0 * (1.0 / Pixels);
    vec2 Coord = vec2(dx * floor(fs_in.texcoord.x / dx),dy * floor(fs_in.texcoord.y / dy));
    vec4 FinalColor = texture(tex, Coord);
	return FinalColor;
}
vec4 sin_wave()
{   
    vec2 aux = fs_in.texcoord;
    aux.x+= 0.05*sin(aux.y * 1.0*3.14 + offset);
	
    vec4 finalcolor =  texture(tex, aux);
	return finalcolor;
}
vec4 bloom()
{   
    vec2 size =vec2(600,600);
	int samples=5;
	float quality=2.5;
	
    vec4 colour=texture(tex,fs_in.texcoord);
	float brightness= (colour.r*0.2126)+(colour.g*0.7152)+(colour.b*0.0722);
	vec4 color=colour * brightness;
	
	vec4 source = texture(tex,fs_in.texcoord);
    vec4 sum = vec4(0);
    int diff = (samples - 1) / 2;
    vec2 sizeFactor = vec2(1) / size * quality;
  
    for (int x = -diff; x <= diff; x++)
    {
        for (int y = -diff; y <= diff; y++)
        {
            vec2 offset = vec2(x, y) * sizeFactor;
            sum += texture(tex, fs_in.texcoord + offset);
        }
    }
  
    return ((sum / (samples * samples)) + source) * color;
	//return color;
	
}
vec4 halftone()
{   
    int pixelsPerRow=10000;
    vec2 p = fs_in.texcoord;
	float pixelSize = 1.0 / float(pixelsPerRow);
	
	float dx = mod(p.x, pixelSize) - pixelSize*0.5;
	float dy = mod(p.y, pixelSize) - pixelSize*0.5;
	
	p.x -= dx;
	p.y -= dy;
	vec3 col = texture(tex, p).rgb;
	float bright = 0.3333*(col.r+col.g+col.b);
	
	float dist = sqrt(dx*dx + dy*dy);
	float rad = bright * pixelSize * 0.72;
	float m = step(dist, rad);

	vec3 col2 = mix(vec3(0.0), vec3(5.0), m);
	vec4 finalcolor = vec4(col2, 1.0);
	return finalcolor;
}

vec4 cool_shader()
{   
    float radius = 200.0;
	float angle = 0.8;
	vec2 center = vec2(400.0, 300.0);
    vec2 texSize = vec2(600, 600);
    vec2 tc = fs_in.texcoord * texSize;
    tc -= center;
    float dist = length(tc);
    if (dist < radius) 
    {
        float percent = (radius - dist) / radius;
        float theta = percent * percent * angle * 8.0;
        float s = sin(theta);
        float c = cos(theta);
        tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
    }
    tc += center;
    vec3 color = texture(tex, tc / texSize).rgb;
    return vec4(color, 1.0);
	
	
	
}

vec4 cool_shader2()
{  
    float exposure =1.0;
    float decay=1.0;
    float density=1.0;
    float weight=1.0;
    vec2 lightPositionOnScreen=vec2(10,10);
    vec4 finalcolor=vec4(0,0,0,0);
    const int NUM_SAMPLES = 100 ;
    vec2 deltaTextCoord = vec2( fs_in.texcoord - lightPositionOnScreen.xy );
    vec2 textCoo = fs_in.texcoord;
    deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
    float illuminationDecay = 1.0;
	
	
    for(int i=0; i < NUM_SAMPLES ; i++)
    {
        textCoo -= deltaTextCoord;
        vec4 sample_ = texture2D(tex, textCoo );
			
        sample_ *= illuminationDecay * weight;

        finalcolor += sample_;

        illuminationDecay *= decay;
    }
    finalcolor *= exposure;
    return finalcolor;
}
void main(void)
{   
    if(bar_on==0)
	{
	    color = texture(tex,fs_in.texcoord);
	}
	else if(bar_on==1)
	{
	    if(gl_FragCoord.x<300)
	    {   
		    color = texture(tex,fs_in.texcoord);
	    }
	    
	    else
	    {   
		    if(state==0)
	            color = texture(tex,fs_in.texcoord);
			else if(state==1)
			{
			    vec4 color1=blur();
				vec4 color2=quantization();
				vec4 color3=dog();
				color = (blur()+quantization())/2*dog();
			}
                			
			else if(state==2)
			    color=red_blue();
			else if(state==3)
			    color=cool_shader2();
			else if(state==4)
			    color=pixelation();
			else if(state==5)
			    color=sin_wave();
			else if(state==6)
			    color=bloom();
			else if(state==7)
			    color=halftone();
			else if(state==8)
			    color=cool_shader();
			
	    }  
	}
    
        
}
