#include <iostream>
#include <string>

using namespace std;

int main()
{
    int r = 0, g = 0, b = 0, count = 0;
    string hex = "test", name = "foo";
    
    cout << "//red\n";
    for(int i = 0; i < 256; i+=10)
    {
        for(int j = 0; j < 256; j+=10)
        {
            for(int k = 0; k < 256; k+=10)
            {
                cout << i << ' ' << j << ' ' << k << ' ' << hex << ' ' << name << '\n';
                count+=10;
                if(count >= 5527125)
                    cout << "//green\n";
                if(count >= 11054250)
                    cout << "//blue\n";
            }
        }
    }

    return 0;
}
