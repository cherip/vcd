#include "om_interface.h"
#include "frame.h"
#include "thread_safe_index.h"
#include "vcd_file.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <map>
#include <pthread.h>

vcd::IndexLRU *frame_index;
vcd::VcdFile *omf_file;
char path[128];
int global_jpg_idx;
std::map<const std::string, int> *str2idx;

const char *YUV = "/yuv/";
const char *DB = "/db/";

int find_img_idx(const std::string &key) {
    if (str2idx->find(key) == str2idx->end()) {
        str2idx->insert(std::pair<const std::string, int>(key, global_jpg_idx));
        int ret = global_jpg_idx++;
        return ret;
    } else {
        return (*str2idx)[key];
    }
}

void save_img(const vcd::Frame *frame, int idx, unsigned char *data, int w, int h) {
    const std::string &key = frame->GetOMStr();
    char jpg_name[256];
    
    /*
     * get the name of the yuv
     */
    /* named by the idx */
    //int img_idx = find_img_idx(key);
    //sprintf(jpg_name, "%s%s%d_%d.yuv", path, YUV, img_idx, idx);
    /* named by the feature */
    sprintf(jpg_name, "%s%s%s_%d.yuv", path, YUV, key.c_str(), idx);
    //fprintf(stderr, "%s\n", jpg_name);

    FILE *pf = fopen(jpg_name, "w");
    if (pf == NULL) {
        fprintf(stderr, "Save Img Error!\n");
        return;
    }

    // first write weight and height
    fwrite(&w, sizeof(int), 1, pf);
    fwrite(&h, sizeof(int), 1, pf);

    // then write the data of Img
    fwrite(data, sizeof(unsigned char), w * h * 3 / 2, pf);

    fclose(pf);
}

int open_db(char *db_path) {
    char log[128];
    strcpy(log, db_path);
    strcat(log, "om.log");

    frame_index = new vcd::IndexLRU(1024u * 500);
    strcpy(path, db_path);

    str2idx = new std::map<const std::string, int>();
    global_jpg_idx = 0;

    //new method
    omf_file = new vcd::VcdFile(db_path, "omf");
    return 0;
}

int close_db() {
    delete frame_index; 
    delete omf_file;

	return 0;
}

int query_image(unsigned char *data, int w, int h) {
    vcd::Frame *frame = new vcd::Frame();
    frame->ExtractFeature(data, w, h);

    if (frame_index->InsertThreadSafe(frame) == 0) {
        omf_file->Append(frame->GetOMStr());
    } else {
        delete frame;
    }
    
    return 0;
}


