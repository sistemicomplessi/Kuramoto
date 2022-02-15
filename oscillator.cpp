#include "oscillator.h"
#include <vector>
#include <random>
#include <iostream>
using namespace std::complex_literals;

double lorentz_g(double freq, double gamma){
  if(gamma<0) { gamma = -1*gamma; }
  if(gamma>0.5) { std::cout<< "Setting gamma>0.5 will flatte the Loretzian density"<<'\n'; }
  return { gamma/ ( M_PI*(gamma*gamma + freq*freq) ) };
}

double Oscillator::_K; //parametro di accoppiamento
int Oscillator::_N;
//double Oscillator::_dt = -1;

Oscillator::Oscillator(double freq, double phase): _freq{freq} {
  if (freq == -1) {
    //generate random frequency according to lorentzian distribution  
    std::random_device seed;
    std::uniform_real_distribution<double> xDist(-4,4);       //max e min da sistemare
    std::uniform_real_distribution<double> yDist(0, 30.61);   //il max è il max della lorentziana

    double randomX;
    double randomY;
    do {
      randomX = xDist(seed);
      randomY = yDist(seed);
    } while (randomY > lorentz_g(randomX));

    _freq = randomX;
    //_freq = Lorentz_g(seed);  //ho fatto così per far generare le frequenze secondo la lorentziana //No. Questo restituisce la lorentziana con quel valore come freq
  }

  if (phase == -1) {
    std::random_device seed;   
    std::uniform_real_distribution<double> phaseDist(0, 2*M_PI);
    _phase = phaseDist(seed);
  }
  else 
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
  /*if (_dt == -1) {
    std::cerr << "WARN (21): _dt not set: using default value. Use Oscillator::setDt static function if you want to set it manually\n";
    //Mentre scrivevo sta riga ridevo perché pensavo che tanto il codice lo usiamo noi e sticazzi. Poi ho fatto tutto il main e non ho settato dt, ahahah
    setDefaultDt();
  }*/
  
  setPhase(_phase + _freq*2*M_PI*dt); //equals to _phase += _freq*dt  + normalize.
}

void Oscillator::interaction(std::vector<Oscillator>& system){
  double k = _K/_N;

  for(int i=0; i<=_N; ++i){  //ogni oscillatore deve interagire con tutti gli altri
    double theta_i = system[i]._phase;
    double Sin=0; 
    for(int j=0; j<=_N; ++j){
      while(j != i) {               //un oscillatore non interagisce con sé stesso
        double theta_j = system[j]._phase;
        Sin += std::sin(theta_j - theta_i);
      }  
    }

    _phase = ( _freq + k*Sin )* _dt;
  }
}