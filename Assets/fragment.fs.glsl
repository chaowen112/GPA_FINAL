#version 410

layout(location = 0) out vec4 fragColor;

uniform mat4 um4mv;
uniform mat4 um4p;
uniform int is_capsule;
in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 V;
    vec3 H; // eye space halfway vector
    vec2 texcoord;
    vec4 FragPosLightSpace;
} vertexData;

uniform sampler2D tex;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    
    return shadow;
}

void main()
{
    if(is_capsule==1)
	{
	    vec3 sun = normalize(vec3(0.0,80.0,-100.0));
        vec3 N = vertexData.N;
        vec3 L = vertexData.L;
        vec3 H = vertexData.H;
        vec3 V = vertexData.V;
        float theta = max(dot(N,L), 0.0);
        float phi = max(dot(N,H), 0.0);
        vec3 texColor = vec3(1.0,2.0,3.0);
        vec3 ambient = texColor * vec3(0.3,0.3,0.3);//[TODO] ambient = Ka*Ia
        vec3 diffuse = texColor * vec3(0.5,0.5,0.5) * theta;//[TODO] diffuse = Kd*Id*theta
        vec3 specular = vec3(1.0,1.0,1.0) * pow(phi,100);//[TODO] specular = Ks*Is*pow(phi,shinness)
        fragColor = vec4(ambient + diffuse + specular,1.0);
		
		
	}
	else
	{
	    vec3 sun = normalize(vec3(100.0,30.0,100.0));
        vec3 N = vertexData.N;
        vec3 L = vertexData.L;
        vec3 H = vertexData.H;
        vec3 V = vertexData.V;
        float theta = max(dot(N,L), 0.0);
        float phi = max(dot(N,H), 0.0);
        vec3 texColor = texture(tex, vertexData.texcoord).rgb;
        vec3 ambient = texColor * vec3(0.3,0.3,0.3);//[TODO] ambient = Ka*Ia
        vec3 diffuse = texColor * vec3(0.5,0.5,0.5) * theta;//[TODO] diffuse = Kd*Id*theta
        vec3 specular = vec3(1.0,1.0,1.0) * pow(phi,100);//[TODO] specular = Ks*Is*pow(phi,shinness)
        float shadow = ShadowCalculation(vertexData.FragPosLightSpace);
        fragColor = vec4(ambient + (1-shadow)*(diffuse + specular), 1.0);
	}
}



