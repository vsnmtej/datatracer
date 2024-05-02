#ifndef DATATRACER_H
#define DATATRACER_H

#include "modelstats.h"
#include "imagestats.h"

class DataTracer {
public:
    // Image profile functions
    int create_imageProfile(ImageStats &imagestats);
    int delete_imageProfile(ImageStats &imagestats);
    int log_imageProfile(ImageStats &imagestats);
    int send_imageProfile(ImageStats &imagestats);

    // Model profile functions
    int create_modelProfile(ModelStats &modelstats);
    int delete_modelProfile(ModelStats &modelstats);
    int log_modelProfile(ModelStats &modelstats);
    int send_modelProfile(ModelStats &modelstats);
};

// Absoulte base classes
// create_Profile
// delete_Profile
// log_Profile
// send_profile

#endif /* DATATRACER_H */
