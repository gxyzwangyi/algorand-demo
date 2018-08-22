// block_test.cpp: tests class Block using boost test.
// Note: compile using g++ -std=c++14 -I (boost dir) block_test.cpp -o 
//      block_test.exe

#define BOOST_TEST_MODULE block_test
#include <boost/test/included/unit_test.hpp>
#include "../src/block.h"
#include <random>
#include <iostream>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

namespace {
    class MyMessage : public MessageBase {
    public:
        explicit MyMessage(const uint256_t &num) : _num(num) {}
        virtual uint256_t hash() const override {
            return _num;
        }
    protected:
        uint256_t _num;
    };

    struct BlockComp {
        template<typename Alloc>
        bool operator()(const Block<Alloc> *lhs, const Block<Alloc> *rhs)
            const {
            return lhs->height < rhs->height;
        }
    };

    template<typename FwdIt, typename Eng>
    inline decltype(auto) random_choose(FwdIt first, FwdIt last, Eng &&eng) {
        return ::next(first, uniform_int_distribution<size_t>(0, 
            ::distance(first, last) - 1)(eng));
    }

    template<typename Alloc>
    inline bool is_ancestor(const Block<Alloc> *a, const Block<Alloc> *x) {
        for (auto p = x; p; p = p->prev)
            if (a == p)
                return true;
        return false;
    }

    template<typename Alloc>
    inline bool is_common_ancestor(const Block<Alloc> *a, 
        const Block<Alloc> *x, const Block<Alloc> *y) {
        return is_ancestor(a, x) && is_ancestor(a, y);
    }
}

BOOST_AUTO_TEST_SUITE(BlockTest)
BOOST_AUTO_TEST_CASE(get_skip_height_test) {
    cout << "Testing detail::get_skip_height:" << endl;
    default_random_engine eng(random_device{}());
    uniform_int_distribution<size_t> rand_int;
    for (size_t i = 0; i != 1024; ++i) {
        auto x = rand_int(eng);
        auto h = ALGORAND detail::get_skip_height(x);
        BOOST_TEST(h <= x);
    }
    cout << "Done." << endl;
}

BOOST_AUTO_TEST_CASE(Block_test) {
  cout << "Testing Block:" << endl;

  vector<Block<>*> blocks;
  blocks.push_back(new Block<>(nullptr));
  blocks.back()->messages.push_back(make_shared<MyMessage>(0));
  for (size_t i = 1; i != 16; ++i) {
      blocks.push_back(new Block<>(blocks.back()));
      blocks.back()->messages.push_back(make_shared<MyMessage>(i));
      blocks.rbegin()[1]->next = blocks.back();
  }
  
  for (auto &&b : blocks) {
      cout << *b << ", ";
      if (b->skip)
          cout << "skip->height=" << b->skip->height << endl;
      else
          cout << "skip=nullptr" << endl;
  }

  for (auto &&b : blocks)
      delete b;

  cout << "Done." << endl;
}

BOOST_AUTO_TEST_CASE(last_common_ancestor_test) {
    cout << "Testing last_common_ancestor:" << endl;

    default_random_engine eng(random_device{}());
    vector<Block<> *> block_vctr; 
    multimap<size_t, Block<> *> block_map;
    {
        auto genesis_block = new Block<>(nullptr);
        block_vctr.push_back(genesis_block);
        block_map.emplace(genesis_block->height, genesis_block);
    }
    for (size_t i = 1; i != 1024; ++i) {
        auto prev = *random_choose(block_vctr.cbegin(), block_vctr.cend(), 
            eng);
        auto new_block = new Block<>(prev);
        block_vctr.push_back(new_block);
        block_map.emplace(new_block->height, new_block);
    }

    for (size_t i = 0; i != 1024; ++i) {
        auto x = *random_choose(block_vctr.cbegin(), block_vctr.cend(), eng);
        auto y = *random_choose(block_vctr.cbegin(), block_vctr.cend(), eng);
        auto lca = last_common_ancestor(x, y);
        BOOST_TEST(lca);
        BOOST_TEST(lca->height <= x->height);
        BOOST_TEST(lca->height <= y->height);
        BOOST_TEST(is_common_ancestor(lca, x, y));
        auto &&candidates = block_map.equal_range(lca->height + 1);
        BOOST_TEST(all_of(candidates.first, candidates.second, 
            [&](const auto &p) {
            return !is_common_ancestor(p.second, x, y);
        }));
    }

    for (auto &&b : block_vctr)
            delete b;

    cout << "Done." << endl;
}
BOOST_AUTO_TEST_SUITE_END()