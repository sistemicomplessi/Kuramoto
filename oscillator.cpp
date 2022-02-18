#include "oscillator.h"
#include <vector>
#include <random>
#include <iostream>
using namespace std::complex_literals;

double lorentz_g(double freq, double mean, double gamma){
  if(gamma < 0) { gamma = -gamma; }
  if(gamma > 1) { 
    if (gamma != 998)
      std::cout<< "Setting gamma > 1 will flatten the Loretzian density"<<'\n';
    gamma = 0.5;  //ho visto che in varie simulazioni online usavano 0.5 per gamma
  }
  return gamma/ ( M_PI*(gamma*gamma + (freq-mean)*(freq-mean)) );

}

double gauss_g(double freq, double mean, double sigma){
  double c = 1/sqrt(2*M_PI*sigma);
  return c*std::exp( (freq-mean)*(freq-mean) / (-2*sigma));
}

//Non ho capito bene che distribuzione sarebbe. Mi servirebbe poi un modo per calcolare la media, in analogia con le altre distribuzioni
/*
*è la distribuzione di maxwell boltzmann, quella del microcanonico in fisica della materia: ti dice che le frequenze 
*sono maggiormente distribuite vicino a Omega (che sarebbe la frequenza media caratteristica del sistema) e la 
*probabilità di avere freq>Omega diminuisce esponenzialmente. La media è Omega^2. Problema che ho realizzato dopo avere scritto tutto: non è simmetrica 
*rispetto all'origine, quindi succederà un macello? da vedere
*/
double boltzmann_g(double freq, double Omega){ //Maxwell-Boltzmann distribution, frequency has the same interpretation as Energy 
  if (Omega == 998)
    Omega = 2;    //Qualcosa di accettabile
  return std::exp(-freq/Omega);
}

double exp_g(double freq, double mean) {  //questa che sarebbe?
  return std::exp(-freq/mean) / mean;
}

Oscillator::Oscillator(double freq, double phase) : _freq{freq} {
  if (phase == -1) {
    //generate phase randomly
    std::random_device seed;   
    std::uniform_real_distribution<double> phaseDist(0, 2*M_PI);
    _phase = phaseDist(seed);
    return;
  }
  setPhase(phase);
}

//definire i parametri di campo medio
std::complex<double> Oscillator::orderParameter(std::vector<Oscillator>& system) {
  double cosAll;
  double sinAll;
  int n = system.size();
  for(int i = 0; i < n; ++i){
    cosAll += std::cos(system[i].phase());  //cosAll è la somma di tutti i coseni di theta_i
    sinAll += std::cos(system[i].phase());  //stesso per sinAll
  }
  double x = cosAll/n;
  double y = sinAll/n;           //possiamo vedere la somma dei fasori come COS/N + i*SIN/N = X + iY. Da qui lo riportiamo in exp
  double r = std::sqrt(x*x + y*y);   
  double psi = std::atan(y/x);        //non è così semplice calcolare la fase, non so se esiste qualche funzione che considera i vari casi possibili
  return std::polar(r, psi);  //polar costruisce un exp complesso
}

Oscillator::Oscillator(Distribution dist, double mean, double param) {
  
  //generate random frequency according to the selected distribution  
  double randomX;
  double randomY;
  std::random_device seed;
  
  if (dist == Distribution::Lorentz) {
    std::uniform_real_distribution<double> xDist(-4,4);
    std::uniform_real_distribution<double> yDist(0, lorentz_g(0,mean, param));   //max is for freq = 0.
    do {
      randomX = xDist(seed);
      randomY = yDist(seed);
    } while (randomY > lorentz_g(randomX, param));
  }
  else if (dist == Distribution::Gauss) {
    std::uniform_real_distribution<double> xDist(-4,4);
    std::uniform_real_distribution<double> yDist(0, gauss_g(0, mean, param));
    do {
      randomX = xDist(seed);
      randomY = yDist(seed);
    } while (randomY > gauss_g(randomX, mean, param));
  }
  else if (dist == Distribution::Boltzmann) {
    std::uniform_real_distribution<double> xDist(-4,4);   //il min e max della x
    std::uniform_real_distribution<double> yDist(0, 50);  //da calcolare il max (di boltzmann)
    do {
      randomX = xDist(seed);
      randomY = yDist(seed);
    } while (randomY > boltzmann_g(randomX, param));
  }
  else if (dist == Distribution::Expo) {
    std::uniform_real_distribution<double> xDist(0,4);  //da calcolare meglio il max
    std::uniform_real_distribution<double> yDist(0, exp_g(0, mean));
    do {
      randomX = xDist(seed);
      randomY = yDist(seed);
    } while (randomY > exp_g(randomX, mean));
  }
  _freq = randomX;

  //generate phase randomly
  std::random_device seedPhase;   
  std::uniform_real_distribution<double> phaseDist(0, 2*M_PI);
  _phase = phaseDist(seedPhase);
}

void Oscillator::setPhase(double phase) {
  while (phase < 0) {
    phase += 2 * M_PI;
  }
  while (phase >= 2*M_PI) {
    phase -= 2 * M_PI;
  }
  _phase = phase;
}

void Oscillator::print() {
  
  //just for debug
  // system("cls");
  // std::cout << "dt: " << _dt << '\n';
  // std::cout << "phase: " << _phase << " - freq: " << _freq << '\n';
  
  if (std::sin(_phase) > 0.9)
    std::cout << "X ";
  else
    std::cout << ". "; 
}

void Oscillator::evolve(double dt) {   
  setPhase(_phase + _freq*2*M_PI*dt); //equals to _phase += _freq*dt  + normalize.
}

std::string toString(double num) {
  if (num == -1)
    return "[random]";
  return std::to_string(num);
}