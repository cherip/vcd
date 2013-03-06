#include "vcd_dir.h"
#include <dirent.h>
#include <stdlib.h>

namespace vcd {

Dir::Dir(): _namelist(NULL), _is_open(false),
    _file_num(0), _idx(0) {
}

Dir::Dir(const char *path): _namelist(NULL), _is_open(false),
    _file_num(0), _idx(0) {
    OpenDir(path);
}

Dir::~Dir() {
    CloseDir();
}

bool Dir::OpenDir(const char *path) {
    if (path == NULL) {
        return false;
    }

    _file_num = scandir(path, &_namelist, NULL, alphasort);
    if (_file_num < 0) {
        return false;
    }
    _is_open = true;
    _path = std::string(path);

    return true;
}

bool Dir::GetNextFile(std::string *file_name) {
    if (_is_open == false) {
        return false;
    }

    if (_idx >= _file_num) {
        return false;
    }

    *file_name = _path + "/" + std::string(_namelist[_idx]->d_name);
    _idx++;

    return true;
}

bool Dir::GetAllFile(std::vector<std::string> *file_list) {
    if (_is_open == false) {
        return false;
    }

    Reset();
    std::string file;
    while (true) {
        if (GetNextFile(&file) == false) {
            break;
        } 

        file_list->push_back(file);
    }

    return true;
}

bool Dir::Reset() {
    if (_is_open) {
        _idx = 0;
        return true;
    }
    return false;
}

bool Dir::CloseDir() {
    if (_is_open == false) {
        return false;
    }

    for (int i = 0; i < _file_num; ++i) {
        free(_namelist[i]);
    }
    _file_num = 0;

    return true;
}

int Dir::GetFileNum() {
    return _is_open ? _file_num : -1;
}

} // namespace vcd
