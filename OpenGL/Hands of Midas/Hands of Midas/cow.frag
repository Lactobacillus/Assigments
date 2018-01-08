#version 400 core

in vec3 L;
in vec3 VV;
in vec3 N;
in vec3 verpos;

out vec4 color;

flat in int render1;
flat in int render2;

uniform vec3 LightPosW;
uniform samplerCube text;

void main () {


   // Diffuse
   vec3 a = normalize(L);
   vec3 b = normalize(N);
   float cos = max(dot(a, b), 0);
   vec3 lightcolor = vec3(1.0, 1.0, 1.0);
   vec3 cowcolor = vec3(1.0, 0.5, 0.2);
   float distance = length(L); 
   float lightpower = 200.0;
   vec3 diffuse = (cos * lightcolor * cowcolor * lightpower) / (distance * distance);

   //for original
   vec3 cowcolor2 = vec3(0.7,0.45, 0.3);
   vec3 diffuse2 = (cos * lightcolor * cowcolor2 * lightpower) / (distance * distance);



   // Ambient
   float ambientstrength = 0.1;
   vec3 ambient = ambientstrength * lightcolor * cowcolor;

   //for original
   vec3 ambient2 =  ambientstrength * lightcolor * cowcolor2;

   // Specular
   float specularlightpower = 200.0;
   vec3 reflect = reflect(-a, b);
   vec3 v = normalize(VV);
   float cosalpha = pow(max(dot(v, reflect), 0.0), 2.0);
   vec3 specular = lightcolor * cosalpha * cowcolor * specularlightpower / (distance * distance);
   
   //for metalcolor
   vec3 specular3 = lightcolor * cosalpha * specularlightpower / (distance * distance);


   //for ice
   float cosalpha2 = pow(max(dot(v, reflect), 0.0), 10.0);
   vec3 specular2 = lightcolor * cosalpha2* specularlightpower / (distance * distance);


   // Cartoon render
   vec4 tooncolor;

   if (cos > 0.98) {

      tooncolor = vec4(0.8, 0.8, 0.5, 1.0) * vec4(cowcolor, 1.0);
   }
   else if (cos> 0.5) {

      tooncolor = vec4(0.8, 0.8, 0.4, 1.0) * vec4(cowcolor, 1.0);
   }
   else if (cos > 0.25) {

      tooncolor = vec4(0.6, 0.6, 0.2, 1.0) * vec4(cowcolor, 1.0);
   }
   else {

      tooncolor = vec4(0.2, 0.2, 0.1, 1.0);
   }

   //light rendering
   //color = vec4(specular+diffuse+ambient,1.0);

   //reflect environment mapping
   vec3 rmv = reflect(normalize(-VV),N);
   vec4 reflectenv = vec4(texture(text,rmv).rgb,1.0);

   //refract environment mapping
   float air = 1.00;
   //float water = 1.33;
   float ice = 1.309;
   float glass = 1.52;
   //float diamond = 2.42;

   // ice
   float ice_ratio = air/ice;
   vec3 ice_r2mv = refract(normalize(-VV),b,ice_ratio);
   vec4 ice_reflectenv = vec4(texture(text,ice_r2mv).rgb,1.0);

   // glass
   float glass_ratio = air/ glass;
   vec3 glass_r2mv = refract(normalize(-VV),b,glass_ratio);
   vec4 glass_reflectenv = vec4(texture(text,glass_r2mv).rgb,1.0);



   vec4 original = vec4((diffuse2 + ambient), 1.0);
   vec4 iceColor = vec4((ice_reflectenv.xyz)+specular2, 1.0);
   vec4 goldColor = (reflectenv + vec4(specular, 1.0)) * vec4(0.95, 0.55, 0.25, 1.0) + original;
   vec4 metalColor = reflectenv + vec4(specular3, 1.0);
   vec4 glassColor = glass_reflectenv + vec4(specular, 1.0);
   
   // 0 : base
   // 1 : ice
   // 2 : cartoon
   // 3 : gold
   // 4 : metal
   // 5 : glass

   vec4 beforeColor = vec4(1.0, 1.0, 1.0, 1.0);
   vec4 afterColor = vec4(1.0, 1.0, 1.0, 1.0);

   if (render2 == 0) beforeColor = original;
   else if (render2 == 1) beforeColor = iceColor;
   else if (render2 == 2) beforeColor = tooncolor;
   else if (render2 == 3) beforeColor = goldColor;
   else if (render2 == 4) beforeColor = metalColor;
   else if (render2 == 5) beforeColor = glassColor;

   if (render1 == 0) afterColor = original;
   else if (render1 == 1) afterColor = iceColor;
   else if (render1 == 2) afterColor = tooncolor;
   else if (render1 == 3) afterColor = goldColor;
   else if (render1 == 4) afterColor = metalColor;
   else if (render1 == 4) afterColor = glassColor;



   color = 0.5 * beforeColor + 0.5 * afterColor;
 }