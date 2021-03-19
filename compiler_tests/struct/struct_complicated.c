struct innerA{
    int a;
    int b;
};

struct innerB{
    struct innerA inner;
};

struct outer{
    int a;
    float b;
    double c;
    char d;

    struct innerB e;

    // int arr[5];

    // struct {
    //     int a;
    //     int b;
    // } f;
};

int f()
{
    struct outer st;

    st.a = 5;
    st.b = 43.0f;
    st.c = 3424.43;
    st.d = '\r';

    st.e.inner.a = 22;
    st.e.inner.b = 222;

    // st.arr[1] = 11;

    return  (st.a == 5) &&
            (st.b == 43.0f) &&
            (st.c == 3424.43) &&
            (st.d == '\r') &&
            (st.e.inner.a == 22) &&
            (st.e.inner.b == 222) ;
            // (st.arr[1] == 11);
}