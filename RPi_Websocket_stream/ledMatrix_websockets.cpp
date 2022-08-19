#include "/home/ronshani/pxmatrix/include/led-matrix.h"
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include </home/ronshani/websocketpp/websocketpp/config/asio_no_tls_client.hpp>
#include </home/ronshani/websocketpp/websocketpp/client.hpp>
#include <iostream>



using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

class matdemo {
 RGBMatrix *mtrx;
 public:
  matdemo(){
  RGBMatrix::Options matrix_options;
  matrix_options.cols=64;
  matrix_options.rows=32;
  matrix_options.parallel=2;
  matrix_options.chain_length=4;
  matrix_options.pixel_mapper_config="V-mapper";
  rgb_matrix::RuntimeOptions runtime_opt;
  RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
  mtrx=matrix;
 }
    void rundemo(uint8_t *c){
        for (uint8_t x = 0; x < 127; ++x) {
                for (uint8_t y = 0; y < 128; ++y) {
                matdemo::mtrx->SetPixel(y,x,*c++,*c++,*c++);
	}}
	for (uint8_t y = 0; y < 127; ++y) {
                matdemo::mtrx->SetPixel(y,127,*c++,*c++,*c++);
	}
	matdemo::mtrx->SetPixel(127,127,*c++,*c++,*c);
    }
};

matdemo *demo = new matdemo();

typedef websocketpp::client<websocketpp::config::asio_client> client;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}
void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
        if (msg->get_opcode() == websocketpp::frame::opcode::text) {
		    std::cout << msg->get_payload() << std::endl;
        } else {
		std::string pld = msg->get_raw_payload();
		uint8_t* c = reinterpret_cast< uint8_t* >(&pld[0]);
		demo->rundemo(c);
		}
}
int main(int argc, char *argv[]) {
std::cout<< "finished loading" << std::endl;
client c;
std::string uri = argv[0];
if (argc == 2) {
      uri = argv[1];
  }

  try {
      c.set_access_channels(websocketpp::log::alevel::all);
      c.clear_access_channels(websocketpp::log::alevel::frame_payload);
      c.set_error_channels(websocketpp::log::elevel::all);

      c.init_asio();
      c.set_message_handler(&on_message);
      websocketpp::lib::error_code ec;
      client::connection_ptr con = c.get_connection(uri, ec);
      if (ec) {
          std::cout << "could not create connection because: " << ec.message() << std::endl;
	  return 0;
      }
      c.connect(con);
      c.run();
  }
  catch (websocketpp::exception const& e) { std::cout << e.what() << std::endl;
  }
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);
  return 0;
}
