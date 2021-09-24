#pragma once
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Color
{
    public:
        int R, G, B;
        std::string hex, name;
        bool used;
        void toHex();
        Color(int r=0, int g=0, int b=0, std::string hex="", std::string name="");
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
        std::pair<int, int> bl, tl, tr, br;
        bool left, top, right, bottom;
        std::string domColor;
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
        std::vector<Color *> red, blue, green, gray;
        std::unordered_map<Pixel, Color *, PixelHash> uniquePixs;
        void readColors(std::string colorOption);
        void closestColor(Pixel *pixel, std::string colorOption);
        int findSimilarity(Pixel *pixel, Color *color);
        ~ColorVectors();
};

class ColorBlock
{
    public:
        plank::Disjoint_Set colorBlocks;
        float fontsize;
        std::string title;
        std::unordered_map<Color, int, ColorHash> colorIDs;
        void checkAdj(Pixel *pixel, const std::vector< std::vector<Pixel *> > &image);
        void printBlocks(std::vector< std::vector<Pixel *> > &image, std::unordered_set<std::string> &options);
        std::pair<float, float> findNumberXY(int setID, std::vector< std::vector<Pixel *> > &image);
};

class ColorByNumber
{
    public:
        ColorVectors cv;
        ColorBlock cb;
        std::vector< std::vector<Pixel *> > image;
        std::unordered_set<std::string> options;
        std::string P3;
        int width, height, intensity;
        ColorByNumber(int argc, char **argv);
        ~ColorByNumber();
};
