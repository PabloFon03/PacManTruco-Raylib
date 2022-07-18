#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform float flipVal;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);

    // NOTE: Implement here your fragment shader code

    // (224 - 48) / 448
    float UIStart = 0.392857142857;
    float lerpVal = step(UIStart, abs(fragTexCoord.y - 0.5));

    float scale = 1 / (1 - flipVal);
    
    float xVal = fragTexCoord.x * scale - 0.5 * flipVal * scale;
    float yVal = mix(fragTexCoord.y, (fragTexCoord.y - 0.5) * 0.25 * (fragTexCoord.x - 0.5) + 0.5, flipVal);

    float boundCheck = step(0, xVal) * step(xVal, 1) * step(0, yVal) * step(yVal, 1) * step(flipVal, 0.999);
    vec4 mazeColor = mix(vec4(0, 0, 0, 1), texture(texture0, vec2(xVal, yVal)), boundCheck);

    finalColor = mix(mazeColor, texelColor, lerpVal);
}