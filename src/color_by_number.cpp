/* Author: Zachery Creech
 * COSC 494
 * Lab 1: Jgraph
 * 09/25/2021
 *
 * Credit to Dr. James Plank (plank@cs.utk.edu) for his implementation of a Disjoint_Set
 * data structure (https://bitbucket.org/jimplank/plank-disjoint-sets/src/master/) and
 * for the jgraph utility. */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <cmath>
#include <algorithm>
#include "disjoint_set.hpp"
#include "color_by_number.hpp"

using namespace std;
using namespace plank;

int main(int argc, char **argv)
{
    //if using the shell script this probably won't be an issue, but check just in case
    if(argc < 3)
    {
        cerr << "usage: ./bin/color-by-number <color> <fontsize> <options>\n";
        return -1;
    }

    ColorByNumber cbn(argc, argv);

    return 0;
}

/* Constructor for ColorByNumber driver class that is responsible for executing main functions of program:
 * Determining what optional arguments the user selected, reading the ppm image information from stdin,
 * instantiating ColorVectors class for reading in Color objects from color/robust file and approximating
 * each pixel's color, instantiating ColorBlock class that manages the disjoint set(s) of colors as well as
 * prints to jgraph.txt to create the graph, and optionally prints the "closest color approximated" version
 * of the image to output.ppm */
ColorByNumber::ColorByNumber(int argc, char **argv)
{  
    ofstream fout;
    int defaultID;
    string colorOption, checkTitle, title;
    Pixel *pixel;
    Color *print;

    //get all optional arguments
    for(int i = 3; i < argc; i++)
    {
        checkTitle = argv[i];
        if(checkTitle == "title" && i + 1 < argc) //title command must be followed immediately by the title string
        {
            title = argv[i + 1];
            replace(title.begin(), title.end(), '_', ' '); //remove underscores
        }
        options.insert(argv[i]);
    }

    //read in colors from color/robust based on chosen color option
    colorOption = argv[1];
    options.insert(colorOption);
    cv.readColors(colorOption);

    //read in ppm header info
    cin >> P3 >> width >> height >> intensity;

    //set up the 2D image vector and initialize the disjoint set
    image.resize(height);
    cb.colorBlocks.Initialize(width*height);
    cb.fontsize = atof(argv[2]);
    cb.title = title;

    //create the 2D image vector pixel by pixel
    defaultID = 0;
    for(int i = 0; i < height; i++)
    {    
        image[i].resize(width);
        for(int j = 0; j < width; j++)
        {
            //create the pixel and find its closest color
            pixel = new Pixel;
            cin >> pixel->R >> pixel->G >> pixel->B;
            cv.closestColor(pixel, colorOption);
            image[i][j] = pixel;
            
            //set up the pixel's info and then check its top and left neighbors to see if it's in the same "block"
            pixel->height = height;
            pixel->i = i;
            pixel->j = j;
            pixel->blockID = defaultID;
            defaultID++;
            cb.checkAdj(pixel, image);
        }
    }
    
    //print the jgraph.txt file to be used by jgraph
    cb.printBlocks(image, options);

    //optionally print out the approximated version of the original image
    if(options.find("print_ppm") != options.end())
    {
        fout.open("output.ppm");

        fout << P3 << '\n' << width << ' ' << height << '\n' << intensity << '\n';

        for(int i = 0; i < height; i++)
        {
            for(int j = 0; j < width; j++)
            {
                print = image[i][j]->closestColor;
                fout << print->R << ' ' << print->G << ' ' << print->B << ' ';
            }
            fout << '\n';
        }
        fout.close();
    }
}

//Destructor for ColorByNumber class, when main() returns delete all the pixel pointers in the 2D image vector
ColorByNumber::~ColorByNumber()
{
    for(int i = 0; i < image.size(); i++)
    {
        for(int j = 0; j < image[i].size(); j++)
            delete image[i][j];
    }
}

//Constructor for Color class, really only used when ROYGBIV option is specified
Color::Color(int r, int g, int b, string hex, string name)
{
    R = r;
    G = g;
    B = b;
    this->hex = hex;
    this->name = name;
}

//Convert RGB value to equivalent hex, only used when "extreme" color option is specified
void Color::toHex()
{
    char hexName[8];

    snprintf(hexName, sizeof hexName, "#%02x%02x%02x", R, G, B);
    
    for(int i = 0; i < strlen(hexName); i++)
        hexName[i] = toupper(hexName[i]);
    
    name = hexName; 
}

//Operator overload to compare two colors, used when map is keyed on color
bool Color::operator==(const Color &c) const
{
    if(this->R == c.R && this->G == c.G && this->B == c.B)
        return true;
    else
        return false;
}

//Hash function for keying map on color, just push RGB values into one integer
size_t ColorHash::operator()(const Color &c) const
{
    string r, g, b;

    r = to_string(c.R);
    g = to_string(c.G);
    b = to_string(c.B);

    return stoi(r + g + b);
}

//Constructor for Pixel class, set up default values for corner coordinates and border booleans
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

//Check if a pixel has more than one dominant color, or is gray
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

//Find the dominant color for the pixel if it isn't multicolored
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

//Determine the coordinates of a pixel's corners when translated to normal XY coordinates
//The only points to be plotted are ones that make up borders of color blocks
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

//Operator overload to compare two pixels, used when map is keyed on pixel
bool Pixel::operator==(const Pixel &p) const
{
    if(this->R == p.R && this->G == p.G && this->B == p.B)
        return true;
    else
        return false;
}

//Hash function for keying map on pixel, just push RGB values into one integer
size_t PixelHash::operator()(const Pixel &p) const
{
    string r, g, b;

    r = to_string(p.R);
    g = to_string(p.G);
    b = to_string(p.B);

    return stoi(r + g + b);
}

//Read in r, g, b, and gray colors into color vectors based on specified color option
void ColorVectors::readColors(string colorOption)
{
    ifstream fin;
    stringstream ss;
    Color *color;
    string line;
    int count, rd, gbd;
    char category;

    //this looks terrible for most images but can produce funny output, also includes black and white
    if(colorOption == "ROYGBIV")
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
    else if(colorOption != "extreme") //if colorOption == extreme then the program won't use any colors from color/robust
    {
        count = 0;

        //these decide how many colors to skip when reading in the colors from color/robust
        //there are significantly more reds in the file than the other colors, so skip more of them
        if(colorOption == "robust")
        {
            rd = 1;
            gbd = 1;
        }
        else if(colorOption == "simpler")
        {
            rd = 3;
            gbd = 2;
        }
        else if(colorOption == "dull")
        {
            rd = 4;
            gbd = 3;
        }
        else if(colorOption == "flat")
        {
            rd = 5;
            gbd = 4;
        }

        fin.open("color/robust");

        //each line in color/robust will either be color information in a specific format or a comment telling which colors are in this part of the file
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

            //for skipping lines if "robust" option isn't chosen
            if(category == 'R' && count % rd != 0)
            {
                count++;
                continue;
            }
            else if((category == 'G' || category == 'B') && count % gbd != 0)
            {
                count++;
                continue;
            }
        
            //each color line consists of "R G B #hexcode name"
            ss.clear();
            ss.str(line);
            color = new Color;
            ss >> color->R >> color->G >> color->B >> color->hex;
            getline(ss, color->name);
            (color->name).erase(0,1); //for whatever reason when getting the name it picks up the leading whitespace. It's clunky but whatever

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
        fin.close();
    }
}

//Approximate the pixel's color to one from color/robust
void ColorVectors::closestColor(Pixel *pixel, string colorOption)
{
    Color *color;
    unordered_map<Pixel, Color *, PixelHash>::iterator mitr;
    int lowestDiff, thisDiff;

    lowestDiff = 766; //biggest possible difference (255 + 255 + 255 + 1)

    mitr = uniquePixs.find(*pixel);

    //if "extreme" is chosen, then every pixel's actual color values will be used. That means they won't have names other than their hex codes
    if(colorOption == "extreme" && mitr == uniquePixs.end())
    {
        color = new Color;
        color->R = pixel->R;
        color->G = pixel->G;
        color->B = pixel->B;
        color->toHex();
        pixel->closestColor = color;
        uniquePixs[*pixel] = color;
        return;
    }
    else if(colorOption == "extreme")
    {
        pixel->closestColor = mitr->second;
        return;
    }

    if(mitr != uniquePixs.end())
    {
        pixel->closestColor = mitr->second;
        return;
    }

    pixel->findDominant();

    //if it's a red dominant pixel, find the red color that has an RGB value closest to it
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
    //same for green dominant
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
    //same for blue dominant
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
    //same for gray
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

    //maintain map of unqiue pixels so the program doesn't have to re-find the closest color for identical pixels
    uniquePixs[*pixel] = pixel->closestColor;
    //**extra note: this actually slows the program down a small amount when only a small number of colors are used
}

//Similarity of two colors can be determined as the sum of the differences between R, G, and B
int ColorVectors::findSimilarity(Pixel *pixel, Color *color)
{
    return abs(pixel->R - color->R) + abs(pixel->G - color->G) + abs(pixel->B - color->B);
}

//Destructor for ColorVectors class, clears all the color pointers from each of its color vectors
ColorVectors::~ColorVectors()
{
    for(int i = 0; i < red.size(); i++)
        delete red[i];
    for(int i = 0; i < green.size(); i++)
        delete green[i];
    for(int i = 0; i < blue.size(); i++)
        delete blue[i];
    for(int i = 0; i < gray.size(); i++)
        delete gray[i];
}

/* This is called while the image is being read in by ColorByNumber to determine if a pixel shares a border with a
 * pixel of the same color. Since the image is read left to right from top to bottom, this only needs to check above
 * and to the left of a pixel. This modifies both this pixel and the one it's being checked against, which in the end
 * ensures that every pixel has each of its borders checked (top, right, bottom, left). Pixels of the same color that
 * border each other are unioned together into the same set so they can be appropriately traversed and printed in
 * printBlocks(). Whenever a nonequal color border is found, including the outer edges of the image, the coordinates
 * for that border are calculated for that pixel (and the adjacent pixel of differing color, if there is one). */
void ColorBlock::checkAdj(Pixel *pixel, const vector< vector<Pixel *> > &image)
{
    int pixelID, upID, leftID;
    int i = pixel->i, j = pixel->j;
    bool updatedID = false;

    pixelID = colorBlocks.Find(pixel->blockID);

    //if two adjacent pixels have the same closestColor and are not already in the same set, union them together
    //otherwise calculate the coordinates of the border for the direction (top, bottom, left, right) of this pixel and the opposite direction for the adjacent
    //i.e. if this pixel has a border on top, that means the pixel above it has a border on bottom

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
            //if the pixel just got unioned into a set above it, then the pixel to the left should also be in that set
            //otherwise this pixel needs to become a member of the set to the left
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

/* This is the longest and most complicated function in the program. It handles printing the general jgraph information,
 * including the "newgraph" header and any stylistic modifications, like those to the title, legend, axes, etc. The
 * complicated part is printing out the points for each color block in order going clockwise around the border of each
 * block, as well as printing the matching number for each color in the image. The details of that process can be found
 * within the function. */
void ColorBlock::printBlocks(vector< vector<Pixel *> > &image, unordered_set<string> &options)
{
    ofstream fout;
    unordered_map<Color, int, ColorHash>::iterator mitr; 
    Pixel *pixel, *leftPix, *topPix, *rightPix, *bottomPix;
    const vector<int> *setIDs;
    pair<pair<int, int>, pair<int, int>> startingEdge;
    pair<float, float> numberXY;
    bool checkT, checkR, checkB, checkL, printLabel;
    int setID, printColorID, colorID, pixeli, pixelj, legendFont;
    float scale;

    setIDs = colorBlocks.Get_Set_Ids();
    colorID = 1;

    fout.open("jgraph.txt");
    fout << "newgraph";

    //deal with scaling the image, the default size for each axis is 6 inches
    //the larger dimension (width or height) needs to be scaled so the output image doesn't look squashed
    if(image[0].size() > image.size())
    {
        scale = image[0].size() * 1.0 / image.size();
        fout << " xaxis size " << 6 * scale << " yaxis size 6\n";
    }
    else
    {
        scale = image.size() * 1.0 / image[0].size();
        fout << " xaxis size 6 yaxis size " << 6 * scale << '\n';
    }

    //by default don't show the axes
    if(options.find("show_axis") == options.end())
        fout << "xaxis nodraw yaxis nodraw\n";

    //set the title if specified
    if(options.find("title") != options.end())
        fout << "title : " << title << '\n';

    //this is where printing each color block begins
    //each color block belongs to a set ID, so iterate through all of them to print all color blocks
    for(int i = 0; i < setIDs->size(); i++)
    {
        setID = (*setIDs)[i];

        //get the first pixel in the set's i,j coordinates in the 2D image vector with this math
        pixeli = setID / image[0].size();
        pixelj = setID % image[0].size();

        pixel = image[pixeli][pixelj];

        //each color block is a line
        fout << "newline marktype none ";

        mitr = colorIDs.find(*(pixel->closestColor));

        //if this color hasn't been printed before, it needs a label so the color shows up in the legend
        if(mitr == colorIDs.end())
        {
            fout << "label : " << colorID << ' ' << pixel->closestColor->name;
            if(options.find("print_hex") != options.end()) //option for printing the color's hex code
                fout << ' ' << pixel->closestColor->hex << '\n';
            else
                fout << '\n';
            colorIDs[*(pixel->closestColor)] = colorID;
            printColorID = colorID;
            colorID++; //increment colorID so that the next color will have a unique number associated with it in the legend
        }
        else
            printColorID = mitr->second;
    
        /* This is where traversing the border of the color block begins. The algorithm works as follows:
         * The first pixel in the set is always the first time this particular set of color was encountered when
         * initially reading the image from left to right, top to bottom. That means the first pixel in the set
         * will always have a left border. Start printing there and save the coordinates of that edge so you know
         * when you've made it all the way around the block. From there, always print in a clockwise direction, i.e.,
         * after printing a left border, try to print the top border. If there isn't a top border, that means that there
         * is a pixel in the same set directly above this pixel. Set the current pixel to be that pixel, and start over
         * printing from the last edge that was printed. In this case, that means another left edge needs to be printed.
         * Continue following this pattern all the way around the block and only the outermost border coordinates will be
         * printed in order. This is necessary because jgraph plots the line in order of the points given to it. */

        //checking top next
        checkT = true;
        checkR = false;
        checkB = false;
        checkL = false;
            
        //save the starting edge so you know when to stop
        startingEdge.first = pixel->bl;
        startingEdge.second = pixel->tl;
            
        fout << "pts " << pixel->bl.first << ' ' << pixel->bl.second << ' ' << pixel->tl.first << ' ' << pixel->tl.second << ' ';
                
        //only ends when the starting edge is reached
        while(1)
        {
            if(checkT && pixel->top) //print top then continue clockwise
            {
                fout << pixel->tr.first << ' ' << pixel->tr.second << ' ';
                checkT = false; //where you were
                checkR = true;  //where you're going in clockwise order
            }
            else if(checkT) //anytime an else if fires, that means there's a neighboring pixel in the same set so you need to move to that pixel, this case move up
            {
                checkT = false; //where you were
                checkL = true;  //going back to print the same side but for the next pixel
                pixel = image[pixel->i - 1][pixel->j];
                continue;
            }
            if(checkR && pixel->right) //print right then continue clockwise
            {
                fout << pixel->br.first << ' ' << pixel->br.second << ' ';
                checkR = false;
                checkB = true;
            }
            else if(checkR) //move right and try printing top again
            {
                checkR = false;
                checkT = true;
                pixel = image[pixel->i][pixel->j + 1];
                continue;
            }
            if(checkB && pixel->bottom) //print bottom then continue clockwise
            {
                fout << pixel->bl.first << ' ' << pixel->bl.second << ' ';
                checkB = false;
                checkL = true;
            }
            else if(checkB) //move down and try printing right again
            {
                checkB = false;
                checkR = true;
                pixel = image[pixel->i + 1][pixel->j];
                continue;
            }
            if(checkL && pixel->left) //print left then continue clockwise
            {
                if(pixel->bl == startingEdge.first && pixel->tl == startingEdge.second) break; //found the starting edge, stop
                fout << pixel->tl.first << ' ' << pixel->tl.second << ' ';
                checkL = false;
                checkT = true;
            }
            else if(checkL) //move left and try printing bottom again
            {
                checkL = false;
                checkB = true;
                pixel = image[pixel->i][pixel->j - 1];
                continue;
            }
        }
        fout << "color 0 0 0\n"; //line color is always black

        //figure out where to print the set's colorID and then tell jgraph where to print it
        numberXY = findNumberXY(setID, image);
        fout << "newstring x " << numberXY.first << " y " << numberXY.second << " fontsize " << fontsize << " : " << printColorID << '\n';
    }
    
    //legend fontsize has to be smaller if more colors are used
    //doesn't work great with a ton of colors because the font gets too small to see to stay on the page
    if(colorID <= 50)
        legendFont = 10;
    else if(colorID > 50 && colorID <= 100)
        legendFont = 6;
    else if(colorID > 100 && colorID <= 180)
        legendFont = 4;
    else if(colorID <= 250)
        legendFont = 2;
    else
        legendFont = 1;

    fout << "legend defaults\nfontsize " << legendFont << " linelength 0.5\n";
    fout.close();
}

//Determine the x,y coordinate for the set's colorID based on where the set's first pixel is located in the original image
pair<float, float> ColorBlock::findNumberXY(int setID, vector< vector<Pixel *> > &image)
{
    Pixel *pixel;
    int pixeli, pixelj, minX, minY, maxX, maxY;
    float x, y;

    pixeli = setID / image[0].size();
    pixelj = setID % image[0].size();

    //put the number in the middle of the pixel
    pixel = image[pixeli][pixelj];
    minX = pixel->tl.first;
    maxX = pixel->tr.first;
    minY = pixel->bl.second;
    maxY = pixel->tl.second;

    x = (minX + maxX) / 2.0;

    //the y value should be shifted down a little bit to offset the height of the number
    y = (minY + maxY) / 2.0 - 0.15;
    
    return pair<float, float>(x, y);
}
