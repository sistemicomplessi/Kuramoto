#include "firefly.h"
#include <thread>
#include <iostream>
#include <X11/Xlib.h> 

int main() {
  XInitThreads();  //serve per XServer

  Firefly::setWindowDim(1000,700);
  Firefly::setK(2000);

  std::vector<Firefly> sciame;
  for (int i = 0; i < 1000; i++) {
    sciame.push_back(Firefly(Distribution::Lorentz,1,0.5));
    //sciame.push_back(Firefly(0.2));
  }

  //fout.close();
  //std::cout << "Kc: " << 2/(M_PI*lorentz_g(0,1,0.5));
  
  //Draw window, plot window and evolve done in separeted threads, passing sciame as reference
  std::thread dThread(&Firefly::draw, std::ref(sciame));  
  std::thread pThread(&Firefly::plot, std::ref(sciame));
  std::thread eThread(&Firefly::evolve, std::ref(sciame), true);

  //wait for threads to terminate before proceding
  dThread.join(); 
  pThread.join();
  Firefly::stopEvolve();
  eThread.join();
  return 0;
}