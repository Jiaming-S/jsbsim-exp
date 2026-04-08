#include <FGFDMExec.h>
#include <initialization/FGInitialCondition.h>

#include <iostream>


int main() {
  std::shared_ptr<JSBSim::FGFDMExec> fdmexec = std::make_shared<JSBSim::FGFDMExec>();
  fdmexec->SetAircraftPath(SGPath("data/aircraft"));
  fdmexec->SetEnginePath(SGPath("data/engine"));
  fdmexec->SetSystemsPath(SGPath("data/systems"));
  fdmexec->SetOutputPath(SGPath("output"));
  fdmexec->LoadModel("f16");

  std::shared_ptr<JSBSim::FGInitialCondition> ic = fdmexec->GetIC();
  ic->InitializeIC();
  ic->Load(SGPath("reset00.xml"));

  fdmexec->RunIC();

  for (long it = 0; fdmexec->Run(); it++) {
    if (it == 10) break;
  }

  return 0;
}
