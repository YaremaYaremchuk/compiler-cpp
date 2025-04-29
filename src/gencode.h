#pragma once

#include <stack>

#include "./parser.h"


class Generator {
public:
    explicit inline Generator(Node_Prog prog) :cur_prog(std::move(prog)){




    }


    void generate_var( const Node_Var* var) {

        struct VarVisitor {

            Generator* gen;

            void operator()(const Node_VarIntLit* var_int_lit) const{

                gen->cur_result << "     mov x1, #" << var_int_lit->int_lit.value.value() << "\n";
                gen->push("x1");

            }
            void operator()(const Node_VarIdf* var_idf) const{


                if (!gen->cur_vars.contains(var_idf->idf.value.value())) {
                    std::cerr << "Error: identifier not found\n";
                    exit(1);

                }

                const auto& idx = gen->cur_vars.at(var_idf->idf.value.value());


                gen->cur_result << "    ldr x1, [sp, #"<< (gen->cur_stack_size - idx.stack_pos -1)*16  << "]\n";

                gen->push("x1");



            }


            void operator()(const Node_VarBracket* var_bracket) const {

                gen->generate_expr(var_bracket->expr);



            }
        };

        VarVisitor visitor({.gen = this});
        std::visit(visitor, var->vari);


    }

    void generate_bin_expr(const Binary_Expr* binary_expr) {

        struct BinVisitor {

            Generator* gen;
            void operator()(const Binary_Expr_Add* add)const {

                gen->generate_expr(add->lefts);
                gen->generate_expr(add->rights);
                gen->pop("x1");
                gen->pop("x2");
                gen->cur_result << "     add x1, x1, x2\n";
                gen->push("x1");

            }

            void operator()(const Binary_Expr_Sub* sub)const {

                gen->generate_expr(sub->rights);

                gen->generate_expr(sub->lefts);
                gen->pop("x1");
                gen->pop("x2");
                gen->cur_result << "     sub x1, x1, x2\n";
                gen->push("x1");



            }
        };


        BinVisitor visitor({.gen = this});

        std::visit(visitor, binary_expr->vari);
    }

    void generate_expr(const Node_Expr* expr) {


        struct ExprVisitor {

            Generator* gen;



            void operator()(const Node_Var* var) const {

                gen->generate_var(var);
            }

            void operator()(const Binary_Expr* binary_expr)const {

                gen->generate_bin_expr(binary_expr);



            }
        };

        ExprVisitor visitor({.gen = this});
        std::visit(visitor, expr->vari);






    }

    void generate_stat(const Node_Stat* stat) {


        struct Statvisitor {

            Generator* gen;
            void operator()(const Node_StatExit* stat_exit) const {

                gen ->generate_expr(stat_exit->expr);

                gen->pop("x0");
                gen->cur_result << "     mov x16, #1\n";


                gen->cur_result << "     svc #0x80\n";


            }
            void operator()(const Node_StatLet* stat_let) const {

                if (gen->cur_vars.contains(stat_let->idf.value.value())) {

                    std::cerr << "Already declared" << "\n";
                    exit(1);
                }

                gen->cur_vars.insert({stat_let->idf.value.value(), Var{.stack_pos = gen->cur_stack_size}});

                gen->generate_expr(stat_let->expr);



            }
        };

        Statvisitor visitor{.gen = this};
        std::visit(visitor, stat->vari) ;


    }




    [[nodiscard]] std::string generate_prog(){


        cur_result << ".global _start\n.align 2\n_start:\n";

        for (const Node_Stat* stat : cur_prog.stats) {

            generate_stat(stat);
        }


        cur_result << "     mov x0, #0\n";
        cur_result << "     mov x16, #1\n";
        cur_result << "     svc #0x80\n";

        return cur_result.str();



    }
private:


    void push(const std::string& reg) {


        cur_result << "     str " << reg << ", [sp, #-16]!\n";
        cur_stack_size++;
    }


    void pop(const std::string& reg) {

        cur_result << "     ldr " << reg << ", [sp], #16\n";
        cur_stack_size--;


    }

    struct Var {

        size_t stack_pos;
    };
    const Node_Prog cur_prog;

    std::stringstream cur_result;

    size_t cur_stack_size = 0;

    std::unordered_map<std::string, Var> cur_vars {};
};