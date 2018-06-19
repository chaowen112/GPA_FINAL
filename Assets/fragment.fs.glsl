#version 410

layout(location = 0) out vec4 fragColor;

uniform mat4 um4mv;
uniform mat4 um4p;

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 V;
    vec3 H; // eye space halfway vector
    vec2 texcoord;
} vertexData;

uniform sampler2D tex;

void main()
{
    vec3 sun = normalize(vec3(5000.0,3000.0,100000.0));
    vec3 N = vertexData.N;
    vec3 L = vertexData.L;
    vec3 H = vertexData.H;
    vec3 V = vertexData.V;
    float theta = max(dot(N,sun), 0.0);
    float phi = max(dot(H,N), 0.0);
    vec3 texColor = texture(tex, vertexData.texcoord).rgb;
    vec3 ambient = texColor * vec3(0.5,0.5,0.5);//[TODO] ambient = Ka*Ia
    vec3 diffuse = texColor * vec3(0.8,0.8,0.8) * theta;//[TODO] diffuse = Kd*Id*theta
    vec3 specular = vec3(1.0,1.0,1.0) * pow(phi,100);//[TODO] specular = Ks*Is*pow(phi,shinness)
    fragColor = vec4(ambient + diffuse + specular, 1.0);
    //fragColor = vec4(texColor, 1.0);
	
}
    
