#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <upcxx/upcxx.hpp>



class UserType {
    public:
    int member1, member2;
    std::vector<int> heija;


};


int main(int argc, char* argv[]) {

    
    upcxx::init();
    if(upcxx::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;
    upcxx::barrier();



    std::cout << "hello world " << std::endl;

    if(upcxx::rank_me() == 0)
    {
        UserType ut;
        ut.member1 = 45;
        ut.member2 = 123;
        ut.heija.push_back(666);
        ut.heija.push_back(665);

        upcxx::global_ptr<UserType> utp = upcxx::new_<UserType>();


        std::cout << utp->member1 << std::endl;
        upcxx::rpc_ff(0, [](UserType new_obj) {

            /*std::cout << new_obj.member1 << " " << new_obj.member2 << std::endl;
            for(auto it : new_obj.heija) {
                std::cout << it << std::endl;
            }*/
        }, ut);

    }



    upcxx::barrier();
	upcxx::finalize();
}
