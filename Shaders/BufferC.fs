out vec4 C;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
uniform vec2 iResolution;
uniform vec4 iMouse;
uniform float iTime;
uniform int iFrame;


// Voronoi Buffer
// every pixel stores the 4 closest particles to it
// every frame this data is shared between neighbours

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





void main( ){
    Res= iResolution.xy;
        ivec2 iFragCoord = ivec2(gl_FragCoord.xy);
     int PARTICLES = int(iResolution.x*iResolution.y*Pp);
    //in this vector the four new closest particles' ids will be stored
    ivec4 new[2] = ivec4[2](ivec4(-1),ivec4(-1));
    //in this vector the distance to these particles will be stored
    vec4 dis[2] =vec4[2](vec4(1e20),vec4(1e20));

    //get all known closest particles from old self and neighbours
    ivec4 old   = getClosest( iFragCoord + ivec2( 0, 0) );
    ivec4 east  = getClosest( iFragCoord + ivec2( 1, 0) );
        ivec4 north = getClosest( iFragCoord + ivec2( 0, 1) );
    ivec4 west  = getClosest( iFragCoord + ivec2(-1, 0) );
    ivec4 south = getClosest( iFragCoord + ivec2( 0,-1) );
    ivec4 northeast = getClosest( iFragCoord + ivec2( 1,1) );
    ivec4 northwest = getClosest( iFragCoord + ivec2( -1,1) );
    ivec4 southeast = getClosest( iFragCoord + ivec2( 1,-1) );
    ivec4 southwest = getClosest( iFragCoord + ivec2( -1,-1) );

    //collect them in a array so we can loop over it
    ivec4[9] candidates = ivec4[9](old, east, north, west, south,northeast,northwest,southeast,southwest);

    for(int i=0; i<9; i++){
        for(int j=0; j<4; j++){
            int id = candidates[i][j];
            float dis2 = distance2Particle(id, gl_FragCoord.xy);
            insertion_sort( new, dis, id, dis2 );
        }
    }

    for(int k = 0; k < 1; k++){
        //random hash. We should make sure that two pixels in the same frame never make the same hash!
        float h = hash(
            iFragCoord.x +
            iFragCoord.y*int(iResolution.x) +
            iFrame*int(iResolution.x*iResolution.y) +
            k
        ,iTime);
        //pick random id of particle
        int p = int(h*float(PARTICLES));
        insertion_sort(new, dis, p, distance2Particle(p, gl_FragCoord.xy) );
    }

    C = new[0];

}
