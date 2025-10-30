#include <iostream>
#include <string>

int main()
{
    std::cout << "\n*-*-*-*-* ASTRA SIMULATOR *-*-*-*-*\n";
    std::cout << "Name your Rocket (eg. Apollo11): ";
    std::string name;
    getline(std::cin, name);
    std::cout << "Name your Mission (eg. cargo, satellite, orbit): ";
    std::string mission;
    getline(std::cin, mission);
    std::cout << "Pick your rocket's material (1 for stainless steel, 2 for titanium): ";
    int materialNum;
    std::cin >> materialNum;
    std::string material;
    switch (materialNum)
    {
        case 1:
            material = "stainless steel";
            break;
        case 2:
            material = "titanium";
        default:
            std::cout << "Wrong number";
    }
    std::cout << "Enter your stage count (eg. 2-3): ";
    int stageCount;
    std::cin >> stageCount;
    
    std::cout << "\n\nYour Rocket's details\n";
    std::cout << name;
    std::cout << "\nMission: " << mission;
    std::cout << "\nMaterial: " << material;
    std::cout << "\nStages: " << stageCount;
    
    return 0;
}