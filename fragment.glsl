#version 330 core
uniform sampler3D VolumeTexture;
uniform vec3 eyePos;
uniform mat4 matrix;
//uniform vec3 eyeDir
in vec3 outray;
out vec4 color;

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

Intersection intersectAABB(vec3 lb,vec3 rt,Ray r){
   if(r.org.x>lb.x && r.org.y>lb.y && r.org.z>lb.z
      &&
      r.org.x<rt.x && r.org.y<rt.y && r.org.z<rt.z ){
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


vec3 castRay(Ray ray){
   vec3 lightPosition = vec3(-50.0,40.0,70.0);//TODO uniform
   vec3 color=vec3(0);
   Intersection intersection = intersectAABB(vec3(0,0,0),vec3(128,128,128),ray);

   if(intersection.t>=0.0){
      vec3 coord = (intersection.pos-vec3(0,0,0))/128.0;
      color = vec3(texture(VolumeTexture,coord).r);
   }
   return color;
}


const vec2 WindowSize = vec2(800,600);//TODO uniform

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
