#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float k_c;
    float k_l;
    float k_q;
};

struct DirectionalLight {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutoff;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
    float outerCutOff;
    float k_c;
    float k_l;
    float k_q;
};

#define MAX_POINT_LIGHTS 20
#define MAX_SPOT_LIGHTS 8

uniform vec3 viewPos;
uniform int numPointLights;
uniform int numSpotLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform DirectionalLight dirLight;
uniform DirectionalLight dirLight2;
uniform Material material;
uniform vec3 globalAmbient;

uniform bool useTexture;
uniform bool blendWithColor;
uniform sampler2D texture1;
uniform float texTiling;
uniform bool useProceduralWall;
uniform bool useProceduralShop;
uniform float alpha;

// Day/Night interpolation factor (0.0 = full day, 1.0 = full night)
uniform float dayNightMix;

vec3 CalcPointLight(Material mat, PointLight light, vec3 N, vec3 Pos, vec3 V);
vec3 CalcDirLight(Material mat, DirectionalLight light, vec3 N, vec3 Pos, vec3 V);
vec3 CalcSpotLight(Material mat, SpotLight light, vec3 N, vec3 Pos, vec3 V);

// Procedural stone/brick wall texture using mathematical functions
vec3 proceduralWallColor(vec3 pos) {
    float brickW = 2.0;
    float brickH = 0.8;
    float mortarT = 0.05;
    float row = floor(pos.y / brickH);
    float offset = mod(row, 2.0) * brickW * 0.5;
    float brickX = pos.x + pos.z + offset;
    float bx = mod(brickX, brickW);
    float by = mod(pos.y, brickH);
    float mx = smoothstep(0.0, mortarT, bx) * smoothstep(0.0, mortarT, brickW - bx);
    float my = smoothstep(0.0, mortarT, by) * smoothstep(0.0, mortarT, brickH - by);
    float brick = mx * my;
    float n = fract(sin(dot(vec2(floor(brickX / brickW), row), vec2(12.9898, 78.233))) * 43758.5453);
    vec3 brickCol = mix(vec3(0.72, 0.62, 0.50), vec3(0.82, 0.72, 0.58), n);
    float n2 = fract(sin(dot(pos.xz * 3.7, vec2(23.14, 41.07))) * 3482.17);
    brickCol += (n2 - 0.5) * 0.05;
    vec3 mortarCol = vec3(0.88, 0.86, 0.82);
    return mix(mortarCol, brickCol, brick);
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    Material mat = material;
    if(useTexture) {
        float tl = (texTiling > 0.0) ? texTiling : 1.0;
        vec3 tc = texture(texture1, TexCoords * tl).rgb;
        if(blendWithColor) {
            mat.ambient = tc * material.ambient;
            mat.diffuse = tc * material.diffuse;
        } else {
            mat.ambient = tc * 0.4;
            mat.diffuse = tc;
        }
    }
    if(useProceduralWall) {
        vec3 wc = proceduralWallColor(FragPos);
        mat.ambient = wc * 0.6;
        mat.diffuse = wc;
    }
    if(useProceduralShop) {
        float size = 5.0;
        float ax = abs(fract(FragPos.x * size + FragPos.z * size) - 0.5);
        float ay = abs(fract(FragPos.y * size) - 0.5);
        float diamond = smoothstep(0.35, 0.45, ax + ay);
        vec3 shopCol = mix(mat.diffuse * 0.75, mat.diffuse, diamond);
        mat.ambient = shopCol * 0.5;
        mat.diffuse = shopCol;
    }

    // Day/Night ambient blending
    vec3 dayAmbient = globalAmbient;
    vec3 nightAmbient = globalAmbient * 0.15;
    vec3 effectiveAmbient = mix(dayAmbient, nightAmbient, dayNightMix);
    vec3 result = mat.ambient * effectiveAmbient;

    // Point lights
    for (int i = 0; i < numPointLights; i++)
    {
        result += CalcPointLight(mat, pointLights[i], N, FragPos, V);
    }

    // Directional lights (sun/lampposts)
    result += CalcDirLight(mat, dirLight, N, FragPos, V);
    result += CalcDirLight(mat, dirLight2, N, FragPos, V);

    // Spot lights (store highlights)
    for (int i = 0; i < numSpotLights; i++)
    {
        result += CalcSpotLight(mat, spotLights[i], N, FragPos, V);
    }

    FragColor = vec4(result, alpha);
}

vec3 CalcPointLight(Material mat, PointLight light, vec3 N, vec3 Pos, vec3 V)
{
    vec3 L = normalize(light.position - Pos);
    vec3 R = reflect(-L, N);

    float d = length(light.position - Pos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * d * d);

    vec3 ambient  = mat.ambient * light.ambient * attenuation;
    vec3 diffuse  = mat.diffuse * max(dot(N, L), 0.0) * light.diffuse * attenuation;
    vec3 specular = mat.specular * pow(max(dot(V, R), 0.0), mat.shininess) * light.specular * attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(Material mat, DirectionalLight light, vec3 N, vec3 Pos, vec3 V)
{
    vec3 fragToLight = normalize(light.position - Pos);
    vec3 lightDir = normalize(light.direction);
    float theta = dot(-fragToLight, lightDir);

    vec3 ambient = mat.ambient * light.ambient;
    vec3 diffuse  = vec3(0.0);
    vec3 specular = vec3(0.0);

    if (theta > light.cutoff)
    {
        vec3 L = fragToLight;
        vec3 R = reflect(-L, N);
        diffuse  = mat.diffuse * max(dot(N, L), 0.0) * light.diffuse;
        specular = mat.specular * pow(max(dot(V, R), 0.0), mat.shininess) * light.specular;
    }

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(Material mat, SpotLight light, vec3 N, vec3 Pos, vec3 V)
{
    vec3 L = normalize(light.position - Pos);
    float d = length(light.position - Pos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * d * d);

    // Spotlight cone with soft edge
    float theta = dot(normalize(-L), normalize(light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 R = reflect(-L, N);
    vec3 ambient  = mat.ambient * light.ambient * attenuation;
    vec3 diffuse  = mat.diffuse * max(dot(N, L), 0.0) * light.diffuse * attenuation * intensity;
    vec3 specular = mat.specular * pow(max(dot(V, R), 0.0), mat.shininess) * light.specular * attenuation * intensity;

    return (ambient + diffuse + specular);
}
