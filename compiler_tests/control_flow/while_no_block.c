int f()
{
    int x = 100;

    while(x)
        x = x-1;
    x = x+1;
    
    return x == 1;
}
