#version 330 core
uniform sampler3D VolumeTexture;
uniform vec3 eyePos;
uniform mat4 matrix;

uniform vec3 lightPosition = vec3(-40,-20,100);

uniform vec2 WindowSize;
//uniform vec3 eyeDir
in vec3 outray;
out vec4 color;

#define DEBUG
#define LOCAL_SHADOWS

const float epsilonTolerance = 0.01;
const float epsilonMove = 0.0001;
const float epsilonGradient = 0.5/128.0;//quite a big component (makes edges blurry) but if it's smaller artifacts start to appear (surface becomes rough)
const int maxRays = 200;

struct Intersection{
      float t;
      vec3 pos;
      vec3 normal;
      vec3 debug;
};
struct Ray{
   vec3 org;
   vec3 dir;
};

Intersection noIntersect = Intersection(-1.0,vec3(0),vec3(0),vec3(0));

bool insideAABB(vec3 lb,vec3 rt,vec3 r){//TODO can be optimized by vector ops (+-)
   if(r.x>lb.x && r.y>lb.y && r.z>lb.z
      &&
      r.x<rt.x && r.y<rt.y && r.z<rt.z )
      return true;
   return false;
}

Intersection intersectAABB(vec3 lb,vec3 rt,Ray ray){
   if(insideAABB(lb,rt,ray.org)){
      return Intersection(0.1,ray.org,vec3(0),vec3(0));
   }
   // r.dir is unit direction vector of ray
   vec3 dirfrac;
   dirfrac.x = 1.0f / ray.dir.x;
   dirfrac.y = 1.0f / ray.dir.y;
   dirfrac.z = 1.0f / ray.dir.z;
   // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
   // ray.org is origin of ray
   float t1 = (lb.x - ray.org.x)*dirfrac.x;
   float t2 = (rt.x - ray.org.x)*dirfrac.x;
   float t3 = (lb.y - ray.org.y)*dirfrac.y;
   float t4 = (rt.y - ray.org.y)*dirfrac.y;
   float t5 = (lb.z - ray.org.z)*dirfrac.z;
   float t6 = (rt.z - ray.org.z)*dirfrac.z;

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
   vec3 point = ray.org+tmin*ray.dir;
   return Intersection(tmin,point,vec3(0),vec3(0));
}

const vec3 chunkPos = vec3(0,0,0);
const float chunkScale = 128.0;

float sampleSDF(vec3 pos){
  return texture(VolumeTexture,(pos-chunkPos)/chunkScale).r;
}

vec3 computeNormal(vec3 pos){
  vec3 coord = (pos-chunkPos)/chunkScale;
  vec3 normal;
  normal.x = texture(VolumeTexture,coord+vec3(epsilonGradient,0,0)).r-texture(VolumeTexture,coord-vec3(epsilonGradient,0,0)).r;
  normal.y = texture(VolumeTexture,coord+vec3(0,epsilonGradient,0)).r-texture(VolumeTexture,coord-vec3(0,epsilonGradient,0)).r;
  normal.z = texture(VolumeTexture,coord+vec3(0,0,epsilonGradient)).r-texture(VolumeTexture,coord-vec3(0,0,epsilonGradient)).r;
  return normalize(normal);
}

Intersection castRay(Ray ray){
  vec3 lt = chunkPos;
  vec3 rb = chunkPos + vec3(chunkScale);
   Intersection intersection = intersectAABB(lt,rb,ray);
   if(intersection.t>=0.0){
      ray.org = intersection.pos+ray.dir*epsilonMove;
      int steps=0;
      while(steps<maxRays
            && insideAABB(lt,rb,ray.org)
            && abs(sampleSDF(ray.org))>epsilonTolerance){
         float dist = sampleSDF(ray.org);
         intersection.t+=dist;
         ray.org+=ray.dir*dist;
         steps++;
      }
      #ifdef DEBUG
        //color=vec3(0,0,5.0*steps/float(maxRays));
        //TODO somehow show how many rays
      #endif
      if(insideAABB(lt,rb,ray.org) && sampleSDF(ray.org)<=epsilonTolerance){
        intersection.pos = ray.org;
        intersection.normal = computeNormal(ray.org);//may defer it to latter step as for shadow its not needed
        return intersection;
      }
      intersection = noIntersect;
      intersection.debug = vec3(0,0,5.0*float(steps)/float(maxRays));
      return intersection;
   }
   return noIntersect;
}

void main()
{
   float FocalLength = 1/tan(0.25*3.1415926535897932384626433832795);
   vec3 rayDirection;
   rayDirection.xy = 2.0 * gl_FragCoord.xy / WindowSize - 1.0;
   rayDirection.z = -FocalLength;
   rayDirection = (vec4(rayDirection, 0) * matrix).xyz;

   Intersection intersection = castRay(Ray(eyePos,rayDirection));

   if(intersection.t<0.0){
     #ifdef DEBUG
     if(intersection.debug!=vec3(0)){
       color = vec4(intersection.debug,1.0);
       return;
     }
     else
     #endif
      discard;
   }

   vec3 material = vec3(1,0,0);
   float ambient = 0.2;

   #ifdef LOCAL_SHADOWS
      vec3 shadowRay = lightPosition-intersection.pos;
      vec3 shadowDir = normalize(shadowRay);
      Intersection shadow = castRay(Ray(intersection.pos+shadowDir*0.3,shadowDir));
      if(shadow.t>0.0 && shadow.t<length(shadowRay)){//there was something in between point and light
          color = vec4(material*ambient,1.0);//only ambient light in that case, because object is occluded
          return;
      }
   #endif

   vec3 lightDir = normalize(lightPosition-intersection.pos);
   vec3 eyeDir = normalize(eyePos);

   float diffuse = max(dot(intersection.normal,lightDir),0.0);


   color = vec4(max(material*diffuse,material*ambient),1.0);
}
