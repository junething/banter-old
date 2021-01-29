#define new(type) calloc(1, sizeof(type))

#define JOIN(x, y) JOIN_AGAIN(x, y)
#define JOIN_AGAIN(x, y) x ## y
