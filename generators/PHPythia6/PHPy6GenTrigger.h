#ifndef PHPYTHIA6_PHPY6GENTRIGGER_H
#define PHPYTHIA6_PHPY6GENTRIGGER_H

#include <HepMC/GenEvent.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace HepMC
{
  class GenEvent;
};

class PHPy6GenTrigger {

 protected:  
  //! constructor
  PHPy6GenTrigger(const std::string &name = "PHPy6GenTrigger");

 public:
  virtual ~PHPy6GenTrigger();

  #ifndef __CINT__
  virtual bool Apply(const HepMC::GenEvent* evt) {
    std::cout << "PHPy8GenTrigger::Apply - in virtual function" << std::endl;
    return false;
  }
  #endif

  virtual std::string GetName() { return _name; }
  
  std::vector<int> convertToInts(std::string s);

  void Verbosity(int v) { _verbosity = v; }

protected:
  int _verbosity;  
  
private:
  std::string _name;
};

#endif	

