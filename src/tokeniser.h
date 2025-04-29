#pragma once

enum class TokenT {
    exit,
    int_literal,
    semicolon,
    open_paren,
    close_paren,
    idf,
    let,
    equals,
    plus,
    minus,

};




struct Token {

    TokenT type;
    std::optional<std::string> value;
};


class Tokenizer {

public:
    inline explicit Tokenizer(std::string str): cur_str(std::move(str)) {

    }

    inline std::vector<Token> totoken() {


        std::vector<Token> tokens;
        std::string buffer;
        while (peek().has_value()) {
            if (std::isalpha(peek().value())) {
                buffer.push_back(next());

                while (peek().has_value() && std::isalnum(peek().value())) {

                    buffer.push_back(next());
                }


                if (buffer == "exit") {
                    tokens.push_back({.type = TokenT::exit});
                    buffer.clear();
                    continue;
                }
                else if (buffer == "let") {
                    tokens.push_back({.type = TokenT::let});
                    buffer.clear();
                    continue;
                }
                else {
                    tokens.push_back({.type = TokenT::idf, .value = buffer});
                    buffer.clear();
                    continue;
                }
            }

            else if (std::isdigit(peek().value())) {
                buffer.push_back(next());

                while (peek().has_value() && std::isdigit(peek().value())) {

                    buffer.push_back(next());
                }

                tokens.push_back({.type = TokenT::int_literal, .value = buffer});
                buffer.clear();
                continue;


            }

            else if (peek().value() == '(') {

                next();
                tokens.push_back({.type = TokenT::open_paren});
                continue;

            }
            else if (peek().value() == ')') {
                next();
                tokens.push_back({.type = TokenT::close_paren});
                continue;

            }

            else if (peek().value() == ';') {
                next();
                tokens.push_back({.type = TokenT::semicolon});
                continue;


            }

            else if (peek().value() == '=') {
                next();
                tokens.push_back({.type = TokenT::equals});
                continue;


            }
            else if (peek().value() == '+') {

                next();
                tokens.push_back({.type = TokenT::plus});
                continue;
            }

            else if (peek().value() == '-') {
                next();
                tokens.push_back({.type = TokenT::minus});
                continue;
            }

            else if (std::isspace(peek().value())) {
                next();

            }
            else {
                std::cerr << "Error, wrong command, not a space" << std::endl;
                exit(1);

            }

        }
        cur_idx = 0;
        return tokens;









    }
private:

    const std::string cur_str;
    size_t cur_idx =0;



    [[nodiscard]] inline std::optional<char> peek(int dist = 0) const{
        if (cur_idx+dist >=cur_str.length()) {

            return std::nullopt; //bopppp
        }
        else {

            // std::cout << cur_str.at(cur_idx + dist) << std::endl;

            return cur_str.at(cur_idx + dist);
        }

    };

     char next() {
        return cur_str.at(cur_idx++);
    }

};