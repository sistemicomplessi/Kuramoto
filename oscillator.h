#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <complex>
#include <cassert>
#include <vector>
using namespace std::complex_literals;



class Oscillator{
 protected:
  //static double _K; //coupling strength 
  static double _dt;
  double _freq;   //frequency omega
  double _phase;  //phase phi/theta

  bool phaseNearZero(double limit = _dt*2) { return (_phase < limit || _phase > 2*M_PI - limit); }
  static void setDefaultDt() { _dt = 0.1; }
  std::complex<double> phasor() { return std::exp(1i*_phase); }                 //returns directly the exponential form
  static std::complex<double> orderParamether(std::vector<Oscillator>& system); //returns r: order parameter

 public:
  //if not indicated, frequencies and phases are set randomly
  Oscillator(double freq = -1, double phase = -1);
  
  static void setDt(double dt) { _dt = dt; }
  double freq() { return _freq; }
  void setFreq(double freq) { _freq = freq; }
  double phase() { return _phase; };
  void setPhase(double phase);
  
  //print the oscillator in ascii
  void print();
  //evolve the oscillator's dinamic
  void evolve();
  //Lorentz Distribution (allows to solve explicitly for r(K))
  double Lorentz_g(double freq, double gamma = 1){};

};


#endif //OSCILLATOR_H
