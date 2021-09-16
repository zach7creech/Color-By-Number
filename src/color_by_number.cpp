#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

using namespace std;

class Color
{
    public:
        int R, G, B;
        string hex, name;
        Color(int r=0, int g=0, int b=0, string hex="", string name="");
};

class Pixel
{
    public:
        int R, G, B;
        string domColor;
        Color *closestColor;
        void findDominant();
        bool multiColor();
        bool operator==(const Pixel &p) const;
};

class PixelHash
{
    public:
        size_t operator()(const Pixel &p) const;
};

class ColorVectors
{
    public:
        vector<Color *> red, blue, green, gray;
        unordered_map<Pixel, Color *, PixelHash> uniquePixs;
        unordered_map<Pixel, Color *, PixelHash>::iterator mitr;
        void readColors(string option);
        void closestColor(Pixel *pixel);
};

int findSimilarity(Pixel *pixel, Color *color);

int main(int argc, char** argv)
{  
    ColorVectors cv;
    vector< vector<Pixel *> > image;
    vector<Pixel *> row;
    string P3;
    int width, height, intensity;
    Pixel *pixel;

    cv.readColors(argv[1]);

    cin >> P3 >> width >> height >> intensity;

    for(int i = 0; i < height; i++)
    {    
        image.push_back(row);
        for(int j = 0; j < width; j++)
        {
            pixel = new Pixel;
            cin >> pixel->R >> pixel->G >> pixel->B;
            cv.closestColor(pixel);
            image[i].push_back(pixel);
        }
    }
    
    /*Color *print;

    cout << P3 << '\n' << width << ' ' << height << '\n' << intensity << '\n';

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            print = image[i][j]->closestColor;
            cout << print->R << ' ' << print->G << ' ' << print->B << ' ';
        }
        cout << '\n';
    }*/

    return 0;
}

Color::Color(int r, int g, int b, string hex, string name)
{
    R = r;
    G = g;
    B = b;
    this->hex = hex;
    this->name = name;
}

void Pixel::findDominant()
{
    if(!multiColor())
    {
        if(R > G && R > B)
            domColor = "red";
        else if(G > R && G > B)
            domColor = "green";
        else if(B > R && B > G)
            domColor = "blue";
    }
}

bool Pixel::multiColor()
{
    if(abs(R - G) < 3 && abs(R - B) < 3)
    {    
        domColor = "gray";
        return true;
    }
    else if(R == G)
    {
        domColor = "red/green";
        return true;
    }
    else if(R == B)
    {
        domColor = "red/blue";
        return true;
    }
    else if(G == B)
    {
        domColor = "green/blue";
        return true;
    }

    return false;
}

bool Pixel::operator==(const Pixel &p) const
{
    if(this->R == p.R && this->G == p.G && this->B == p.B)
        return true;
    else
        return false;
}

size_t PixelHash::operator()(const Pixel &p) const
{
    string r, g, b;

    r = to_string(p.R);
    g = to_string(p.G);
    b = to_string(p.B);

    return stoi(r + g + b);
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
    }

    fin.close();
}

void ColorVectors::closestColor(Pixel *pixel)
{
    int lowestDiff, thisDiff;

    lowestDiff = 766; //biggest possible difference (255 + 255 + 255 + 1)

    pixel->findDominant();

    mitr = uniquePixs.find(*pixel);
    
    if(mitr != uniquePixs.end())
    {
        pixel->closestColor = mitr->second;
        return;
    }

    if(pixel->domColor == "red" || pixel->domColor == "red/green" || pixel->domColor == "red/blue")
    {
        for(int i = 0; i < red.size(); i++)
        {
            thisDiff = findSimilarity(pixel, red[i]);
            if(thisDiff < lowestDiff)
            {
                lowestDiff = thisDiff;
                pixel->closestColor = red[i];
            }
        }
    }
    if(pixel->domColor == "green" || pixel->domColor == "red/green" || pixel->domColor == "green/blue")
    {
        for(int i = 0; i < green.size(); i++)
        {
            thisDiff = findSimilarity(pixel, green[i]);
            if(thisDiff < lowestDiff)
            {
                lowestDiff = thisDiff;
                pixel->closestColor = green[i];
            }
        }
    }
    if(pixel->domColor == "blue" || pixel->domColor == "red/blue" || pixel->domColor == "green/blue")
    {
        for(int i = 0; i < blue.size(); i++)
        {
            thisDiff = findSimilarity(pixel, blue[i]);
            if(thisDiff < lowestDiff)
            {
                lowestDiff = thisDiff;
                pixel->closestColor = blue[i];
            }
        }
    }
    if(pixel->domColor == "gray")
    {
        for(int i = 0; i < gray.size(); i++)
        {
            thisDiff = findSimilarity(pixel, gray[i]);
            if(thisDiff < lowestDiff)
            {
                lowestDiff = thisDiff;
                pixel->closestColor = gray[i];
            }
        }
    }

    uniquePixs[*pixel] = pixel->closestColor;
}

int findSimilarity(Pixel *pixel, Color *color)
{
    return abs(pixel->R - color->R) + abs(pixel->G - color->G) + abs(pixel->B - color->B);
}
