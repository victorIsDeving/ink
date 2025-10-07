

//uniform vec2 iResolution;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
uniform vec2 iResolution;
uniform vec4 iMouse;
uniform float iTime;
uniform int iFrame;
out vec4 C;



// Particle Buffer
// in this buffer every pixel represents a particle
// the particles positions is stored in .xy
//           its velocity  is stored in .zw
// Only the first PARTICLES amount of pixels are actually used.


//returns the ids of the four closest particles from the input
ivec4 getClosest(ivec2 xy){
    return ivec4(texelFetch(iChannel1, xy, 0));
}

//returns the location of the particle within the particle buffer corresponding with the input id
ivec2 locFromID(int id){
    int width = int(iResolution.x);
    return ivec2( id % width, id / width);
}

//get the particle corresponding to the input id
vec4 getParticle(int id){
    return texelFetch(iChannel0, locFromID(id), 0);
}

float distance2Particle(int id, vec2 fragCoord){
    if(id==-1) return 1e20;
    vec2 delta = getParticle(id).xy-fragCoord;
    return dot(delta, delta);
}


vec2 N22(vec2 p,float t)
{
    vec3 a = fract(p.xyx*vec3(123.34,234.34,345.64));
    a=cross(t*a,a+34.45);
    return fract(vec2(a.x*a.z,a.y*a.z));//*2.0 - vec2(1.0);
}


vec2 getVoronoiPolygonCenter(int id) {
     vec2 c[8], n[8];
     vec4 particle = getParticle(id);
     vec2 rm = particle.xy;
     ivec2 iFragCoord = ivec2(rm);
     ivec4 old   = getClosest( iFragCoord + ivec2( 0, 0) );
     ivec4 east  = getClosest( iFragCoord + ivec2( 1, 0) );
         ivec4 north = getClosest( iFragCoord + ivec2( 0, 1) );
     ivec4 west  = getClosest( iFragCoord + ivec2(-1, 0) );
     ivec4 south = getClosest( iFragCoord + ivec2( 0,-1) );
     ivec4 northeast = getClosest( iFragCoord + ivec2( 1,1) );
     ivec4 northwest = getClosest( iFragCoord + ivec2( -1,1) );
     ivec4 southeast = getClosest( iFragCoord + ivec2( 1,-1) );
     ivec4 southwest = getClosest( iFragCoord + ivec2( -1,-1) );
     ivec4[9] candidates = ivec4[9](old, east, north, west, south,northeast,northwest,southeast,southwest);

    //in this vector the four new closest particles' ids will be stored
    ivec4 new[2] = ivec4[2](ivec4(-1),ivec4(-1));
    //in this vector the distance to these particles will be stored
    vec4 dis[2] =vec4[2](vec4(1e20),vec4(1e20));


     for(int i=0; i<9; i++){
         for(int j=0; j<4; j++){
             int id = candidates[i][j];
             float dis2 = distance2Particle(id, rm);
             insertion_sort( new, dis, id, dis2 );
         }
     }
     int PARTICLES = int(iResolution.x*iResolution.y*Pp);
     float h = hash(
         iFragCoord.x +
         iFragCoord.y*int(iResolution.x) +
         iFrame*int(iResolution.x*iResolution.y) +
         1
     ,iTime);
     //pick random id of particle
     int p = int(h*float(PARTICLES));
     insertion_sort(new, dis, p, distance2Particle(p, rm) );
    for(int i=0; i<2; i++){
        for(int j=0; j<4; j++){
            int id = new[i][j];
            vec2 g0 = getParticle(id).xy;
             vec2    n0 = g0 - rm;
             c[i*2+j] = rm + g0*.5;  // Pt entre les 2 cellules
             n[i*2+j] = normalize(vec2(-g0.y,g0.x));
            }
        }

// Find intersection between lines
vec4 seg[8]; // x:-2 y:-1 z:+1 w:+2
for(int i=2; i<10; i++) { // Intersection point with two previous lines
    vec2 t1 = intersect(c[i%8], n[i%8], c[(i-1)%8], n[(i-1)%8]),
         t2 = intersect(c[i%8], n[i%8], c[(i-2)%8], n[(i-2)%8]);
    seg[i%8].x = t2.x;     // -2 of seg  0
    seg[i%8].y = t1.x;     // -1 of seg  0
    seg[(i-1)%8].z = t1.y; // +1 of seg -1
    seg[(i-2)%8].w = t2.y; // +2 of seg -2
}
// Reconstruct the polygon
int out_sz = 0;
for(int i=0; i<8; i++) {
    float dmin = max(seg[i].z,seg[i].w);
    if (dmin < min(seg[i].x,seg[i].y)) { // skip useless segments
        n[out_sz] = c[i]+dmin*n[i];
        out_sz++;
    }
}
float area=0.0;
vec2 center = vec2(0.0);
for(int i=0; i<out_sz-1; i++) {
        area+=(n[i].x*n[i+1].y-n[i+1].x*n[i].y);
    }
    area*=0.5;
    for(int i=0; i<out_sz-1; i++) {
            center.x+=(n[i].x+n[i+1].x)*(n[i].x*n[i+1].y-n[i+1].x*n[i].y);
            center.y+=(n[i].y+n[i+1].y)*(n[i].x*n[i+1].y-n[i+1].x*n[i].y);
        }
        center/=(6.0*area);
        return center;
}

void main(  ){



    ivec2 iFragCoord = ivec2(gl_FragCoord.xy);
    float dt=1.0;
     int PARTICLES = int(iResolution.x*iResolution.y*Pp);
     Res = iResolution.xy;
    //we only simulate PARTICLES amount of particles
    int id = iFragCoord.x + iFragCoord.y*int(iResolution.x);
    vec4 grad =texture(iChannel2,gl_FragCoord.xy/iResolution.xy);
    vec4 prev = getParticle(id);//texelFetch( iChannel0, ivec2(gl_FragCoord.xy), 0 );
    vec2 pos = prev.xy;
    vec2 vel = prev.zw;

    if(iFrame==0)
    {
        C=vec4(gl_FragCoord.xy,0.0,0.0);
        return;
    }

if(id>=PARTICLES || grad.w==0 || ((id%2)!=0)) {
C = vec4(0.0,0.0,0.0,0.0);
return;
}
    float sc=(iResolution.x/800.);

      //  float h1 = hash(id,iTime);
        //float h2 = hash(int(h1*41343.),iTime);


    // gradient, its length, and unit vector


       vec2 g =gradient(iChannel3,pos,ivec2(iResolution.xy));//1.0*getGrad(pos,1.0,iChannel3,iResolution.xy)*sc;//grad.xy;
       //g*=iResolution.xy;
       // add some noise to gradient so plain areas get some texture
       setNSeed();
       g += vec2(rand(),fract(rand()*0.41343))*0.75;//(getRand(p.pos/sc,iChannel1).xy-.5)*.003;  //getRand is pixel based so we divide arg by sc so that it looks the same on all scales
       g+=normalize(pos-iResolution.xy*.5)*.005;
       float gl=length(g);
       vec2 gu=normalize(g);

    //gather forces
    vec2 force = vec2(0);
        //vel +=.75*vec2(hash(id),hash(int(hash(id)*41343.)))/(.03+gl*gl)*sc;
        vel -= 30.0*gu*(1.+sqrt(gl*2.))*sc;
        //friction
        force -= vel*0.3;

        //interaction
        if(iMouse.z > 0.){
            vec2 delta = pos-iMouse.xy;
           // vec2 ndelta= normalize(delta);
            float dis = length(delta);
            force += delta /(dis*dis) * 300.;
        }

        //repulsion from others
        ivec4 closest = getClosest(ivec2(pos));
        for(int i = 0; i < 4; i++){
            int cid = closest[i];
            if(cid==id) continue;

            vec2 delta = pos-getParticle(cid).xy;
            float dis = length(delta);
           // if(dis<1 ){
          //      C= vec4(pos,vel); return;
            //    }
            force += delta /(dis*dis) * 700.;
        }
        vec2 center = getVoronoiPolygonCenter(id);
        vec2 deltac = (center-pos);
        float disc = length(deltac);
        force += deltac /(disc*disc) * 2000.;


        //repulsion from walls
        vec2 dist = max(iResolution.xy-pos-vec2(1), vec2(1));
        force -= 1./(dist*dist) * 100.;
             dist = max(pos, vec2(1));
        force += 1./(dist*dist) * 100.;
    float fl = length(force);

    //integrate forces
    vel += force;

    //cap velocity at max speed
    float dv = length(vel);
    if(dv>SPEED){ vel *= SPEED/dv;
    //C=vec4(0.0);
    //return;
     }
    //integrate velocity
   vec2  posnew = pos+ vel;
   float d = abs(pos.x-posnew.x)+abs(pos.y-posnew.y);
   vec2 gr = gradient(iChannel3,posnew,ivec2(iResolution.xy));
   vec3 col = texture(iChannel3,pos/iResolution.xy).xyz - texture(iChannel3,posnew/iResolution.xy).xyz ;
   float lc= length(gr);
   float lcc = length(col);
   float gray = getVal(posnew,iResolution.xy,iChannel3);


  if(d>1.0 && lc>0.00001  && lcc>0.0001)
  {
    pos=posnew;

    C = vec4(pos,vel);
  }
  else
  C = vec4(pos,vec2(0.));





}
