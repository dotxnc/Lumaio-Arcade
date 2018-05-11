/*    Phosphorish
    Copyright (C) 2011 Themaister
    This program is licensed in the public domain
*/

#version 330

uniform sampler2D texture0;
uniform vec2 screen_size = vec2(512, 512);

in vec2 fragPos;
in vec2 fragTexCoord;

out vec4 finalColor;

vec3 to_focus(float pixel)
{
   pixel = mod(pixel + 3.0, 3.0);
   if (pixel >= 2.0) // Blue
      return vec3(pixel - 2.0, 0.0, 3.0 - pixel);
   else if (pixel >= 1.0) // Green
      return vec3(0.0, 2.0 - pixel, pixel - 1.0);
   else // Red
      return vec3(1.0 - pixel, pixel, 0.0);
}

void main()
{
   float y = mod(fragTexCoord.y * screen_size.y, 1.0);
   float intensity = exp(-0.2 * y);

   vec2 one_x = vec2(1.0 / (3.0 * screen_size.x), 0.0);

   vec3 color = texture(texture0, fragTexCoord.xy - 0.0 * one_x).rgb;
   vec3 color_prev = texture(texture0, fragTexCoord.xy - 1.0 * one_x).rgb;
   vec3 color_prev_prev = texture(texture0, fragTexCoord.xy - 2.0 * one_x).rgb;

   float pixel_x = 3.0 * fragTexCoord.x * screen_size.x;

   vec3 focus = to_focus(pixel_x - 0.0);
   vec3 focus_prev = to_focus(pixel_x - 1.0);
   vec3 focus_prev_prev = to_focus(pixel_x - 2.0);

   vec3 result =
      0.8 * color * focus +
      0.6 * color_prev * focus_prev +
      0.3 * color_prev_prev * focus_prev_prev;

   result = 2.3 * pow(result, vec3(1.4));

   finalColor = vec4(intensity * result, 1.0);
}