#version 330 core
uniform vec3 eyePos;
//uniform mat4 viewMatrix;
//uniform vec3 eyeDir
in vec3 outray;
out vec4 color;

struct Intersection{
      float t;
      vec3 pos;
      vec3 normal;
      vec4 color;
      float specular;
};


Intersection intersectSphere(vec3 center, float radius, vec3 pos, vec3 dir){
   Intersection i;
   i.t=-1.0;
   float a = 1.0;//len of dir squared, normalized so =1
   vec3 bprim = 2.0*dir*(pos-center);
   float b=bprim.x+bprim.y+bprim.z;
   float c=center.x*center.x+center.y*center.y+center.z*center.z + pos.x*pos.x+pos.y*pos.y+pos.z*pos.z + (-2.0)*(center.x*pos.x+center.y*pos.y+center.z*pos.z) - radius*radius;
   float delta = b*b-4.0*a*c;
   if(delta<0.0){
      return i;
   }
   float t = (-b-sqrt(delta))/2.0*a;
   if(t>0.0){
      i.t=t;
      i.pos=pos+dir*t;
      i.normal = normalize(pos-center);
      i.color = vec4(1.0,0.0,0.0,1.0);
   }
   return i;
}



vec4 castRay(vec3 pos, vec3 dir){
   vec3 lightPosition = vec3(-40.0,4.0,-20.0);//TODO uniform
   Intersection intersection;

   intersection = intersectSphere(vec3(4.0,2.0,20.0),7.0,pos,dir);
   if(intersection.t>0.0){
      float ambient = 0.25;
      vec3 L = normalize(lightPosition - intersection.pos);
      float diffuse = max(dot(intersection.normal, L), 0.0);
      return intersection.color*(ambient+diffuse);
   }
   return vec4(0.0);
}

const float focalLength = 1.0;

void main()
{
    //vec3 eyePos = vec3(0);//TODO uniform
    //vec2 screenPos = vec2(gl_FragCoord.x*(1.0/800.0)-0.5,gl_FragCoord.y*(1.0/600.0)-0.5);//,focalLength,1.0);//TODO lens
    //screenPos = viewMatrix * screenPos;
    //vec3 point = screenPos.x*right+screenPos.y*up+direction;
    //vec3 dir = normalize(point);
    //gl_FragColor = castRay(eyePos,ray.xyz);//vec4(screenPos.x,screenPos.y,screenPos.z,1.0);
    color = vec4(outray,1.0);//vec4(outray,1.0);
}
