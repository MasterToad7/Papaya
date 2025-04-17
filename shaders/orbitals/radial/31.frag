float f(float x)
{
    if(x>0)
    {
    return ((4*pow(2.0f, 0.5f))/3.0f) * pow(z/(3.0f*a), 3.0f/2.0f) * ((z*x)/(3.0f*a)) * (1 - ((z*x)/(6.0f*a))) * pow(e, -(z*x)/(3.0f*a));
    } else
    {
        return -2147483647;
    }
}