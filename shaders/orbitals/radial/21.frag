float f(float x)
{
    if(x>0)
    {
    return (1.0f/pow(3.0f, 0.5f))*pow((z/(2.0f*a)), 3.0f/2.0f)*((z*x)/a)*pow(e, -((z*x)/(2.0f*a)));
    } else
    {
        return -2147483647;
    }
}