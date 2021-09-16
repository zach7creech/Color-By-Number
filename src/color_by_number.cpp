#include <iostream>
#include <fstream>
#include <sstream>
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
        string hex, name;
        Color(int r=0, int g=0, int b=0, string hex="", string name="");
};

class ColorVectors
{
    public:
        vector<Color *> red, blue, green, gray;
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

Color::Color(int r, int g, int b, string hex, string name)
{
    R = r;
    G = g;
    B = b;
    this->hex = hex;
    this->name = name;
}

void ColorVectors::readColors(string option)
{
    ifstream fin;
    stringstream ss;
    Color *color;
    string line;
    char category;

    if(option == "ROYGBIV")
    {
        //red
        color = new Color(255, 0, 0, "#FF0000", "Red");
        red.push_back(color);
        //orange
        color = new Color(255, 127, 0, "#FF7F00", "Orange");
        red.push_back(color);
        //yellow
        color = new Color(255, 255, 0, "#FFFF00", "Yellow");
        red.push_back(color);
        green.push_back(color);
        //green
        color = new Color(0, 128, 0, "#008000", "Green");
        green.push_back(color);
        //blue
        color = new Color(0, 0, 255, "#0000FF", "Blue");
        blue.push_back(color);
        //indigo
        color = new Color(75, 0, 130, "#4B0082", "Indigo");
        blue.push_back(color);
        //violet
        color = new Color(148, 0, 211, "#9400D3", "Violet");
        blue.push_back(color);
        //black
        color = new Color(0, 0, 0, "#000000", "Black");
        gray.push_back(color);
        //white
        color = new Color(255, 255, 255, "#FFFFFF", "White");
        gray.push_back(color);
    }
    else
    {
        fin.open("color/robust");

        while(getline(fin, line))
        {
            if(line == "//red")
            {
                category = 'R';
                continue;
            }
            else if(line == "//green")
            {
                category = 'G';
                continue;
            }
            else if(line == "//blue")
            {
                category = 'B';
                continue;
            }
            else if(line == "//gray")
            {
                category = 'g';
                continue;
            }
            
            ss.clear();
            ss.str(line);
            color = new Color;
            ss >> color->R >> color->G >> color->B >> color->hex;
            getline(ss, color->name);
            (color->name).erase(0,1);

            if(category == 'R')
                red.push_back(color);
            else if(category == 'G')
                green.push_back(color);
            else if(category == 'B')
                blue.push_back(color);
            else if(category == 'g')
                gray.push_back(color);
        }

        /*for(int i = 0; i < red.size(); i++)
        {
            cout << red[i]->R << ' ' << red[i]->G << ' ' << red[i]->B << ' ' << red[i]->hex << ' ' << red[i]->name << '\n';
        }

        for(int i = 0; i < green.size(); i++)
        {
            cout << green[i]->R << ' ' << green[i]->G << ' ' << green[i]->B << ' ' << green[i]->hex << ' ' << green[i]->name << '\n';
        }

        for(int i = 0; i < blue.size(); i++)
        {
            cout << blue[i]->R << ' ' << blue[i]->G << ' ' << blue[i]->B << ' ' << blue[i]->hex << ' ' << blue[i]->name << '\n';
        }

        for(int i = 0; i < gray.size(); i++)
        {
            cout << gray[i]->R << ' ' << gray[i]->G << ' ' << gray[i]->B << ' ' << gray[i]->hex << ' ' << gray[i]->name << '\n';
        }*/
    }

    fin.close();
}

void ColorVectors::closestColor(Pixel *)
{

}
