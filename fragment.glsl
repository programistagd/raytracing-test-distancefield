#version 330 core
uniform sampler3D VolumeTexture;
uniform vec3 eyePos;
uniform mat4 matrix;

uniform vec2 WindowSize;
//uniform vec3 eyeDir
in vec3 outray;
out vec4 color;

#define DEBUG

const float epsilonTolerance = 0.01;
const float epsilonMove = 0.001;
const float epsilonGradient = 0.5/128.0;
const int maxRays = 200;

struct Intersection{
      float t;
      vec3 pos;
      vec3 normal;
      vec3 color;
      float specular;
};
struct Ray{
   vec3 org;
   vec3 dir;
};

Intersection noIntersect = Intersection(-1.0,vec3(0),vec3(0),vec3(0),0.0);

bool insideAABB(vec3 lb,vec3 rt,vec3 r){//TODO can be optimized by vector ops (+-)
   if(r.x>lb.x && r.y>lb.y && r.z>lb.z
      &&
      r.x<rt.x && r.y<rt.y && r.z<rt.z )
      return true;
   return false;
}

Intersection intersectAABB(vec3 lb,vec3 rt,Ray r){
   if(insideAABB(lb,rt,r.org)){
      return Intersection(0.1,r.org,vec3(0),vec3(0,1,1),0.0);
   }
   // r.dir is unit direction vector of ray
   vec3 dirfrac;
   dirfrac.x = 1.0f / r.dir.x;
   dirfrac.y = 1.0f / r.dir.y;
   dirfrac.z = 1.0f / r.dir.z;
   // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
   // r.org is origin of ray
   float t1 = (lb.x - r.org.x)*dirfrac.x;
   float t2 = (rt.x - r.org.x)*dirfrac.x;
   float t3 = (lb.y - r.org.y)*dirfrac.y;
   float t4 = (rt.y - r.org.y)*dirfrac.y;
   float t5 = (lb.z - r.org.z)*dirfrac.z;
   float t6 = (rt.z - r.org.z)*dirfrac.z;

   float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
   float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

   // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
   if (tmax < 0)
   {
      return noIntersect;
   }

   // if tmin > tmax, ray doesn't intersect AABB
   if (tmin > tmax)
   {
      return noIntersect;
   }
   vec3 point = r.org+tmin*r.dir;
   return Intersection(tmin,point,vec3(0),vec3(0,1,0),0.0);
}

vec3 computeNormal(vec3 coord){
  vec3 normal;
  normal.x = texture(VolumeTexture,coord+vec3(epsilonGradient,0,0)).r-texture(VolumeTexture,coord-vec3(epsilonGradient,0,0)).r;
  normal.y = texture(VolumeTexture,coord+vec3(0,epsilonGradient,0)).r-texture(VolumeTexture,coord-vec3(0,epsilonGradient,0)).r;
  normal.z = texture(VolumeTexture,coord+vec3(0,0,epsilonGradient)).r-texture(VolumeTexture,coord-vec3(0,0,epsilonGradient)).r;
  return normalize(normal);
}

vec3 castRay(Ray ray){
   vec3 corner = vec3(0,0,0);
   vec3 lightPosition = eyePos;//vec3(-50.0,40.0,70.0);//TODO uniform
   vec3 color=vec3(0);
   Intersection intersection = intersectAABB(vec3(0,0,0),vec3(128,128,128),ray);
   if(intersection.t>=0.0){
      vec3 coord = (intersection.pos+ray.dir*epsilonMove-corner)/128.0;
      //color = vec3(texture(VolumeTexture,coord).r);
      int steps=0;
      while(steps<maxRays
         && insideAABB(vec3(0,0,0),vec3(1,1,1),coord)
         && abs(texture(VolumeTexture,coord).r)>epsilonTolerance){
         float dist = texture(VolumeTexture,coord).r;
         coord+=ray.dir*dist/128.0;
         steps++;
      }
      #ifdef DEBUG
        color=vec3(0,0,5.0*steps/float(maxRays));
      #endif
      if(insideAABB(vec3(0,0,0),vec3(1,1,1),coord) && texture(VolumeTexture,coord).r<=epsilonTolerance){
        vec3 material = vec3(1,0,0);
        float ambient = 0.2;

        vec3 normal = computeNormal(coord);
        vec3 lightDir = normalize(lightPosition-coord*128.0);//TODO proper
        vec3 eyeDir = normalize(eyePos);

        float diffuse = max(dot(normal,lightDir),0.0);
        /*float spec = 0.0;
        if(diffuse>0.0){
          vec3 h = normalize(lightDir+eyeDir);
          spec = max(dot(h,normal),0.0);
        }*/
        color = max(material*diffuse,material*ambient);
         //color = max(material*diffuse+vec3(1,1,1)*spec,material*ambient);
      }
   }
   return color;
}

void main()
{
   float FocalLength = 1/tan(0.25*3.1415926535897932384626433832795);
   vec3 rayDirection;
   rayDirection.xy = 2.0 * gl_FragCoord.xy / WindowSize - 1.0;
   rayDirection.z = -FocalLength;
   rayDirection = (vec4(rayDirection, 0) * matrix).xyz;
   color = vec4(castRay(Ray(eyePos,rayDirection)),1.0);
   //color = vec4(abs(normalize(outray)),1.0);
}
