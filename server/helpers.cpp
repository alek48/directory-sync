#include "helpers.h"

#include <netinet/in.h>

void appendIntToNetworkData(int val, std::vector<char>& data)
{
    int nVal = htonl(val);
    char* networkVal = reinterpret_cast<char*>(&nVal);
    for (int i = 0; i < 4; i++)
        data.push_back(networkVal[i]);
}

void appendStringToNetworkData(const std::string& str, std::vector<char>& data)
{
    for (char c : str)
        data.push_back(c);
}

int readIntFromNetworkData(const char* data)
{
    return ntohl(*reinterpret_cast<const int*>(data));
}