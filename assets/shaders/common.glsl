vec2 getll(vec3 fwd)
{
    // vec2 longitudeLatitude = vec2((atan(ref.z, ref.x) / 3.1415926 + 1.0) * 0.5,
    //                               (asin(ref.y) / 3.1415926 + .5));
    float y = asin(fwd.y);
    float x = atan(fwd.z/ fwd.x);
    if(fwd.z>0 && fwd.x<0)
    {
        x= -x+PI/2;
    }
    if(fwd.x<0 && fwd.z<0)
    {
        x= x+PI;
    }
    if(fwd.x>0 && fwd.z<0)
    {
        x= x+PI*2;
    }
    y = y/PI+0.5f;
    x/=PI*2;
    return vec2(x, y);
}