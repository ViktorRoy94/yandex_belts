
struct Incognizable {
public:
    int x;
    int y;
};

int main() {
    Incognizable a;
    Incognizable b = {};
    Incognizable c = {0};
    Incognizable d = {0, 1};

    a.x = a.x + b.x + c.x + d.x;
    return 0;
}
