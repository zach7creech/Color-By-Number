#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

class Pixel
{
    public:
        int R, G, B;
        string dom_color;
        void find_dominant_color();
};

int main(int argc, char** argv)
{  
    return 0;
}

void Pixel::find_dominant_color()
{
    if(R > G && R > B)
        dom_color = "red";
    else if(G > R && G > B)
        dom_color = "green";
    else if(B > R && B > G)
        dom_color = "blue";
    else
        dom_color = "none"; //unlikely case that there is no dominant color
}
