#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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

class Pixel
{
    public:
        int R, G, B;
        int height, i, j, blockID;
        pair<int, int> bl, tl, tr, br;
        bool left = false, top = false, right = false, bottom = false;
        bool isBorder = false;
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
        int totalColors = 0;
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
        unordered_set<int> printedBlocks;
        unordered_map<int, int> containsSet;
        void checkAdj(Pixel *pixel, const vector< vector<Pixel *> > &image);
        void printBlocks(vector< vector<Pixel *> > &image);
        pair<float, float> findNumberXY(int setID, float x, float y, vector< vector<Pixel *> > &image);
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

    //cout << cb.colorBlocks.Get_Elements()->size() << '\n';
    
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
            //cout << cb.colorBlocks.Find(defaultID) << '\n';
            pixel->blockID = defaultID;
            defaultID++;
            cb.checkAdj(pixel, image);
        }
    }
    
    cb.printBlocks(image);

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

    //cout << cv.totalColors << '\n';

    //cout << cb.colorBlocks.Get_Set_Ids()->size() << '\n';

    /*for(int i = 0; i < image.size(); i++)
    {
        for(int j = 0; j < image[i].size(); j++)
        {
            pixel = image[i][j];
            cout << pixel->left << ' ' << pixel->top << ' ' << pixel->right << ' ' << pixel->bottom << '\n';
            //cout << '(' << pixel->bl.first << ' ' << pixel->bl.second << ')' << ' ' << '(' << pixel->tl.first << ' ' << pixel->tl.second << ')' << ' ' << 
              //      '(' << pixel->tr.first << ' ' << pixel->tr.second << ')' << ' ' << '(' << pixel->br.first << ' ' << pixel->br.second << ')' << ' ' << '\n';
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

bool Color::operator==(const Color &c) const
{
    if(this->R == c.R && this->G == c.G && this->B == c.B)
        return true;
    else
        return false;
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
        //cout << i << ' ' << j << " has left border\n";
        left = true;
        bl.first = j;
        bl.second = abs(i + 1 - height);
        tl.first = j;
        tl.second = abs(i - height);
    }
    else if(side == 't')
    {
        //cout << i << ' ' << j << " has top border\n";
        top = true;
        tl.first = j;
        tl.second = abs(i - height);
        tr.first = j + 1;
        tr.second = abs(i - height);
    }
    else if(side == 'r')
    {
        //cout << i << ' ' << j << " has right border\n";
        right = true;
        tr.first = j + 1;
        tr.second = abs(i - height);
        br.first = j + 1;
        br.second = abs(i + 1 - height);
    }
    else if(side == 'b')
    {
        //cout << i << ' ' << j << " has bottom border\n";
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

    if(!pixel->closestColor->used) { pixel->closestColor->used = true; totalColors++; }
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
            //cout << "pixelID was " << pixelID << '\n';
            pixelID = colorBlocks.Union(pixelID, upID);
            //cout << "pixelID is now " << pixelID << '\n';
            updatedID = true;
        }
        else if(upID != pixelID)
        {
            pixel->isBorder = true;
            pixel->borderCalc('t');
            image[i - 1][j]->isBorder = true;
            image[i - 1][j]->borderCalc('b');
        }
    }
    else //is in top row
    {
        //cout << "top\n";
        pixel->isBorder = true;
        pixel->borderCalc('t');
    }

    if(j - 1 >= 0) //check left
    {
        leftID = colorBlocks.Find(image[i][j - 1]->blockID);
        if(image[i][j - 1]->closestColor == pixel->closestColor && leftID != pixelID)
        {
            if(updatedID)
            {
                //cout << "leftID was " << leftID << '\n';
                leftID = colorBlocks.Union(leftID, pixelID);
                //cout << "leftID is now " << leftID << '\n';
            }
            else
            {
                //cout << "pixelID is " << pixelID << '\n';
                pixelID = colorBlocks.Union(pixelID, leftID);
                //cout << "pixelID is now " << pixelID << '\n';
            }
        }
        else if(leftID != pixelID)
        {
            pixel->isBorder = true;
            pixel->borderCalc('l');
            image[i][j - 1]->isBorder = true;
            image[i][j - 1]->borderCalc('r');
        }
    }
    else //is in leftmost column
    {
        //cout << "leftmost\n";
        pixel->isBorder = true;
        pixel->borderCalc('l');
    }

    //cout << i + 1 << ' ' << image.size() << '\n';
    if(i + 1 > image.size() - 1) //is in bottom row
    {
        //cout << "bottom\n";
        pixel->isBorder = true;
        pixel->borderCalc('b');
    }
    //cout << j + 1 << ' ' << image[i].size() << '\n';
    if(j + 1 > image[i].size() - 1) //is in right most column
    {
        //cout << "rightmost\n";
        pixel->isBorder = true;
        pixel->borderCalc('r');
    }
}

void ColorBlock::printBlocks(vector< vector<Pixel *> > &image)
{
    ofstream fout;
    Pixel *pixel;
    pair<pair<int, int>, pair<int, int>> startingEdge;
    pair<float, float> numberXY;
    bool checkT, checkR, checkB, checkL;
    int minX, minY, maxX, maxY;
    int setID;
    
    fout.open("jgraph.txt");
    fout << "newgraph\n";

    for(int i = 0; i < image.size(); i++)
    {
        for(int j = 0; j < image[i].size(); j++)
        {
            pixel = image[i][j];
            setID = colorBlocks.Find(pixel->blockID);
            if(pixel->isBorder && printedBlocks.find(setID) == printedBlocks.end())
            {
                if(pixel->bl.first >= 0)
                {    
                    minX = pixel->bl.first;
                    minY = pixel->bl.second;
                }   
                if(pixel->tr.first >= 0)
                {
                    maxX = pixel->tr.first;
                    maxY = pixel->tr.second;
                }
                
                checkT = true;
                checkR = false;
                checkB = false;
                checkL = false;
                
                printedBlocks.insert(colorBlocks.Find(pixel->blockID));
                startingEdge.first = pixel->bl;
                startingEdge.second = pixel->tl;
                
                fout << "newline poly pcfill 1 1 1 pts " << pixel->bl.first << ' ' << pixel->bl.second << ' ' << pixel->tl.first << ' ' << pixel->tl.second << ' ';
                
                while(1)
                {
                    if(pixel->bl.first >= 0)
                    {
                        if(pixel->bl.first < minX && pixel)
                            minX = pixel->bl.first;
                        if(pixel->bl.second < minY)
                            minY = pixel->bl.second;
                    }
                    if(pixel->tr.first >= 0)   
                    {
                        if(pixel->tr.first > maxX)
                            maxX = pixel->tr.first;
                        if(pixel->tr.second > maxY)
                            maxY = pixel->tr.second;
                    }

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
                numberXY = findNumberXY(setID, (minX + maxX) / 2.0, (minY + maxY) / 2.0, image);
                //cout << minX << ' ' << maxX << ' ' << minY << ' ' << maxY << ' ' << numberXY.first << ' ' << numberXY.second << '\n';
                fout << "newstring x " << numberXY.first << " y " << numberXY.second << " fontsize 9 : " << '1' << '\n';
            }
        }
    }

    fout.close();
}

pair<float, float> ColorBlock::findNumberXY(int setID, float x, float y, vector< vector<Pixel *> > &image)
{
    Pixel *pixel;
    int i, j;

    j = floor(x);
    i = floor(abs(y - image.size()));
    pixel = image[i][j];
    
    cout << "x is " << x << " y is " << y << '\n';
    cout << "i is " << i << " j is " << j << '\n';
    cout << colorBlocks.Find(pixel->blockID) << ' ' << setID << '\n';

    if(floor(x) == x)
        x += 0.5;
    if(floor(y) == y)
        y -= 0.5;

    if(colorBlocks.Find(pixel->blockID) == setID)
        return pair<float, float>(x, y);
    else
        return pair<float, float>(-1, -1);
}
int findSimilarity(Pixel *pixel, Color *color)
{
    return abs(pixel->R - color->R) + abs(pixel->G - color->G) + abs(pixel->B - color->B);
}
