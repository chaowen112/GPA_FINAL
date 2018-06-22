#version 410 core                                                                                                                 
in VS_OUT                                                         
{                                                                 
    vec3 normal;                                                  
} fs_in;                                                          
                                                                   
out vec4 frag_normal;                                             
                                                                      
void main(void)                                                   
{                                                                 
    frag_normal = vec4(normalize(fs_in.normal), 0.0);             
}                                                                 