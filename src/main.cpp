#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include "./parser.h"

#include "./tokeniser.h"

#include "./gencode.h"
#include "./arenaloc.h"



int main(int argc, char* argv[]){
    if (argc != 2) {
        std::cerr << "The file is needed" << std::endl;
        exit(1);
        }


    std::string result;
    std::fstream input(argv[1], std::ios::in);
    std::stringstream ss;
    ss << input.rdbuf();

    result=ss.str();
    input.close();




    Tokenizer tokenizer(std::move(result));

    std::vector<Token> tokens =  tokenizer.totoken();

    Parser parser(std::move(tokens));
    std::optional<Node_Prog> tree = parser.parse_prog();

    if (!tree.has_value()) {
        std::cerr << "Invalid program" << std::endl;
        exit(1);

    }

    Generator generator(tree.value());



    {

        std::fstream file("out.s", std::ios::out);

        file << generator.generate_prog();
    }

    system("as -o out.o out.s");
    system("ld -macosx_version_min 13.0.0 -o out out.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64");


    return 0;

    
}