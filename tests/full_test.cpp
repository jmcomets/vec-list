#include <vec_list>

void test_basic_usage()
{
    vec_list<int> ls;
    assert(ls.size() == 0);
    assert(ls.empty());
    ls.push_back(1);
    ls.push_back(2);
    ls.push_back(3);
    assert(ls.front() == 1);
    assert(ls.back() == 3);
    assert(ls.size() == 3);
    ls.pop_back();
    assert(ls.back() == 2);
    ls.pop_back();
    assert(ls.front() == 1);
    assert(ls.size() == 1);
    ls.push_back(-2);
    assert(ls.size() == 2);
    ls.pop_back();
    ls.pop_back();
    assert(ls.size() == 0);
    assert(ls.empty());
}

void test_insertion()
{
    vec_list<int> ls;
    auto it2 = ls.insert(ls.end(), 2);
    assert(ls.back() == 2);
    ls.insert(ls.begin(), 0);
    assert(ls.front() == 0);
    assert(ls.size() == 2);
    auto it1 = ls.insert(it2, 1);
    assert(ls.size() == 3);
    assert(ls.back() == 2);
    ls.erase(ls.end());
    assert(ls.back() == 1);
    ls.erase(it1);
    assert(ls.back() == 0);
    ls.erase(ls.begin());
    assert(ls.empty());
}

int main()
{
    test_basic_usage();
    test_insertion();
    return 0;
}
