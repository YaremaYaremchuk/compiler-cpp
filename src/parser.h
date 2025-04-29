#pragma once

#include "tokeniser.h"
#include <variant>

#include "arenaloc.h"


struct Node_VarIntLit {
    Token int_lit;
};

struct Node_VarIdf {
    Token idf;
};



struct Node_Expr;

struct Node_VarBracket {
    Node_Expr* expr;
};


struct Node_StatExit {

    Node_Expr* expr;

};

struct Node_StatLet {

    Token idf;

    Node_Expr* expr;

};

struct Node_Stat {

    std::variant<Node_StatExit*, Node_StatLet*> vari;
};


struct Node_Prog {

    std::vector<Node_Stat*> stats;


};



struct Binary_Expr_Add {

    Node_Expr* lefts;
    Node_Expr* rights;


};
struct Binary_Expr_Sub {

    Node_Expr* lefts;
    Node_Expr* rights;
};


struct Binary_Expr {
    std::variant<Binary_Expr_Add*, Binary_Expr_Sub*> vari;



};



struct Node_Var {

    std::variant<Node_VarIntLit*, Node_VarIdf*, Node_VarBracket*> vari;




};
struct Node_Expr {

    std::variant<Node_Var*, Binary_Expr*> vari;
};


class Parser {

    public:

        inline explicit Parser(std::vector<Token> tokens) :curtokens(std::move(tokens)), cur_alloc(1024*1024*4) {




        }


        std::optional<Node_Var*> parse_var() {







            if (peek().has_value() && peek().value().type == TokenT::int_literal) {

                auto var_int_lit = cur_alloc.alloc<Node_VarIntLit>();
                var_int_lit->int_lit = next();

                auto var = cur_alloc.alloc<Node_Var>();
                var->vari = var_int_lit;


                return var;


            }
            else if (peek().has_value() && peek().value().type == TokenT::idf) {

                auto var_idf = cur_alloc.alloc<Node_VarIdf>();
                var_idf->idf = next();

                auto var = cur_alloc.alloc<Node_Var>();
                var->vari = var_idf;

                return var;
            }
            else if(peek().has_value() && peek().value().type == TokenT::open_paren) {
                next();

                auto expr = parse_expr();

                if (!expr.has_value()) {
                    std::cerr << "Error parsing brackets expression\n";
                    exit(1);
                }

                if (!peek().has_value() || peek().value().type != TokenT::close_paren) {
                    std::cerr << "Error no closing bracket\n";
                    exit(1);

                }
                next();



                auto var_bracket = cur_alloc.alloc<Node_VarBracket>();
                var_bracket->expr = expr.value();


                auto var = cur_alloc.alloc<Node_Var>();
                var->vari = var_bracket;
                return var;



            }
            else {
                return std::nullopt;
            }


        }

        std::optional<Node_Expr*> parse_expr(int ord = 0) {


            std::optional<Node_Var*> var_lefts = parse_var() ;
            if (!var_lefts.has_value()) {
                return {};
            }
            auto expr_lefts = cur_alloc.alloc<Node_Expr>();
            expr_lefts->vari = var_lefts.value();



            while (true) {
                std::optional<Token> cur_tok = peek();
                if (!cur_tok.has_value() || (cur_tok.value().type != TokenT::plus && cur_tok.value().type != TokenT::minus) || ord == 1) {
                    break;

                }






                Token op = next();

                auto expr_rights = parse_expr(1);

                if (!expr_rights.has_value()) {
                    std::cerr << "Error parsing right expression" << std::endl;
                    exit(1);
                }


                auto expr = cur_alloc.alloc<Binary_Expr>();

                auto expr_lcopy = cur_alloc.alloc<Node_Expr>();



                if (op.type == TokenT::plus) {

                    auto add = cur_alloc.alloc<Binary_Expr_Add>();

                    expr_lcopy->vari=expr_lefts->vari;

                    add->lefts = expr_lcopy;

                    add->rights = expr_rights.value();

                    expr->vari = add;



                }
                else if (op.type == TokenT::minus) {

                    auto sub = cur_alloc.alloc<Binary_Expr_Sub>();

                    expr_lcopy->vari=expr_lefts->vari;

                    sub->lefts = expr_lcopy;

                    sub->rights = expr_rights.value();

                    expr->vari = sub;


                }

                expr_lefts->vari = expr;
            }

            return expr_lefts;

        }


        std::optional<Node_Stat*> parse_stat() {


            if (peek().value().type == TokenT::exit && peek(1).has_value() && peek(1).value().type == TokenT::open_paren) {
                next();
                next();
                auto stat_exit = cur_alloc.alloc<Node_StatExit>();
                if (auto node_expr = parse_expr()) {
                    stat_exit->expr =  node_expr.value();
                }
                else {
                    std::cerr << "Error parsing expression" << std::endl;
                    exit(1);
                }

                if (peek().has_value() && peek().value().type == TokenT::close_paren) {
                    next();
                }
                else {
                    std::cerr << "Error no )" << std::endl;
                    exit(1);

                }

                if (peek().has_value() && peek().value().type == TokenT::semicolon) {
                    next();

                }
                else {

                    std::cerr << "Error no ;" << std::endl;
                    exit(1);
                }

                auto stat = cur_alloc.alloc<Node_Stat>();
                stat->vari= stat_exit;

                return stat;
            }
            else if (peek().has_value() && peek().value().type == TokenT::let && peek(1).has_value()
                && peek(1).value().type == TokenT::idf
                && peek(2).has_value() && peek(2).value().type == TokenT::equals) {


                next();

                auto stat_let = cur_alloc.alloc<Node_StatLet>();
                stat_let->idf = next();
                next();
                if (auto expr = parse_expr()) {

                    stat_let->expr = expr.value();




                }
                else {


                    std::cerr << "Error parsing expression" << std::endl;
                    exit(1);
                }

                if (peek().has_value() && peek().value().type == TokenT::semicolon) {

                    next();
                }
                else {
                    std::cerr << "Error no ;" << std::endl;
                    exit(1);
                }

                auto stat = cur_alloc.alloc<Node_Stat>();
                stat->vari= stat_let;

                return stat;


            }
            else {
                return {};

            }


        }

        std::optional<Node_Prog> parse_prog() {

            Node_Prog prog;


            while (peek().has_value()) {

                if (auto stat = parse_stat()) {
                    prog.stats.push_back(stat.value());

                }
                else {
                    std::cerr << "Error parsing program" << std::endl;
                    exit(1);
                }


            }

            return prog;
        }




    private:

        [[nodiscard]] inline std::optional<Token> peek(int dist = 0) const{
            if (cur_idx+dist >= curtokens.size()) {

                return std::nullopt; //bopppp
            }
            else {

                return curtokens.at(cur_idx + dist);
            }

        };

        inline Token next() {
            return curtokens.at(cur_idx++);
        }

        const std::vector<Token> curtokens;

        size_t cur_idx = 0;


        ArenaAllocator cur_alloc;

    };
