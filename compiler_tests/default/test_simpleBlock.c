int f()
{
    int x = 5;
    
    {
        x = 10;
        int a = 10;
        int b = 10;
        int c = 10;
    }

    {
        int x = 20;
    }

    return x == 10;
}
