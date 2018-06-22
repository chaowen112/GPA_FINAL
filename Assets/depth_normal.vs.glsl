#version 410 core                                 
                                                     
uniform mat4 mv_matrix;                           
uniform mat4 proj_matrix;                         
                                                   
layout (location = 0) in vec3 position;           
layout (location = 1) in vec3 normal;             
out VS_OUT                                        
{                                                 
    vec3 normal;                                 
} vs_out;                                         
                                                    
void main(void)                                   
{                                                 
    vs_out.normal = mat3(mv_matrix) * normal;     
    vec4 pos_vs = mv_matrix * vec4(position, 1.0);
    gl_Position = proj_matrix * pos_vs;           
}                                                 