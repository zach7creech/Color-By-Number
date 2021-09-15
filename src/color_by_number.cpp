#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Pixel
{
    public:
        int R, G, B;
        string dom_color;
        void find_dominant_color();
};

class Color
{
    public:
        int R, G, B;
        string name;
};

class ColorVectors
{
    public:
        vector<Color *> red, blue, green;
        void readColors(string option);
        void closestColor(Pixel *pixel);
};

int main(int argc, char** argv)
{  
    ColorVectors cv;
    vector< vector<Pixel *> > image;
    vector<Pixel *> row;
    string P3;
    int width, height, intensity;
    Pixel *pixel = new Pixel;

    cv.readColors(argv[1]);

    cin >> P3 >> width >> height >> intensity;

    for(int i = 0; i < height; i++)
    {    
        image.push_back(row);
        for(int j = 0; j < width; j++)
        {
            cin >> pixel->R >> pixel->G >> pixel->B;
            pixel->find_dominant_color();
            image[i].push_back(pixel);
            pixel = new Pixel;
        }
    }

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

void ColorVectors::readColors(string option)
{

}

void ColorVectors::closestColor(Pixel *)
{

}
