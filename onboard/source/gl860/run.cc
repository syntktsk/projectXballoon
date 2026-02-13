#include "GL860main.hh"
#include <iostream>
#include <cstring>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace balloon;

int main(){
// initialize
	GL860main obj;
	/*check*/ 
	std::string ip = "192.168.1.100";
	int port = 8023;
	std::string directry = "./";
	/*check*/ 
	std::string command;
	std::string response;

	if (!obj.initialize(ip, port)) {
        std::cerr << "Initialization failed!" << std::endl;
        return 1;
    }
	std::cout << "connection started!"<< std::endl;

	auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_c);
    std::stringstream ss;
    ss << directry <<"log_" << std::put_time(&now_tm, "%Y%m%d_%H%M%S") << ".txt";
    std::string text_title = ss.str();
	std::ofstream outputfile(text_title);
	if (!outputfile.is_open()) {
    std::cerr << "cannnot open file" << std::endl;
    return 1;
	}

	int count = 0;

//analysis
	while(1){
		count++;
		command = "";
		response = "";
		std::cout << "enter command >" << std::endl;	
		// std::cin >> command;
		std::getline(std::cin, command);
		if (command == "exit" || command == "quit") break;
		if ( command == "help"){
			std::cout<<"-----------GL860 command example -----------"<<std::endl;
			std::cout<<":IF:CONIF? 接続参照"<<std::endl;
			std::cout<<":MEAS:OUTP:ONECSV? 瞬時値(テキスト型)"<<std::endl;
			std::cout<<":MEAS:OUTP:ORDER? データの並び参照"<<std::endl;
			std::cout<<":MEAS:OUTP:CLR? バッファクリア"<<std::endl;
			std::cout<<":MEAS:START/STOP 収録の開始/停止"<<std::endl;
			std::cout<<":MEAS:OUTP:ACK? バッファ内の転送"<<std::endl;
			std::cout<<"-------------------------------------------"<<std::endl;
			continue;
		}
		response = obj.sendAndReceive(command);
		std::cout << "response:"<<response<<std::endl;
		outputfile << "No: " << count << "\n"
                   << "Cmd> " << command << "\n"
                   << "Res> " << response << "\n" << std::endl;
	}

//finalize 
	outputfile.close();
	obj.finalize();
	std::cout << "safely finished!"<< std::endl;
	return 0;
}