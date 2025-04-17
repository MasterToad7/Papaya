float f(float x)
{
    if(x>0)
    {
        return ((2.0f*pow(2.0f, 0.5f))/(3.0f*pow(5.0f, 0.5f))) * pow(z/(3.0f*a), 3.0f/2.0f) * pow((z*x)/(3.0f*a), 2.0f) * pow(e, -(z*x)/(3.0f*a));
    } else
    {
        return -2147483647;
    }
}