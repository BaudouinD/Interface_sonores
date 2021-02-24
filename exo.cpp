#define RTMIDI17_HEADER_ONLY 1
#include <rtmidi17/reader.hpp>
#include <iostream>
#include <fstream>

int main(void)
{
std::vector<uint8_t> bytes;
std::ifstream file{"/chemin/vers/fichier.mid", std::ios::binary};
bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
rtmidi::reader reader;
reader.parse(bytes);
// reader contient maintenant les informations contenues dans le fichier midi.
}