#include <gmock/gmock.h>
#include <ai/solving/ZOL.hpp>

namespace ai {
namespace solving {

TEST(ZOL, variable) {
    Variable p("p");
    VAR(q);
    VAR(pro);

    ASSERT_TRUE(p == p);
    ASSERT_TRUE(p != q);
    ASSERT_EQ(q.name(), "q");
    ASSERT_EQ(pro.name(), "pro");
}

TEST(ZOL, Formula) {
    VAR(p);
    VAR(q);
    VAR(r);
    Formula f = p;
    auto g = And(p, q);
    ZOLMap map;

    map[p] = true;
    map[q] = false;
    ASSERT_TRUE(f.evaluate(map));
    ASSERT_FALSE(g.evaluate(map));

    map[p] = false;
    map[q] = true;
    ASSERT_FALSE(f.evaluate(map));
    ASSERT_FALSE(g.evaluate(map));

    map[p] = true;
    map[q] = true;
    ASSERT_TRUE(g.evaluate(map));

    auto h = Or(r, g);

    map[p] = true;
    map[q] = false;
    map[r] = false;
    ASSERT_EQ(h.evaluate(map), map[p] && map[q] || map[r]);

    map[p] = true;
    map[q] = false;
    map[r] = true;
    ASSERT_EQ(h.evaluate(map), map[p] && map[q] || map[r]);

    auto k = Imply(p, q);
    map[p] = false;
    map[q] = false;
    ASSERT_EQ(k.evaluate(map), !map[p] || map[q]);
    map[p] = true;
    map[q] = false;
    ASSERT_EQ(k.evaluate(map), !map[p] || map[q]);

    auto l = Equiv(p, q);
    map[p] = false;
    map[q] = false;
    ASSERT_TRUE(l.evaluate(map));
    map[p] = true;
    map[q] = false;
    ASSERT_FALSE(l.evaluate(map));
    map[p] = true;
    map[q] = true;
    ASSERT_TRUE(l.evaluate(map));

    f = q;
    auto m = Not(f);
    map[q] = true;
    ASSERT_EQ(m.evaluate(map), !map[q]);
    map[q] = false;
    ASSERT_EQ(m.evaluate(map), !map[q]);

    f = p;
    map[q] = true;
    ASSERT_EQ(m.evaluate(map), !map[q]);
    map[q] = false;
    ASSERT_EQ(m.evaluate(map), !map[q]);
}

TEST(ZOL, equals) {
    VAR(a); VAR(b); VAR(c);
    ASSERT_TRUE(Imply(a, b) == Imply(a, b));
    ASSERT_FALSE(Imply(a, b) == Or(a, b));
    ASSERT_FALSE(Or(a, b) == And(a, b));
    ASSERT_FALSE(Imply(a, b) == Imply(a, c));
    ASSERT_TRUE(Or(a, b) == Or(b, a));
    ASSERT_TRUE(And(a, b) == And(b, a));
    // ASSERT_TRUE(Or(a, Or(b, c)) == Or(b, Or(a, c)));
}

TEST(ZOL, replace_imply_equiv_for_imply) {
    VAR(a); VAR(b); VAR(c);
    Formula f = Imply(a, b);

    replace_imply_equiv(f);
    Formula g = Or(Not(a), b);
    Formula h = Or(a, b);

    ASSERT_TRUE(f == g);
    ASSERT_FALSE(f == h);

    Formula m = Imply(a, Imply(b, c));
    Formula n = Or(Not(a), Or(Not(b), c));
    ASSERT_TRUE(m != n);
    replace_imply_equiv(m);
    ASSERT_TRUE(m == n);
    replace_imply_equiv(n);
    ASSERT_TRUE(m == n);
}

TEST(ZOL, replace_imply_equiv_for_equiv) {
    VAR(a); VAR(b); VAR(c);
    auto f = Equiv(a, b);
    auto g = Or(And(a, b), And(Not(a), Not(b)));
    ASSERT_TRUE(f != g);
    replace_imply_equiv(f);
    ASSERT_TRUE(f == g);
}

TEST(ZOL, push_down_not) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    auto f = Not(And(a, b));
    auto g = Or(Not(a), Not(b));
    push_down_not(f);
    ASSERT_TRUE(f == g);

    auto m = And(Not(And(Not(a), Or(b, c))), d);
    push_down_not(m);
    auto n = And(Or(a, And(Not(b), Not(c))), d);
    ASSERT_TRUE(m == n);

    f = Not(a);
    push_down_not(f);
    ASSERT_TRUE(f == Not(a));
}

void push_down_or(Formula& f);

TEST(ZOL, push_down_or) {
    VAR(a); VAR(b); VAR(c); VAR(d);

    auto f = And(a, b);
    push_down_or(f);
    ASSERT_TRUE(f == And(a, b));

    f = Not(a);
    push_down_or(f);
    ASSERT_TRUE(f == Not(a));

    f = a;
    push_down_or(f);
    ASSERT_TRUE(f == a);

    f = Or(And(a, b), c);
    push_down_or(f);
    ASSERT_TRUE(f == And(Or(a, c), Or(b, c)));

    f = Or(a, And(b, c));
    push_down_or(f);
    ASSERT_TRUE(f == And(Or(a, b), Or(a, c)));
}

TEST(ZOL, CNF) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    auto f = Not(And(a, b));
    CNF(f);
    auto g = Or(Not(a), Not(b));
    ASSERT_TRUE(f == g);

    f = Or(a, And(b, c));
    CNF(f);
    g = And(Or(a, b), Or(a, c));
    ASSERT_TRUE(f == g);
}

TEST(ZOL, CNF2) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    auto f = Imply(a, b);
    auto g = Or(Not(a), b);
    CNF(f);
    ASSERT_TRUE(f == g);
}

TEST(ZOL, CNF3) {
    VAR(a); VAR(b); VAR(c); VAR(d);

    auto f = Imply(Imply(a, b), Imply(c, d));
    auto k = Or(Not(c), d);

    CNF(f);
    auto g = And(Or(a, k), Or(Not(b), k));
    ASSERT_TRUE(f == g);
}

TEST(ZOL, DisjunctForm_equals) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    DisjunctForm f = {a, Not(b), c};
    DisjunctForm g = {Not(b), a, c};
    DisjunctForm h = {Not(b), d, c};
    ASSERT_TRUE(f == g);
    ASSERT_TRUE(f != h);
}

TEST(ZOL, to_disjunction_list) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    auto f = Or(a, b);
    std::vector<DisjunctForm> forms = to_disjunction_list(f);
}

} // namespace solving
} // namespace ai

