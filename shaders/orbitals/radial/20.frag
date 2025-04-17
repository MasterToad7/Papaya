float f(float x)
{
    if(x>0)
    {
        return 2.0f*pow(z/(2.0f*a), 3.0f/2.0f) * (1 - ((z*x)/(2.0f*a))) * pow(e, -(z*x)/(2.0f*a));
    } else
    {
        return -2147483647;
    }
}