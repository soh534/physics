#version 400

// Lighting
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec4 vertColor;
out vec4 fragColor;

in vec3 fragPos;
in vec3 normVector;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize( normVector );
    vec3 lightDir = normalize( lightPos - fragPos );
    float diff = max( dot( norm, lightDir ), 0.f );
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize( viewPos - fragPos );
    vec3 reflectDir = reflect( -lightDir, norm );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), 32 );
    vec3 specular = specularStrength * spec * lightColor;

    fragColor = vec4( ambient + diffuse + specular, 1.0f ) * vertColor;
}
