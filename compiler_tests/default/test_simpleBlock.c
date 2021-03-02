int f()
{
    int x = 5;
    
    {
        x = 10;
    }

    {
        int x = 20;
    }

    return x == 10;
}
