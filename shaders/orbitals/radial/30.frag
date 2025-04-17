float f(float x)
{
    if(x>0)
    {
        float p = (z*x)/a;
        return (2.0f/27.0f)*pow(z/(3.0f*a), 3.0f/2.0f)*(27.0f - (18.0f*p) + (2.0f*pow(p, 2.0f)))*pow(e, -p/3.0f);
    } else
    {
        return -2147483647;
    }
}