#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<string>
#include<string.h> 
#include<vector>
#include<fstream>
#include<sstream>
#include <sys/stat.h>
#include <sys/types.h>


using namespace std ;

void DeezFiles(int i);
int main(int argc, char **argv, char *envp[]) {
    const char *pathname;
    pathname = "DRIVE/";

    //if ()
    {
        mkdir(pathname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        for (int i = 0; i < 32; i++)
        {
            DeezFiles(i);
        }
        string os;
        os = "DRIVE/CHECKPOINT";
        cout << "File Created: " << os << endl;
        ofstream out;
        out.open(os.c_str());
        for (int j = 0; j < 192; j++)
        {
            out << '0';
        }
        out.close();
    }
    /*else
    {
        cout << "PATH CREATED! RUN AGAIN" << endl;
    }*/
        
    return 0;
}

void DeezFiles(int i)
{
    string os;
    os = "DRIVE/SEGMENT";
    os.append(to_string( i ));
    //os += ".txt";
    cout << "File Created: " << os << endl;
    ofstream out;
    out.open(os.c_str());
    for (int j = 0; j < 1024*1024; j++)
    {
        out << '0';
    }
    out.close();

}
