#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <utility>
#include "disjoint_set.hpp"

using namespace std;
using namespace plank;

class Color
{
    public:
        int R, G, B;
        string hex, name;
        bool used;
        Color(int r=0, int g=0, int b=0, string hex="", string name="");
        bool operator==(const Color &c) const;
};

class ColorHash
{
    public:
        size_t operator()(const Color &c) const;
};

class Pixel
{
    public:
        int R, G, B;
        int height, i, j, blockID;
        pair<int, int> bl, tl, tr, br;
        bool left, top, right, bottom;
        string domColor;
        Color *closestColor;
        Pixel();
        bool multiColor();
        void findDominant();
        void borderCalc(char side);
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

class ColorBlock
{
    public:
        Disjoint_Set colorBlocks;
        float fontsize;
        unordered_map<Color, int, ColorHash> colorIDs;
        unordered_map<Color, int, ColorHash>::iterator mitr;
        void checkAdj(Pixel *pixel, const vector< vector<Pixel *> > &image);
        void printBlocks(vector< vector<Pixel *> > &image);
        pair<float, float> findNumberXY(int setID, vector< vector<Pixel *> > &image);
};

int findSimilarity(Pixel *pixel, Color *color);

int main(int argc, char** argv)
{  
    ColorVectors cv;
    ColorBlock cb;
    vector< vector<Pixel *> > image;
    string P3;
    int width, height, intensity;
    int defaultID = 0;
    Pixel *pixel;

    cv.readColors(argv[1]);

    cin >> P3 >> width >> height >> intensity;

    image.resize(height);
    cb.colorBlocks.Initialize(width*height);
    cb.fontsize = atof(argv[2]);

    for(int i = 0; i < height; i++)
    {    
        image[i].resize(width);
        for(int j = 0; j < width; j++)
        {
            pixel = new Pixel;
            cin >> pixel->R >> pixel->G >> pixel->B;
            cv.closestColor(pixel);
            image[i][j] = pixel;
            pixel->height = height;
            pixel->i = i;
            pixel->j = j;
            pixel->blockID = defaultID;
            defaultID++;
            cb.checkAdj(pixel, image);
        }
    }
    
    cb.printBlocks(image);

    Color *print;

    cout << P3 << '\n' << width << ' ' << height << '\n' << intensity << '\n';

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            print = image[i][j]->closestColor;
            cout << print->R << ' ' << print->G << ' ' << print->B << ' ';
        }
        cout << '\n';
    }

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

bool Color::operator==(const Color &c) const
{
    if(this->R == c.R && this->G == c.G && this->B == c.B)
        return true;
    else
        return false;
}

size_t ColorHash::operator()(const Color &c) const
{
    string r, g, b;

    r = to_string(c.R);
    g = to_string(c.G);
    b = to_string(c.B);

    return stoi(r + g + b);
}

Pixel::Pixel()
{
    bl.first = -1;
    bl.second = -1;
    tl.first = -1;
    tl.second = -1;
    tr.first = -1;
    tr.second = -1;
    br.first = -1;
    br.second = -1;

    left = false;
    top = false;
    right = false;
    bottom = false;
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

void Pixel::borderCalc(char side)
{
    if(side == 'l')
    {
        left = true;
        bl.first = j;
        bl.second = abs(i + 1 - height);
        tl.first = j;
        tl.second = abs(i - height);
    }
    else if(side == 't')
    {
        top = true;
        tl.first = j;
        tl.second = abs(i - height);
        tr.first = j + 1;
        tr.second = abs(i - height);
    }
    else if(side == 'r')
    {
        right = true;
        tr.first = j + 1;
        tr.second = abs(i - height);
        br.first = j + 1;
        br.second = abs(i + 1 - height);
    }
    else if(side == 'b')
    {
        bottom = true;
        bl.first = j;
        bl.second = abs(i + 1 - height);
        br.first = j + 1;
        br.second = abs(i + 1 - height);
    }
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
        int count = 0;

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

            if(option == "simple" && count % 5 != 0)
            {
                count++;
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
            count++;
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

void ColorBlock::checkAdj(Pixel *pixel, const vector< vector<Pixel *> > &image)
{
    int pixelID, upID, leftID;
    int i = pixel->i, j = pixel->j;
    bool updatedID = false;

    pixelID = colorBlocks.Find(pixel->blockID);

    if(i - 1 >= 0) //check up
    {
        upID = colorBlocks.Find(image[i - 1][j]->blockID);
        if(image[i - 1][j]->closestColor == pixel->closestColor && upID != pixelID)
        {    
            pixelID = colorBlocks.Union(pixelID, upID);
            updatedID = true;
        }
        else if(upID != pixelID)
        {
            pixel->borderCalc('t');
            image[i - 1][j]->borderCalc('b');
        }
    }
    else //is in top row
        pixel->borderCalc('t');

    if(j - 1 >= 0) //check left
    {
        leftID = colorBlocks.Find(image[i][j - 1]->blockID);
        if(image[i][j - 1]->closestColor == pixel->closestColor && leftID != pixelID)
        {
            if(updatedID)
                leftID = colorBlocks.Union(leftID, pixelID);
            else
                pixelID = colorBlocks.Union(pixelID, leftID);
        }
        else if(leftID != pixelID)
        {
            pixel->borderCalc('l');
            image[i][j - 1]->borderCalc('r');
        }
    }
    else //is in leftmost column
        pixel->borderCalc('l');

    if(i + 1 > image.size() - 1) //is in bottom row
        pixel->borderCalc('b');

    if(j + 1 > image[i].size() - 1) //is in right most column
        pixel->borderCalc('r');
}

void ColorBlock::printBlocks(vector< vector<Pixel *> > &image)
{
    ofstream fout;
    Pixel *pixel, *leftPix, *topPix, *rightPix, *bottomPix;
    const vector<int> *setIDs;
    pair<pair<int, int>, pair<int, int>> startingEdge;
    pair<float, float> numberXY;
    bool checkT, checkR, checkB, checkL, printLabel;
    int setID, printColorID, colorID, pixeli, pixelj, legendFont;

    setIDs = colorBlocks.Get_Set_Ids();
    colorID = 1;

    fout.open("jgraph.txt");
    fout << "newgraph xaxis nodraw yaxis nodraw\n";

    for(int i = 0; i < setIDs->size(); i++)
    {
        setID = (*setIDs)[i];

        pixeli = setID / image[0].size();
        pixelj = setID % image[0].size();

        pixel = image[pixeli][pixelj];

        fout << "newline marktype none ";

        mitr = colorIDs.find(*(pixel->closestColor));

        if(mitr == colorIDs.end())
        {
            fout << "label : " << colorID << ' ' << pixel->closestColor->name <<  '\n';;
            colorIDs[*(pixel->closestColor)] = colorID;
            printColorID = colorID;
            colorID++;
        }
        else
            printColorID = mitr->second;
    
        checkT = true;
        checkR = false;
        checkB = false;
        checkL = false;
            
        startingEdge.first = pixel->bl;
        startingEdge.second = pixel->tl;
            
        fout << "pts " << pixel->bl.first << ' ' << pixel->bl.second << ' ' << pixel->tl.first << ' ' << pixel->tl.second << ' ';
                
        while(1)
        {
            if(checkT && pixel->top)
            {
                fout << pixel->tr.first << ' ' << pixel->tr.second << ' ';
                checkT = false; //where you were
                checkR = true;  //where you're going
            }
            else if(checkT)
            {
                checkT = false;
                checkL = true;
                pixel = image[pixel->i - 1][pixel->j];
                continue;
            }
            if(checkR && pixel->right)
            {
                fout << pixel->br.first << ' ' << pixel->br.second << ' ';
                checkR = false;
                checkB = true;
            }
            else if(checkR)
            {
                checkR = false;
                checkT = true;
                pixel = image[pixel->i][pixel->j + 1];
                continue;
            }
            if(checkB && pixel->bottom)
            {
                fout << pixel->bl.first << ' ' << pixel->bl.second << ' ';
                checkB = false;
                checkL = true;
            }
            else if(checkB)
            {
                checkB = false;
                checkR = true;
                pixel = image[pixel->i + 1][pixel->j];
                continue;
            }
            if(checkL && pixel->left)
            {
                if(pixel->bl == startingEdge.first && pixel->tl == startingEdge.second) break;
                fout << pixel->tl.first << ' ' << pixel->tl.second << ' ';
                checkL = false;
                checkT = true;
            }
            else if(checkL)
            {
                checkL = false;
                checkB = true;
                pixel = image[pixel->i][pixel->j - 1];
                continue;
            }
        }
        fout << "color 0 0 0\n";

        numberXY = findNumberXY(setID, image);
        fout << "newstring x " << numberXY.first << " y " << numberXY.second << " fontsize " << fontsize << " : " << printColorID << '\n';
    }
    
    if(colorID <= 50)
        legendFont = 8;
    else if(colorID > 50 && colorID <= 100)
        legendFont = 6;
    else
        legendFont = 5;

    fout << "legend defaults\nfontsize " << legendFont << " linelength 0.5\n";
    fout.close();
}

pair<float, float> ColorBlock::findNumberXY(int setID, vector< vector<Pixel *> > &image)
{
    Pixel *pixel;
    int pixeli, pixelj, minX, minY, maxX, maxY;
    float x, y;

    pixeli = setID / image[0].size();
    pixelj = setID % image[0].size();

    pixel = image[pixeli][pixelj];
    minX = pixel->tl.first;
    maxX = pixel->tr.first;
    minY = pixel->bl.second;
    maxY = pixel->tl.second;

    x = (minX + maxX) / 2.0;

    y = (minY + maxY) / 2.0 - 0.15;
    
    return pair<float, float>(x, y);
}

int findSimilarity(Pixel *pixel, Color *color)
{
    return abs(pixel->R - color->R) + abs(pixel->G - color->G) + abs(pixel->B - color->B);
}
