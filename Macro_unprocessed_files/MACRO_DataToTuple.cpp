// Description : Scrip that reads a folder containing oscilloscope output (Time, Amplitude)
//               and turns it into a TNtuple


// Usage : Put this script inside the data folder and execute from there
//         root MACRO_DataToTuple.cpp


// WARNING : the data folder should contain only data in .txt or .dat format!


// Customize the name of the output root file
TString ntuple_file_name = "noise_amplitude_ntuple_2.root";
// Customize the output folder
TString output_folder    = "/home/esteban/work/4d-tracking/data/name-of-the-folder"
  
void MACRO_DataToTuple(){
  gSystem->Exec("ls -1 > list_of_files.txt");

  std::ifstream file_list("list_of_files.txt");
  std::string file_name;

  TTree* ntuple_data = new TTree("ntuple_data","Time and Amplitude");

  while(std::getline(file_list,file_name)){
    std::string out_name       = file_name+".root";
    const char* out_name_valid = out_name.c_str();
    if(file_name=="MACRO_DataToTuple.cpp" || file_name=="list_of_files.txt") continue;
    std::cout<<"Reading "<<out_name<<std::endl;
    TFile* f = new TFile(out_name_valid,"RECREATE"); // Ojo con esta parte!
    gROOT->cd();
      
    ntuple_data->ReadFile(file_name.c_str(),"time/D:ampl/D");

    f->cd();
    ntuple_data->Write();
    
    delete f;
    ntuple_data->Reset(); //o algo asi
    if(ntuple_data->GetEntries()!=0){
      std::cout<<"Tuple didnt cleaned!"<<std::endl;
    }
  }

  gSystem->Exec("rm list_of_files.txt");

  gSystem->Exec("hadd "+ntuple_file_name+" *.root && mv "+ntuple_file_name+" "+output_folder);

  gSystem->Exec("rm *.root");
}
