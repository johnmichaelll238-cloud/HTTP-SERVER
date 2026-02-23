#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::endl;


int main(int argc, char* argv[]) {
    bool debug = false;
    bool verbose = false;

    //parse command line arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--mode=debug") {
            debug = true;
        }
        if (arg == "--verbose") {
            verbose = true;
        }
        }
      if (debug){
        cout << "[DEBUG] Running in debug mode" << endl;   
       }
      if (verbose){
        cout << "[VERBOSE] Verbose logging enabled" << endl;
      }
        return 0;
    }



