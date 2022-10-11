#include <iostream>
#include <string>
#include <chrono>
#include <map>


using namespace std;

//basic unit of a crypto currency
struct Tokens {
    string name;
    string symbol;
    double quantity;
    Tokens(string name, string symbol, double quantity): name(name), symbol(symbol), quantity(quantity){}
};

//ideally, we would have 2 agents, a liquidity provider and a trader
//but that is not needed for this assessment
//these agents could interact with the pool, using the setup

struct LiquidityProvider {

    LiquidityProvider(string id): id(id){}
    private:
        string id;
};


struct Trader {
    string id;
};

//the uniswap pair pool

struct UniswapPair {

    UniswapPair(Tokens a, Tokens b): token_a(a), token_b(b){
        //initialize
        k = token_a.quantity * token_b.quantity; 
    }


    double add_tokens(LiquidityProvider* lp, Tokens n_tok_a, Tokens n_tok_b){
        token_a.quantity += n_tok_a.quantity;
        token_b.quantity += n_tok_b.quantity;

        int prevshare = 0;
        if (lps.find(lp) != lps.end()){
            prevshare = lps[lp]; 
        }
        k = token_a.quantity * token_b.quantity;

        //update contribution of the lp
        lps[lp] = prevshare + (n_tok_a.quantity + n_tok_b.quantity);

        return lps[lp];
    }

    double retrieve_tokens(LiquidityProvider* lp, Tokens n_tok_a, Tokens n_tok_b){
        token_a.quantity -= n_tok_a.quantity;
        token_b.quantity -= n_tok_b.quantity;

        int prevshare = 0;
        
        if (lps.find(lp) != lps.end()){
            prevshare = lps[lp]; 
        }
        k = token_a.quantity * token_b.quantity;
        lps[lp] = prevshare + (n_tok_a.quantity + n_tok_b.quantity); 
        return lps[lp];
    }

    double calculate_a_b(Tokens n_tok_a){
        //calculation done without taking fee into account
        //this is like the theoretical/market price, without the exchange fees
        double val = token_b.quantity - k/(token_a.quantity + n_tok_a.quantity);
        return val;
    }

    double calculate_b_a(Tokens n_tok_b){
        double val = token_a.quantity - k/(token_b.quantity + n_tok_b.quantity);
        return val;
    }

    double swap_a_b(Tokens n_tok_a){
        //once the transaction is to take place, we add the fees
        
        Tokens without_fees = n_tok_a;
        without_fees.quantity /= (1+FEE);
        
        double val = calculate_a_b(without_fees);
        token_b.quantity -= val;
        token_a.quantity += n_tok_a.quantity;
        //update k as it changes
        k = token_a.quantity*token_b.quantity;
        return val;
    }

    double swap_b_a(Tokens n_tok_b){


        Tokens without_fees = n_tok_b;
        without_fees.quantity /= (1+FEE);

        double val = calculate_b_a(without_fees);
        token_a.quantity -= val; 
        token_b.quantity += n_tok_b.quantity;
        k = token_a.quantity*token_b.quantity;
        return val;
    }

    double get_k(){
        return k;
    }

    Tokens get_token_a_type(){
        return Tokens(token_a.name, token_a.symbol, 0);
    }

    Tokens get_token_b_type(){
        return Tokens(token_b.name, token_b.symbol, 0);
    }

    double get_conversion_b_a(){
        return token_a.quantity/token_b.quantity;
    }
    
    double get_conversion_a_b(){
        return token_b.quantity/token_a.quantity;
    }
    private:
        //tokens of each type
        Tokens token_a;
        Tokens token_b;

        //depends on the pair, but lets keep it the same for all
        const double FEE = 0.003;

        //this maintains the liquidity pool for each lp
        //the fee goes to each provider
        map<LiquidityProvider*, double> lps;
        
        //const product, which could change 
        double k;
};

//simulator, all calls will go from here
struct Setup {
    public: 
        void initialize(bool f, bool fast){
            //first argument is for verbosity
            //second one is for whether to run the slow version or fast version

            pool_a = new UniswapPair(Tokens("ethereum","ETH",100),Tokens("dai","DAI",100000));
            pool_b = new UniswapPair(Tokens("ethereum","ETH",150),Tokens("dai","DAI",120000));
            flag = f;
            fast_flag = fast;
            //not sure if this makes sense, the creation of pool is just for letting them have an initial
            //value
            // calculate_arbitrage_a();
        }

        //whenever any add/retrieve/swap takes place, look for arbitrage

        void add_tokens(LiquidityProvider *lp, UniswapPair* pool, Tokens token_a, Tokens token_b){
            pool->add_tokens(lp, token_a, token_b);
            if (fast_flag)
                calculate_arbitrage_a_fast();
            else 
                calculate_arbitrage_a();
            // we may want to try arbitrage for token b as well
            // calculate_arbitrage_b();
        }

        void retrieve_tokens(LiquidityProvider *lp, UniswapPair* pool, Tokens token_a, Tokens token_b){
            pool->retrieve_tokens(lp, token_a, token_b);
            if (fast_flag)
                calculate_arbitrage_a_fast();
            else 
                calculate_arbitrage_a();
            // calculate_arbitrage_b();
        }

        void swap_tokens(UniswapPair* pool, Tokens token){
            if (token.symbol == pool->get_token_a_type().symbol)
                pool->swap_a_b(token);
            if (token.symbol == pool->get_token_b_type().symbol)
                pool->swap_b_a(token);
            if (fast_flag)
                calculate_arbitrage_a_fast();
            else 
                calculate_arbitrage_a();
        }

        UniswapPair* get_pool_a(){
            return pool_a;
        }

        UniswapPair* get_pool_b(){
            return pool_b;
        }

    private:
        //binary search solution
        double find_profit(double tok_a, UniswapPair* buypool, UniswapPair* sellpool){
            Tokens tok_a_obj = buypool->get_token_a_type();
            Tokens tok_b_obj = sellpool->get_token_b_type();

            tok_a_obj.quantity = tok_a/(1+FEE);
            double tok_b = buypool->calculate_a_b(tok_a_obj);
            tok_b_obj.quantity = tok_b/(1+FEE);

            double tok_a_1 = sellpool->calculate_b_a(tok_b_obj);
            return tok_a_1 - tok_a;
        }
        void calculate_arbitrage_a_fast() {
            double delta = 0.001;

            //ideally, check if we can perform arbitrage
            //by seeing if types are the same


            //I dont think as traders, we would have this information
            //So ideally, this is not the best way to do it
            double price1 = pool_a->get_conversion_a_b();
            double price2 = pool_b->get_conversion_a_b();

            if (flag)
                cout<<price1<<" "<<price2<<endl;

            //find the appropriate pool
            UniswapPair* buypool = price1 > price2 ? pool_a : pool_b;
            UniswapPair* sellpool = price1 < price2 ? pool_a : pool_b;

            double max_profit = 0;
            double arg_max_profit = 0;

            double begin = 0.01, end = 100;
            while(end - begin > delta){
                double tok_a = (begin + end)/2;
                double p1 = find_profit(tok_a, buypool, sellpool);
                double p2 = find_profit(tok_a+delta, buypool, sellpool);
                if (p1 < p2){
                    max_profit = p2;
                    arg_max_profit = tok_a+delta;
                    begin = tok_a;
                }
                else {
                    max_profit = p1;
                    arg_max_profit = tok_a;
                    end = tok_a;
                }
            }
            if (flag){
                if (buypool == pool_a){
                    cout<<"Sell "<<arg_max_profit<<"of ETH from A and buy at B to get a profit of "<<max_profit<<endl;
                }
                else 
                    cout<<"Sell "<<arg_max_profit<<"of ETH from B and buy at A to get a profit of "<<max_profit<<endl;
            }
        }


        //simple iterative solution
        void calculate_arbitrage_a() {
            double delta = 0.001;

            //ideally, check if we can perform arbitrage
            //by seeing if types are the same


            //I dont think as traders, we would have this information
            //So ideally, this is not the best way to do it
            double price1 = pool_a->get_conversion_a_b();
            double price2 = pool_b->get_conversion_a_b();

            if (flag)
                cout<<price1<<" "<<price2<<endl;

            //find the appropriate pool
            UniswapPair* buypool = price1 > price2 ? pool_a : pool_b;
            UniswapPair* sellpool = price1 < price2 ? pool_a : pool_b;

            double max_profit = 0;
            double arg_max_profit = 0;

            //it turns out that we need only one txn
            //after which, we will not get any profit

            Tokens tok_a_obj = buypool->get_token_a_type();
            Tokens tok_b_obj = sellpool->get_token_b_type();

            for (double tok_a = 0.01; tok_a <= 100; tok_a += delta){

                tok_a_obj.quantity = tok_a/(1+FEE);
                double tok_b = buypool->calculate_a_b(tok_a_obj);
                tok_b_obj.quantity = tok_b/(1+FEE);

                double tok_a_1 = sellpool->calculate_b_a(tok_b_obj);

                //check if current number of A can be sold to get a profit
                //find max possible value
                if (tok_a_1 - tok_a > max_profit){
                    max_profit = tok_a_1 - tok_a;
                    arg_max_profit = tok_a;
                }
            }
            if (flag){
                if (buypool == pool_a){
                    cout<<"Sell "<<arg_max_profit<<"of ETH from A and buy at B to get a profit of "<<max_profit<<endl;
                }
                else 
                    cout<<"Sell "<<arg_max_profit<<"of ETH from B and buy at A to get a profit of "<<max_profit<<endl;
            }
        }

        void calculate_arbitrage_b() {
            //buy a at one and sell it at another
            //we can implement this later
        }

        const double FEE = 0.003;  
        bool flag;
        bool fast_flag;
        //ideally, we would a set of pools, and we check arbitrage between each pair of pools         
        UniswapPair* pool_a;
        UniswapPair* pool_b;
};