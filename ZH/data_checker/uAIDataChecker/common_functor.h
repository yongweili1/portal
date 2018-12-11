
#ifndef UIH_COMMON_FUNTOR_H_
#define UIH_COMMON_FUNTOR_H_

#include <vector>
#include <string>

#include "uai_app_defs.h"

BEGIN_NAMESPACE

std::vector<std::string> Split( const std::string &s, const std::string &seperator);

std::string ChangeTextFromUTF8ToAnsi(std::string& str);

bool ConvertDT(double& dDateTime, tm& tmResult);

bool ConvertDA(unsigned long long& iYYYYMMDD,tm& tmResult);

bool ConvertTM(double& dHHMMSSFF,tm& tmResult);	

END_NAMESPACE

#endif