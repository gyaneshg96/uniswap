#include "classes.cpp"
#include <chrono>

using namespace std::chrono;

void basic_test(){
    Setup test_setup = Setup();
    
    LiquidityProvider *lp1 = new LiquidityProvider("lp1"); 
    LiquidityProvider *lp2 = new LiquidityProvider("lp2");

    test_setup.initialize(true, false);

    UniswapPair* pool_a = test_setup.get_pool_a();
    UniswapPair* pool_b = test_setup.get_pool_b();

    //testing calculate functions
    cout<<pool_a->calculate_a_b(Tokens("ethereum","ETH",10))<<endl;
    cout<<pool_a->calculate_b_a(Tokens("dai","DAI",2000))<<endl;
    cout<<pool_b->calculate_a_b(Tokens("ethereum","ETH",30))<<endl;
    cout<<pool_b->calculate_b_a(Tokens("dai","DAI",10000))<<endl;

    // //testing swapping functions
    // //ignoring arbitrage for now



    cout<<pool_a->get_k()<<endl;
    test_setup.add_tokens(lp1, pool_a, Tokens("ethereum","ETH",10), Tokens("dai","DAI",8000));
    cout<<pool_a->get_k()<<endl;
    cout<<pool_b->get_k()<<endl;
    test_setup.add_tokens(lp1, pool_b, Tokens("ethereum","ETH",20), Tokens("dai","DAI",15000));
    cout<<pool_b->get_k()<<endl;

    cout<<pool_a->get_k()<<endl;
    test_setup.swap_tokens(pool_a, Tokens("ethereum","ETH",10));
    cout<<pool_a->get_k()<<endl;
    test_setup.swap_tokens(pool_a, Tokens("dai","DAI",2000));
    cout<<pool_a->get_k()<<endl;


    cout<<pool_b->get_k()<<endl;
    test_setup.swap_tokens(pool_b, Tokens("ethereum","ETH",30));
    cout<<pool_b->get_k()<<endl;
    test_setup.swap_tokens(pool_b, Tokens("dai","DAI",10000));
    cout<<pool_b->get_k()<<endl;

}
void benchmark_slow(){
    Setup test_setup = Setup();
    
    LiquidityProvider *lp1 = new LiquidityProvider("lp1"); 
    LiquidityProvider *lp2 = new LiquidityProvider("lp2");

    test_setup.initialize(false, false);

    UniswapPair* pool_a = test_setup.get_pool_a();
    UniswapPair* pool_b = test_setup.get_pool_b();


    auto start = high_resolution_clock::now();
    for (int i = 0; i < 10000;i++){
        int eth = rand()%20;
        int dai = rand()%20000;
        test_setup.add_tokens(lp1, pool_a, Tokens("ethereum","ETH",eth), Tokens("dai","DAI",dai));
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
 
    cout << (double)duration.count()/1000 << endl;
}
void benchmark_fast(){
    Setup test_setup = Setup();
    
    LiquidityProvider *lp1 = new LiquidityProvider("lp1"); 
    LiquidityProvider *lp2 = new LiquidityProvider("lp2");

    test_setup.initialize(false, true);

    UniswapPair* pool_a = test_setup.get_pool_a();
    UniswapPair* pool_b = test_setup.get_pool_b();

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 10000;i++){
        int eth = rand()%20;
        int dai = rand()%20000;
        test_setup.add_tokens(lp1, pool_a, Tokens("ethereum","ETH",eth), Tokens("dai","DAI",dai));
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
 
    cout << (double)duration.count()/1000 << endl;
}

int main(){
    basic_test();
    benchmark_slow();
    benchmark_fast();
    return 0;
}