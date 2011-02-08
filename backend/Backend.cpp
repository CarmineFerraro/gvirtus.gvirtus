/*
 * gVirtuS -- A GPGPU transparent virtualization component.
 *
 * Copyright (C) 2009-2010  The University of Napoli Parthenope at Naples.
 *
 * This file is part of gVirtuS.
 *
 * gVirtuS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * gVirtuS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gVirtuS; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Written by: Giuseppe Coviello <giuseppe.coviello@uniparthenope.it>,
 *             Department of Applied Science
 */

#include <vector>
#include <cstring>
#include <cstdio>

/**
 * @file   Backend.cpp
 * @author Giuseppe Coviello <giuseppe.coviello@uniparthenope.it>
 * @date   Wed Sep 30 12:29:26 2009
 *
 * @brief
 *
 *
 */

#include "Backend.h"

#include <iostream>

#include "Process.h"
#include <dlfcn.h>
#include <string>
#include <cstring>

using namespace std;

static GetHandler_t LoadModule(const char *name) {
    char path[4096];
    if(*name == '/')
        strcpy(path, name);
    else
        sprintf(path, _PLUGINS_DIR "/lib%s-backend.so", name);

    void *lib = dlopen(path, RTLD_LAZY);
    if(lib == NULL) {
        cerr << "Error loading " << path << ": " << dlerror() << endl;
        return NULL;
    }

    HandlerInit_t init = (HandlerInit_t) ((uint64_t) dlsym(lib, "HandlerInit"));
    if(init == NULL) {
        dlclose(lib);
        cerr << "Error loading " << name << ": HandlerInit function not found."
                << endl;
        return NULL;
    }

    if(init() != 0) {
        dlclose(lib);
        cerr << "Error loading " << name << ": HandlerInit failed."
                << endl;
        return NULL;
    }

    GetHandler_t sym = (GetHandler_t) ((uint64_t) dlsym(lib, "GetHandler"));
    if(sym == NULL) {
        dlclose(lib);
        cerr << "Error loading " << name << ": " << dlerror() << endl;
        return NULL;
    }

    cout << "Loaded module '" << name << "'." << endl;

    return sym;
}

Backend::Backend(vector<string> &plugins) {
    GetHandler_t h;
    for(vector<string>::iterator i = plugins.begin(); i != plugins.end(); i++) {
        if((h = LoadModule((*i).c_str())) != NULL)
            mHandlers.push_back(h);
    }
}

void Backend::Start(Communicator * communicator) {
    communicator->Serve();
    while (true) {
        Communicator *client =
                const_cast<Communicator *> (communicator->Accept());
        vector<Handler *> *h = new vector<Handler *>();
        for(vector<GetHandler_t>::iterator i = mHandlers.begin();
                i != mHandlers.end(); i++)
            h->push_back((*i)());
        Process *process = new Process(client, h);
        process->Start(NULL);
    }
}

void Backend::EventOccurred(std::string& event, void* object) {
    std::cout << "EventOccurred: " << event << std::endl;
}
